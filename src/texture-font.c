#include "../include/texture-font.h"
#include "../include/texture-atlas.h"

#include <ft2build.h>
#include <float.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_STROKER_H
// #include FT_ADVANCES_H
#include FT_LCD_FILTER_H
#include FT_TRUETYPE_TABLES_H

#define HRES  64
#define HRESf 64.0f
#define DPI   72

static float
ftgl_convert_F26Dot6_to_float(FT_F26Dot6 value)
{
        return ((float) value) / 64.0;
}

static FT_F26Dot6
ftgl_convert_float_to_F26Dot6(float value)
{
        return (FT_F26Dot6) (value * 64.0);
}

// rol8 ror8

#if (defined(_WIN32) || defined(_WIN64)) && !defined(__GNUC__)
# define inline
static inline __builtin_bswap32(uint32_t in)
{
        return ((in >> 24) & 0xFF) | ((in >> 8) & 0xFF00)
                | ((in & 0xFF00) << 8) | ((in & 0xFF) << 24);
}
#endif

static inline uint32_t rol(uint32_t in, uint32_t x)
{
        return (in >> (32-x)) | (in << x);
}

static void
computegradient(double *img, int w, int h, double *gx, double *gy)
{
        int i, j, k, p, q;
        double glength, phi, phiscaled, ascaled,
                errsign, pfrac, qfrac, err0, err1, err;
#define SQRT2 1.4142136
        for (i = 1; i < h - 1; i++) {
                for (j = 1; j < w - 1; j++) {
                        k = i * w + j;
                        if ((img[k] > 0.0) && (img[k] < 1.0)) {
                                gx[k] = -img[k - w - 1] - SQRT2 * img[k - 1] - img[k + w -1]
                                        + img[k - w + 1] + SQRT2 * img[k + 1] + img[k + w + 1];
                                gy[k] = img[k - w - 1] - SQRT2 * img[k - w] - img[k - w + 1]
                                        + img[k + w - 1] + SQRT2 * img[k + w] + img[k + w + 1];
                                glength = gx[k] * gx[k] + gy[k] * gy[k];
                                if (glength > 0.0) {
                                        glength = sqrt(glength);
                                        gx[k] = gx[k] / glength;
                                        gy[k] = gy[k] / glength;
                                }
                        }
                }
        }
}

static double
edgedf(double gx, double gy, double a)
{
        double df, glength, temp, a1;

        if ((gx == 0) || (gy == 0)) { // Either A) gu or gv are zero, or B) both
                df = 0.5-a;  // Linear approximation is A) correct or B) a fair guess
        } else {
                glength = sqrt(gx*gx + gy*gy);
                if(glength>0) {
                        gx = gx/glength;
                        gy = gy/glength;
                }
                /* Everything is symmetric wrt sign and transposition,
                 * so move to first octant (gx>=0, gy>=0, gx>=gy) to
                 * avoid handling all possible edge directions.
                 */
                gx = fabs(gx);
                gy = fabs(gy);
                if(gx<gy) {
                        temp = gx;
                        gx = gy;
                        gy = temp;
                }
                a1 = 0.5*gy/gx;
                if (a < a1) { // 0 <= a < a1
                        df = 0.5*(gx + gy) - sqrt(2.0*gx*gy*a);
                } else if (a < (1.0-a1)) { // a1 <= a <= 1-a1
                        df = (0.5-a)*gx;
                } else { // 1-a1 < a <= 1
                        df = -0.5*(gx + gy) + sqrt(2.0*gx*gy*(1.0-a));
                }
        }
        return df;
}

static double
distaa3(double *img, double *gximg, double *gyimg, int w, int c,
        int xc, int yc, int xi, int yi)
{
        double di, df, dx, dy, gx, gy, a;
        int closest;

        closest = c-xc-yc*w; // Index to the edge pixel pointed to from c
        a = img[closest];    // Grayscale value at the edge pixel
        gx = gximg[closest]; // X gradient component at the edge pixel
        gy = gyimg[closest]; // Y gradient component at the edge pixel

        if(a > 1.0) a = 1.0;
        if(a < 0.0) a = 0.0; // Clip grayscale values outside the range [0,1]
        if(a == 0.0) return 1000000.0; // Not an object pixel, return "very far" ("don't know yet")

        dx = (double)xi;
        dy = (double)yi;
        di = sqrt(dx*dx + dy*dy); // Length of integer vector, like a traditional EDT
        if(di==0) { // Use local gradient only at edges
                // Estimate based on local gradient only
                df = edgedf(gx, gy, a);
        } else {
                // Estimate gradient based on direction to edge (accurate for large di)
                df = edgedf(dx, dy, a);
        }
        return di + df; // Same metric as edtaa2, except at edges (where di=0)
}

#define DISTAA(c,xc,yc,xi,yi) (distaa3(img, gx, gy, w, c, xc, yc, xi, yi))

static void
edtaa3(double *img, double *gx, double *gy, int w, int h,
       short *distx, short *disty, double *dist)
{
        int x, y, i, c;
        int offset_u, offset_ur, offset_r, offset_rd,
                offset_d, offset_dl, offset_l, offset_lu;
        double olddist, newdist;
        int cdistx, cdisty, newdistx, newdisty;
        int changed;
        double epsilon = 1e-3;

        /* Initialize index offsets for the current image width */
        offset_u = -w;
        offset_ur = -w+1;
        offset_r = 1;
        offset_rd = w+1;
        offset_d = w;
        offset_dl = w-1;
        offset_l = -1;
        offset_lu = -w-1;

        /* Initialize the distance images */
        for(i=0; i<w*h; i++) {
                distx[i] = 0; // At first, all pixels point to
                disty[i] = 0; // themselves as the closest known.
                if (img[i] <= 0.0) {
                        dist[i]= 1000000.0; // Big value, means "not set yet"
                } else if (img[i]<1.0) {
                        dist[i] = edgedf(gx[i], gy[i], img[i]); // Gradient-assisted estimate
                } else {
                        dist[i]= 0.0; // Inside the object
                }
        }

        /* Perform the transformation */
        do {
                changed = 0;

                /* Scan rows, except first row */
                for(y=1; y<h; y++) {

                        /* move index to leftmost pixel of current row */
                        i = y*w;

                        /* scan right, propagate distances from above & left */

                        /* Leftmost pixel is special, has no left neighbors */
                        olddist = dist[i];
                        if(olddist > 0) { // If non-zero distance or not set yet 
                                c = i + offset_u; // Index of candidate for testing
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_ur;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }
                        i++;

                        /* Middle pixels have all neighbors */
                        for(x=1; x<w-1; x++, i++) {
                                olddist = dist[i];
                                if(olddist <= 0) continue; // No need to update further

                                c = i+offset_l;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_lu;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_u;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_ur;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }

                        /* Rightmost pixel of row is special, has no right neighbors */
                        olddist = dist[i];
                        if(olddist > 0) { // If not already zero distance
                                c = i+offset_l;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_lu;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_u;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx;
                                newdisty = cdisty+1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }

                        /* Move index to second rightmost pixel of current row. */
                        /* Rightmost pixel is skipped, it has no right neighbor. */
                        i = y*w + w-2;

                        /* scan left, propagate distance from right */
                        for(x=w-2; x>=0; x--, i--) {
                                olddist = dist[i];
                                if(olddist <= 0) continue; // Already zero distance

                                c = i+offset_r;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }
                }

                /* Scan rows in reverse order, except last row */
                for(y=h-2; y>=0; y--) {
                        /* move index to rightmost pixel of current row */
                        i = y*w + w-1;

                        /* Scan left, propagate distances from below & right */

                        /* Rightmost pixel is special, has no right neighbors */
                        olddist = dist[i];
                        if(olddist > 0) { // If not already zero distance
                                c = i+offset_d;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_dl;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }
                        i--;

                        /* Middle pixels have all neighbors */
                        for(x=w-2; x>0; x--, i--) {
                                olddist = dist[i];
                                if(olddist <= 0) continue; // Already zero distance

                                c = i+offset_r;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_rd;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_d;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_dl;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }
                        /* Leftmost pixel is special, has no left neighbors */
                        olddist = dist[i];
                        if(olddist > 0) { // If not already zero distance
                                c = i+offset_r;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_rd;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx-1;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        olddist=newdist;
                                        changed = 1;
                                }

                                c = i+offset_d;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx;
                                newdisty = cdisty-1;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if(newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }

                        /* Move index to second leftmost pixel of current row. */
                        /* Leftmost pixel is skipped, it has no left neighbor. */
                        i = y*w + 1;
                        for(x=1; x<w; x++, i++) {
                                /* scan right, propagate distance from left */
                                olddist = dist[i];
                                if(olddist <= 0) continue; // Already zero distance

                                c = i+offset_l;
                                cdistx = distx[c];
                                cdisty = disty[c];
                                newdistx = cdistx+1;
                                newdisty = cdisty;
                                newdist = DISTAA(c, cdistx, cdisty, newdistx, newdisty);
                                if (newdist < olddist-epsilon) {
                                        distx[i]=newdistx;
                                        disty[i]=newdisty;
                                        dist[i]=newdist;
                                        changed = 1;
                                }
                        }
                }
        } while(changed); // Sweep until no more updates are made

        /* The transformation is completed. */
}

static double *
make_distance_mapd(double *data, unsigned int width, unsigned int height)
{
        short *xdist = malloc(width * height * sizeof(*xdist));
        short *ydist = malloc(width * height * sizeof(*ydist));
        double *gx = calloc(width * height, sizeof(*gx));
        double *gy = calloc(width * height, sizeof(*gy));
        double *outside = calloc(width * height, sizeof(*outside));
        double *inside = calloc(width * height, sizeof(*inside));
        double vmin = DBL_MAX;
        unsigned int i;

        computegradient(data, width, height, gx, gy);
        edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
        for (i = 0; i < width * height; ++i) {
                if (outside[i] < 0.0)
                        outside[i] = 0.0;
        }

        memset(gx, 0, sizeof(double) * width * height);
        memset(gy, 0, sizeof(double) * width * height);
        for (i = 0; i < width * height; ++i) {
                data[i] = 1 - data[i];
        }
        
        computegradient(data, width, height, gx, gy);
        edtaa3(data, gx, gy, width, height, xdist, ydist, inside);
        for (i = 0; i < width * height; ++i) {
                if (inside[i] < 0) {
                        inside[i] = 0.0;
                }
        }

        for (i = 0; i < width * height; ++i) {
                outside[i] -= inside[i];
                if (outside[i] < vmin) {
                        vmin = outside[i];
                }
        }

        vmin = fabs(vmin);

        for (i = 0; i < width * height; ++i) {
                double v = outside[i];
                if (v < -vmin) {
                        outside[i] = -vmin;
                } else if (v > +vmin) {
                        outside[i] = +vmin;
                }

                data[i] = (outside[i] + vmin) / (2 * vmin);
        }

        free(xdist);
        free(ydist);
        free(gx);
        free(gy);
        free(outside);
        free(inside);
        return data;
}

static unsigned char *
make_distance_mapb(unsigned char *img,
                   unsigned int width, unsigned int height)
{
        double *data = calloc(width * height, sizeof(*data));
        unsigned char *out = malloc(width * height * sizeof(*out));
        unsigned int i;

        double img_min = DBL_MAX;
        double img_max = DBL_MIN;

        for (i = 0; i < width * height; ++i) {
                double v = img[i];
                data[i] = v;
                if (v > img_max) {
                        img_max = v;
                }

                if (v < img_min) {
                        img_min = v;
                }
        }

        for (i = 0; i < width * height; ++i) {
                data[i] = (img[i] - img_min) / img_max;
        }

        data = make_distance_mapd(data, width, height);

        for (i = 0; i < width * height; ++i) {
                out[i] = (unsigned char) (255 * (1 - data[i]));
        }

        free(data);
        return out;
}

static size_t
utf8_surrogate_len(const char *character)
{
        size_t result = 0;
        char test_char;

        if (!character)
                return 0;

        test_char = character[0];

        if ((test_char & 0x80) == 0)
                return 1;

        while (test_char & 0x80) {
                test_char <<= 1;
                result++;
        }

        return result;
}

static size_t
utf8_strlen(const char *string)
{
        const char *ptr = string;
        size_t result = 0;

        while (*ptr) {
                ptr += utf8_surrogate_len(ptr);
                result++;
        }


        return result;
}

static uint32_t
utf8_to_utf32(const char *character)
{
        if( !character ) {
                return -1;
        }

        if ((character[0] & 0x80) == 0x0) {
                return character[0];
        }

        if ((character[0] & 0xE0) == 0xC0) {
                return ((character[0] & 0x3F) << 6) | (character[1] & 0x3F);
        }

        if ((character[0] & 0xF0) == 0xE0) {
                return ((character[0] & 0x1F) << (6 + 6)) | ((character[1] & 0x3F) << 6)
                        | (character[2] & 0x3F);
        }

        if ((character[0] & 0xF8) == 0xF0) {
                return ((character[0] & 0x0F) << (6 + 6 + 6))
                        | ((character[1] & 0x3F) << (6 + 6))
                        | ((character[2] & 0x3F) << 6)
                        | (character[3] & 0x3F);
        }

        if ((character[0] & 0xFC) == 0xF8) {
                return ((character[0] & 0x07) << (6 + 6 + 6 + 6))
                        | ((character[1] & 0x3F) << (6 + 6 + 6))
                        | ((character[2] & 0x3F) << (6 + 6 ))
                        | ((character[3] & 0x3F) << 6)
                        | (character[4] & 0x3F);
        }

        return 0xFFFD; // invalid character
}

__THREAD ftgl_texture_font_library_t *freetype_gl_library = NULL;
__THREAD ftgl_font_mode_t ftgl_mode_default = FTGL_MODE_FREE_CLOSE;

void
ftgl_texture_font_default_mode(ftgl_font_mode_t mode)
{
        ftgl_mode_default = mode;
}

ftgl_texture_font_library_t *
ftgl_texture_font_library_create(void)
{
        ftgl_texture_font_library_t *library = calloc(1, sizeof(*library));
        if (!library) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate library!");
#endif /* FTGL_DEBUG */
                return NULL;
        }

        library->mode = FTGL_MODE_ALWAYS_OPEN;
        return library;
}

static ftgl_return_t
ftgl_texture_font_set_size(ftgl_texture_font_t *font,
                           float size)
{
        FT_Error error = 0;
        FT_Matrix matrix = {
                (int)((1.0/HRES) * 0x10000L),
                (int)((0.0)      * 0x10000L),
                (int)((0.0)      * 0x10000L),
                (int)((1.0)      * 0x10000L)
        };

        if (FT_HAS_FIXED_SIZES(font->face)) {
                if (font->face->num_fixed_sizes == 0) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE("No fixed size in font!");
#endif /* FTGL_DEBUG */
                        return FTGL_NO_FIXED_SIZE_FONT_ERROR;
                }

                int best_match = 0;
                float diff = 1e20;
                int i;

                for (i = 0; i < font->face->num_fixed_sizes; ++i) {
                        float new_size = ftgl_convert_F26Dot6_to_float(font->face->available_sizes[i].size);
                        float ndiff = size > new_size ? size / new_size : new_size / size;
                        if (ndiff < diff) {
                                best_match = i;
                                diff = ndiff;
                        }
                }

                error = FT_Select_Size(font->face, best_match);
                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        return FTGL_FREETYPE_ERROR;
                }

                font->scale = font->size / ftgl_convert_F26Dot6_to_float(font->face->available_sizes[best_match].size);
        } else {
                error = FT_Set_Char_Size(font->face, ftgl_convert_float_to_F26Dot6(size), 0, DPI * HRES, DPI);
                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        return FTGL_FREETYPE_ERROR;
                }
        }

        FT_Set_Transform(font->face, &matrix, NULL);
        return FTGL_NO_ERROR;
}

static void
ftgl_texture_font_init_size(ftgl_texture_font_t *font)
{
        FT_Size_Metrics metrics;

#ifdef FTGL_DEBUG
        assert(font);
        assert(font->face);
#endif /* FTGL_DEBUG */

        font->underline_position = font->face->underline_position
                / (float)(HRESf*HRESf) * font->size;
        
        font->underline_position = roundf(font->underline_position);
        if (font->underline_position > -2) {
                font->underline_position = -2.0;
        }

        font->underline_thickness = font->face->underline_thickness
                / (float) (HRESf*HRESf) * font->size;
        font->underline_thickness = roundf(font->underline_thickness);
        if (font->underline_thickness < 1) {
                font->underline_thickness = 1.0;
        }

        metrics = font->face->size->metrics;
        font->ascender  = metrics.ascender  >> 6;
        font->descender = metrics.descender >> 6;
        font->height    = metrics.height    >> 6;
        font->linegap   = font->height - font->ascender + font->descender;
}

static ftgl_return_t
ftgl_texture_font_init(ftgl_texture_font_t *font)
{
        ftgl_return_t ret;
#ifdef FTGL_DEBUG
        assert(font->atlas);
        assert(font->size > 0);
        assert((font->location == FTGL_TEXTURE_FONT_FILE && font->filename)
               || (font->location == FTGL_TEXTURE_FONT_MEMORY
                   && font->memory.base && font->memory.size));
#endif /* FTGL_DEBUG */
        
        font->glyphs = ftgl_vector_create(sizeof(ftgl_texture_glyph_t *));
        if (!font->glyphs) {
                return FTGL_MEMORY_ERROR;
        }
        
        font->height = 0;
        font->height = 0;
        font->ascender = 0;
        font->descender = 0;
        font->linegap = 0;
        font->rendermode = FTGL_RENDER_NORMAL;
        font->outline_thickness = 0.0;
        font->hinting = 1;
        font->kerning = 1;
        font->filtering = 1;
        font->scaletex = 1;
        font->scale = 1.0;

        font->lcd_weights[0] = 0x10;
        font->lcd_weights[1] = 0x40;
        font->lcd_weights[2] = 0x70;
        font->lcd_weights[3] = 0x40;
        font->lcd_weights[4] = 0x10;


        if ((ret = ftgl_texture_font_load_face(font, font->size)) != FTGL_NO_ERROR)
                return ret;

        ftgl_texture_font_init_size(font);
        
        if ((ret = ftgl_texture_font_set_size(font, font->size)) != FTGL_NO_ERROR)
                return ret;

        ftgl_texture_font_get_glyph(font, NULL);
        return FTGL_NO_ERROR;
}

ftgl_texture_font_t *
ftgl_texture_font_new_from_file(ftgl_texture_atlas_t *atlas,
                                const float pt_size,
                                const char *filename)
{
        ftgl_texture_font_t *font;

#ifdef FTGL_DEBUG
        assert(atlas);
        assert(pt_size > 0.0);
        assert(filename);
#endif /* FTGL_DEBUG */

        font = calloc(1, sizeof(*font));
        if (!font) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate font!");
#endif /* FTGL_DEBUG */
                return NULL;
        }
        
        font->atlas = atlas;
        font->size = pt_size;
        
        font->location = FTGL_TEXTURE_FONT_FILE;
        font->filename = strdup(filename);
        font->mode = ftgl_mode_default;

        if (ftgl_texture_font_init(font) != FTGL_NO_ERROR) {
                ftgl_texture_font_free(font);
                return NULL;
        }
        
        return font;
}

ftgl_texture_font_t *
ftgl_texture_font_new_from_memory(ftgl_texture_atlas_t *atlas,
                                  float pt_size,
                                  const void *memory_base,
                                  size_t memory_size)
{
        ftgl_texture_font_t *font;
#ifdef FTGL_DEBUG
        assert(atlas);
        assert(memory_base);
        assert(memory_size);
        assert(pt_size > 0.0);
#endif /* FTGL_DEBUG */

        font = calloc(1, sizeof(*font));
        if (!font) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate font!");
#endif /* FTGL_DEBUG */
                return NULL;
        }

        font->atlas = atlas;
        font->size = pt_size;

        font->location = FTGL_TEXTURE_FONT_MEMORY;
        font->memory.base = memory_base;
        font->memory.size = memory_size;
        font->mode = ftgl_mode_default;

        if (ftgl_texture_font_init(font) != FTGL_NO_ERROR) {
                ftgl_texture_font_free(font);
                return NULL;
        }

        return font;
}

ftgl_texture_font_t *
ftgl_texture_font_copy(ftgl_texture_font_t *font,
                       float pt_size)
{
        ftgl_texture_font_t *new_font;
        FT_Error error = 0;
        float native_size = font->size / font->scale;

        new_font = calloc(1, sizeof(*new_font));
        if (!new_font) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate font!");
#endif /* FTGL_DEBUG */
                return NULL;                
        }

        memcpy(new_font, font, sizeof(*new_font));
        new_font->size = pt_size;

        error = FT_New_Size(new_font->face, &new_font->ft_size);
        if (error) {
#ifdef FTGL_DEBUG
                FT_Error_String(error);
#endif /* FTGL_DEBUG */
                free(new_font);
                return NULL;
        }

        error = FT_Activate_Size(new_font->ft_size);
        if (error) {
#ifdef FTGL_DEBUG
                FT_Error_String(error);
#endif /* FTGL_DEBUG */
                free(new_font);
                return NULL;
        }

        if (ftgl_texture_font_set_size(new_font, pt_size) != FTGL_NO_ERROR) {
                free(new_font);
                return NULL;
        }

        ftgl_texture_font_init_size(new_font);

        if (new_font->size / new_font->scale != native_size) {
                new_font->glyphs = ftgl_vector_create(sizeof(ftgl_texture_glyph_t *));
                if (!new_font->glyphs) {
                        return NULL;
                }
        }
        
        return new_font;
}

void
ftgl_texture_font_close(ftgl_texture_font_t *font,
                        ftgl_font_mode_t face_mode,
                        ftgl_font_mode_t library_mode)
{
        if (font->face && font->mode <= face_mode) {
                FT_Done_Face(font->face);
                font->face = NULL;
        } else {
                return;
        }

        if (font->library && font->library->library
            && font->library->mode <= library_mode) {
                FT_Done_FreeType(font->library->library);
                font->library->library = NULL;
        }
}

void
ftgl_texture_font_free(ftgl_texture_font_t *font)
{
        size_t i;
        ftgl_texture_glyph_t *glyph;
        FT_Error error = 0;

#ifdef FTGL_DEBUG
        assert(font);
#endif /* FTGL_DEBUG */

        error = FT_Done_Size(font->ft_size);
        if (error) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
        }

        ftgl_texture_font_close(font, FTGL_MODE_ALWAYS_OPEN, FTGL_MODE_FREE_CLOSE);

        if (font->location == FTGL_TEXTURE_FONT_FILE && font->filename)
                free(font->filename);

        for (i = 0; i < ftgl_vector_size(font->glyphs); i++) {
                ftgl_texture_glyph_t **__glyphs;
                if ((__glyphs = *(ftgl_texture_glyph_t ***) ftgl_vector_get(font->glyphs, i))) {
                        int __i;
                        for (__i = 0; __i < 0x100; __i++) {
                                if ((glyph = __glyphs[__i])) {
                                        ftgl_texture_glyph_free(glyph);
                                }
                        }
                        free(__glyphs);
                }
        }

        ftgl_vector_free(font->glyphs);
        free(font);
}

ftgl_return_t
ftgl_texture_font_load_face(ftgl_texture_font_t *font,
                            float size)
{
        FT_Error error;
        if (!font->library) {
                if (!freetype_gl_library) {
                        freetype_gl_library = ftgl_texture_font_library_create();
                        if (!freetype_gl_library) {
                                return FTGL_MEMORY_ERROR;
                        }
                }
                font->library = freetype_gl_library;
        }

        if (!font->library->library) {
                error = FT_Init_FreeType(&font->library->library);
                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup;
                }
        }

        if (!font->face) {
                switch (font->location) {
                case FTGL_TEXTURE_FONT_FILE:
                        error = FT_New_Face(font->library->library, font->filename, 0, &font->face);
                        if (error) {
#ifdef FTGL_DEBUG
                                FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                                goto cleanup_library;
                        }
                        break;
                case FTGL_TEXTURE_FONT_MEMORY:
                        error = FT_New_Memory_Face(font->library->library,
                                                   font->memory.base,  font->memory.size, 0, &font->face);
                        if (error) {
#ifdef FTGL_DEBUG
                                FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                                goto cleanup_library;
                        }
                        break;
                }

                error = FT_Select_Charmap(font->face, FT_ENCODING_UNICODE);
                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup_face;
                }

                error = FT_New_Size(font->face, &font->ft_size);
                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup_face;
                }

                error = FT_Activate_Size(font->ft_size);
                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup_face;
                }

                if (ftgl_texture_font_set_size(font, size))
                        goto cleanup_face;
        }

        return FTGL_NO_ERROR;

 cleanup_face:
        ftgl_texture_font_close(font, FTGL_MODE_ALWAYS_OPEN, FTGL_MODE_FREE_CLOSE);
        return FTGL_FREETYPE_ERROR;
 cleanup_library:
        ftgl_texture_font_close(font, FTGL_MODE_ALWAYS_OPEN, FTGL_MODE_ALWAYS_OPEN);
 cleanup:
        return FTGL_FREETYPE_ERROR;
}

static ftgl_texture_glyph_t *
ftgl_texture_font_find_glyph(ftgl_texture_font_t *font,
                             const char *codepoint)
{
        if (!codepoint)
                return (ftgl_texture_glyph_t *) font->atlas->special;

        return ftgl_texture_font_find_glyph_gi(font, utf8_to_utf32(codepoint));
}

ftgl_texture_glyph_t *
ftgl_texture_font_get_glyph(ftgl_texture_font_t *font,
                            const char *codepoint)
{
        ftgl_texture_glyph_t *glyph;

#ifdef FTGL_DEBUG
        assert(font);
        assert(font->filename);
        assert(font->atlas);
#endif /* FTGL_DEBUG */

        glyph = NULL;
        if (!(glyph = ftgl_texture_font_find_glyph(font, codepoint))) {
                if (ftgl_texture_font_load_glyph(font, codepoint)) {
                        glyph = ftgl_texture_font_find_glyph(font, codepoint);
                }
        }

        return glyph;
}

ftgl_return_t
ftgl_texture_font_index_glyph(ftgl_texture_font_t *font,
                              ftgl_texture_glyph_t *glyph,
                              uint32_t codepoint)
{
        ftgl_return_t ret;
        uint32_t i = codepoint >> 8;
        uint32_t j = codepoint & 0xFF;
        ftgl_texture_glyph_t ***glyph_index1, *glyph_insert;

        if (font->glyphs->size <= i) {
                if ((ret = ftgl_vector_resize(font->glyphs, i+1)) != FTGL_NO_ERROR)
                        return ret;
        }

        glyph_index1 = (ftgl_texture_glyph_t ***) ftgl_vector_get(font->glyphs, i);

        if (!*glyph_index1) {
                *glyph_index1 = calloc(0x100, sizeof(ftgl_texture_glyph_t *));
                if (!*glyph_index1) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE("Failed to allocate glyph index!");
#endif /* FTGL_DEBUG */
                        return FTGL_MEMORY_ERROR;
                }
        }

        if ((glyph_insert = (*glyph_index1)[j])) {
                int i = 0;
                while (glyph_insert[i].glyphmode != FTGL_GLYPH_END)
                        i++;
                glyph_insert[i].glyphmode = FTGL_GLYPH_CONT;
                (*glyph_index1)[j] = glyph_insert = realloc(glyph_insert,
                                                            sizeof(ftgl_texture_glyph_t)*(i+2));
                memcpy(glyph_insert+(i+1), glyph, sizeof(ftgl_texture_glyph_t));
                return FTGL_NO_ERROR /* FTGL_GLYPH_ALREADY_EXISTS */;
        } else {
                (*glyph_index1)[j] = glyph;
                return FTGL_NO_ERROR;
        }

        return FTGL_NO_ERROR;
}

ftgl_return_t
ftgl_texture_font_load_glyph(ftgl_texture_font_t *font,
                             const char *codepoint)
{
        if (!codepoint)
                return FTGL_NO_ERROR;

        uint32_t ucodepoint = utf8_to_utf32(codepoint);
        return ftgl_texture_font_load_glyph_gi(font,
                                               FT_Get_Char_Index(font->face, ucodepoint),
                                               ucodepoint);
}

ftgl_texture_glyph_t *
ftgl_texture_font_get_glyph_gi(ftgl_texture_font_t *font,
                               uint32_t glyph_index)
{
        ftgl_texture_glyph_t *glyph;

#ifdef FTGL_DEBUG
        assert(font);
        assert(font->filename);
        assert(font->atlas);
#endif /* FTGL_DEBUG */

        if ((glyph = ftgl_texture_font_find_glyph_gi(font, glyph_index)))
                return glyph;

        if (ftgl_texture_font_load_glyph_gi(font, glyph_index, glyph_index))
                return ftgl_texture_font_find_glyph_gi(font, glyph_index);

        return NULL;
}

ftgl_texture_glyph_t *
ftgl_texture_font_find_glyph_gi(ftgl_texture_font_t *font,
                                uint32_t codepoint)
{
        uint32_t i = codepoint >> 8;
        uint32_t j = codepoint & 0xFF;
        ftgl_texture_glyph_t **glyph_index1, *glyph;

        if (font->glyphs->size <= i)
                return NULL;

        glyph_index1 = *(ftgl_texture_glyph_t ***) ftgl_vector_get(font->glyphs, i);

        if (!glyph_index1) {
                return NULL;
        } else {
                glyph = glyph_index1[j];
        }

        while (glyph && (glyph->rendermode != font->rendermode ||
                         glyph->outline_thickness != font->outline_thickness)) {
                if (glyph->glyphmode != FTGL_GLYPH_CONT)
                        return NULL;
                glyph++;
        }
        
        return glyph;
}

static void
ftgl_texture_font_index_kerning(ftgl_texture_glyph_t *glyph,
                                uint32_t codepoint,
                                float kerning)
{
        uint32_t i = codepoint >> 8;
        uint32_t j = codepoint & 0xFF;
        float **kerning_index;

        if (glyph->kerning->size <= i) {
                ftgl_vector_resize(glyph->kerning, i+1);
        }

        kerning_index = (float **) ftgl_vector_get(glyph->kerning, i);

        if (!*kerning_index) {
                *kerning_index = calloc(0x100, sizeof(float));
        }

        (*kerning_index)[j] = kerning;
}

static void
ftgl_texture_font_generate_kerning(ftgl_texture_font_t *font,
                                   FT_Library *library, FT_Face *face)
{
        size_t i, j, k;
        FT_UInt glyph_index, prev_index;
        ftgl_texture_glyph_t *glyph, *prev_glyph;
        FT_Vector kerning;

#ifdef FTGL_DEBUG
        assert(font);
#endif /* FTGL_DEBUG */
        for (i = 0; i < ftgl_vector_size(font->glyphs); i++) {
                ftgl_texture_glyph_t **__glyphs;
                if ((__glyphs = *(ftgl_texture_glyph_t ***) ftgl_vector_get(font->glyphs, i))) {
                        int __i;
                        for (__i = 0; __i < 0x100; __i++) {
                                if ((glyph = __glyphs[__i])) {
                                        glyph_index = FT_Get_Char_Index(*face, glyph->codepoint);

                                        for (k = 0; k < glyph->kerning->size; k++) {
                                                free( *(float **) ftgl_vector_get(glyph->kerning, k));
                                        }
                                        ftgl_vector_clear(glyph->kerning);

                                        for (j = 0; j < ftgl_vector_size(font->glyphs); j++) {
                                                ftgl_texture_glyph_t **__prev_glyphs;
                                                if ((__prev_glyphs = *(ftgl_texture_glyph_t ***) ftgl_vector_get(font->glyphs, j))) {
                                                        int __j;
                                                        for (__j = 0; __j < 0x100; __j++) {
                                                                if ((prev_glyph = __prev_glyphs[__j])) {
                                                                        prev_index = FT_Get_Char_Index(*face,
                                                                                                       prev_glyph->codepoint);
                                                                        FT_Get_Kerning(*face, prev_index, glyph_index,
                                                                                       FT_KERNING_UNFITTED, &kerning);

                                                                        if (kerning.x) {
                                                                                ftgl_texture_font_index_kerning(glyph,
                                                                                                                prev_glyph->codepoint,
                                                                                                                ftgl_convert_F26Dot6_to_float(kerning.x) / HRESf);
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

int
ftgl_texture_font_load_glyph_gi(ftgl_texture_font_t *font,
                                uint32_t glyph_index,
                                uint32_t ucodepoint)
{
        size_t i, x, y;

        FT_Error error;
        FT_Face face;
        FT_Glyph ft_glyph = NULL;
        FT_GlyphSlot slot;
        FT_Bitmap ft_bitmap;

        ftgl_texture_glyph_t *glyph;
        FT_Int32 flags = 0;
        int ft_glyph_top = 0;
        int ft_glyph_left = 0;

        ivec4_t region;
        size_t missed = 0;

        if (ftgl_texture_font_find_glyph_gi(font, ucodepoint))
                return 1;

        if (!ftgl_texture_font_load_face(font, font->size))
                return 0;

        flags = 0;
        ft_glyph_top = 0;
        ft_glyph_left = 0;
        if (!glyph_index) {
                ftgl_texture_glyph_t *glyph;
                if ((glyph = ftgl_texture_font_find_glyph(font, "\0"))) {
                        ftgl_texture_font_index_glyph(font, glyph, ucodepoint);
                        ftgl_texture_font_close(font, FTGL_MODE_AUTO_CLOSE, FTGL_MODE_AUTO_CLOSE);

                        return 1;
                }
        }

        if (font->rendermode != FTGL_RENDER_NORMAL
            && font->rendermode != FTGL_RENDER_SIGNED_DISTANCE_FIELD) {
                flags |= FT_LOAD_NO_BITMAP;
        } else {
                flags |= FT_LOAD_RENDER;
        }

        if (!font->hinting) {
                flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
        } else {
                flags |= FT_LOAD_FORCE_AUTOHINT;
        }

        if (font->atlas->depth == 3) {
                FT_Library_SetLcdFilter(font->library->library, FT_LCD_FILTER_LIGHT);
                flags |= FT_LOAD_TARGET_LCD;

                if (font->filtering) {
                        FT_Library_SetLcdFilterWeights(font->library->library, font->lcd_weights);
                }
        } else if (HRES == 1) {
                flags |= FT_LOAD_TARGET_LIGHT;
        }

        if (font->atlas->depth == 4) {
#ifdef FT_LOAD_COLOR
                flags |= FT_LOAD_COLOR;
#else
#ifdef FTGL_DEBUG 
                FTGL_LOG_MESSAGE("Load colour not available for font!");
#endif /* FTGL_DEBUG */
#endif /* FT_LOAD_COLOR */
        }

        error = FT_Activate_Size(font->ft_size);
        if (error) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                return 0;
        }

        error = FT_Load_Glyph(font->face, glyph_index, flags);
        if (error) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                ftgl_texture_font_close(font, FTGL_MODE_AUTO_CLOSE, FTGL_MODE_AUTO_CLOSE);
                return 0;
        }

        if (font->rendermode == FTGL_RENDER_NORMAL
            || font->rendermode == FTGL_RENDER_SIGNED_DISTANCE_FIELD) {
                slot          = font->face->glyph;
                ft_bitmap     = slot->bitmap;
                ft_glyph_top  = slot->bitmap_top;
                ft_glyph_left = slot->bitmap_left;
        } else {
                FT_Stroker stroker;
                FT_BitmapGlyph ft_bitmap_glyph;

                error = FT_Stroker_New(font->library->library, &stroker);

                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup_stroker;
                }

                FT_Stroker_Set(stroker,
                               (int) (font->outline_thickness * HRES),
                               FT_STROKER_LINECAP_ROUND,
                               FT_STROKER_LINEJOIN_ROUND,
                               0);

                error = FT_Get_Glyph(font->face->glyph, &ft_glyph);

                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */                        
                        goto cleanup_stroker;
                }

                if (font->rendermode == FTGL_RENDER_OUTLINE_EDGE) {
                        error = FT_Glyph_Stroke(&ft_glyph, stroker, 1);
                } else if (font->rendermode == FTGL_RENDER_OUTLINE_POSITIVE) {
                        error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 0, 1);
                } else if (font->rendermode == FTGL_RENDER_OUTLINE_NEGATIVE) {
                        error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 1, 1);
                }

                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup_stroker;
                }

                switch (font->atlas->depth) {
                case 1:
                        error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
                        break;
                case 3:
                        error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_LCD, 0, 1);
                        break;
                case 4:
                        error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
                        break;
                }

                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        goto cleanup_stroker;
                }

                ft_bitmap_glyph = (FT_BitmapGlyph) ft_glyph;
                ft_bitmap       = ft_bitmap_glyph->bitmap;
                ft_glyph_top    = ft_bitmap_glyph->top;
                ft_glyph_left   = ft_bitmap_glyph->left;

        cleanup_stroker:
                FT_Stroker_Done(stroker);

                if (error) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE(FT_Error_String(error));
#endif /* FTGL_DEBUG */
                        return 0;
                }
        }

        struct {
                int left;
                int top;
                int right;
                int bottom;
        } padding = { 0, 0, 1, 1 };

        if (font->rendermode == FTGL_RENDER_SIGNED_DISTANCE_FIELD) {
                padding.top = 1;
                padding.left = 1;
        }

        if (font->padding != 0) {
                padding.top += font->padding;
                padding.left += font->padding;
                padding.right += font->padding;
                padding.bottom += font->padding;
        }

        size_t src_w = font->atlas->depth == 3 ? ft_bitmap.width / 3 : ft_bitmap.width;
        size_t src_h = ft_bitmap.rows;

        size_t tgt_w = src_w + padding.left + padding.right;
        size_t tgt_h = src_h + padding.top + padding.bottom;

        region = ftgl_texture_atlas_get_region(font->atlas, tgt_w, tgt_h);

        if (region.x < 0) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Texture Atlas Full");
#endif /* FTGL_DEBUG */
                ftgl_texture_font_close(font, FTGL_MODE_AUTO_CLOSE, FTGL_MODE_AUTO_CLOSE);
                return 0;
        }

        x = region.x;
        y = region.y;

        unsigned char *buffer = calloc(tgt_w * tgt_h * font->atlas->depth, sizeof(*buffer));

        unsigned char *dst_ptr = buffer + (padding.top * tgt_w + padding.left) * font->atlas->depth;
        unsigned char *src_ptr = ft_bitmap.buffer;
        if (ft_bitmap.pixel_mode == FT_PIXEL_MODE_BGRA && font->atlas->depth == 4) {
                for (i = 0; i < src_h; i++) {
                        int j;
                        for (j = 0; j < ft_bitmap.width; j++) {
                                uint32_t bgra, rgba;
                                bgra = ((uint32_t *) src_ptr)[j];
#if __BYTE_ORDER == __BIG_ENDIAN
                                rgba = rol(__builtin_bswap32(bgra), 8);
#else
                                rgba = rol(__builtin_bswap32(bgra), 24);
#endif
                                ((uint32_t *) dst_ptr)[j] = rgba;
                        }
                        dst_ptr += tgt_w * font->atlas->depth;
                        src_ptr += ft_bitmap.pitch;
                }
        } else if (ft_bitmap.pixel_mode == FT_PIXEL_MODE_BGRA && font->atlas->depth == 1) {
                struct src_pixel_t {
                        uint8_t b;
                        uint8_t g;
                        uint8_t r;
                        uint8_t a;
                } * src = (struct src_pixel_t *) ft_bitmap.buffer;

                for (int row = 0; row < src_h; row++, dst_ptr += tgt_w * font->atlas->depth) {
                        for (int col = 0; col < src_w; col++, src++) {
                                dst_ptr[col] = (0.3 * src->r + 0.59 * src->g + 0.11 * src->b)
                                        * (src->a / 255.0);
                        }
                }
        } else if (ft_bitmap.pixel_mode == FT_PIXEL_MODE_GRAY && font->atlas->depth == 4) {
                struct dst_pixel_t {
                        uint8_t r;
                        uint8_t g;
                        uint8_t b;
                        uint8_t a;
                } *dst = (struct dst_pixel_t *) dst_ptr;

                for (int row = 0; row < src_h; row++, dst += tgt_w) {
                        for (int col = 0; col < src_w; col++, src_ptr++) {
                                dst[col] = (struct dst_pixel_t) { 255, 255, 255, *src_ptr };
                        }
                }
        } else {
                for (i = 0; i < src_h; i++) {
                        memcpy(dst_ptr, src_ptr, ft_bitmap.width);
                        dst_ptr += tgt_w * font->atlas->depth;
                        src_ptr += ft_bitmap.pitch;
                }
        }

        if (font->rendermode == FTGL_RENDER_SIGNED_DISTANCE_FIELD) {
                unsigned char *sdf = make_distance_mapb(buffer, tgt_w, tgt_h);
                free(buffer);
                buffer = sdf;
        }

        ftgl_texture_atlas_set_region(font->atlas, x, y, tgt_w, tgt_h, buffer, tgt_w * font->atlas->depth);
        free(buffer);

        glyph = ftgl_texture_glyph_create();
        glyph->codepoint = glyph_index ? ucodepoint : 0;
        glyph->width = tgt_w;
        glyph->height = tgt_h;
        glyph->rendermode = font->rendermode;
        glyph->outline_thickness = font->outline_thickness;
        glyph->offset_x = ft_glyph_left;
        glyph->offset_y = ft_glyph_top;

        if (font->scaletex) {
                glyph->s0 = x / (float) font->atlas->width;
                glyph->t0 = y / (float) font->atlas->height;
                glyph->s1 = (x + glyph->width) / (float) font->atlas->width;
                glyph->t1 = (y + glyph->height) / (float) font->atlas->height;
        } else {
                glyph->s0 = x - 0.5;
                glyph->t0 = y - 0.5;
                glyph->s1 = x + tgt_w - 0.5;
                glyph->t1 = y + tgt_h - 0.5;
        }

        slot = font->face->glyph;
        if (FT_HAS_FIXED_SIZES(font->face)) {
                glyph->advance_x = slot->advance.x;
                glyph->advance_y = slot->advance.y;
        } else {
                glyph->advance_x = ftgl_convert_F26Dot6_to_float(slot->advance.x) * font->scale;
                glyph->advance_y = ftgl_convert_F26Dot6_to_float(slot->advance.y) * font->scale;
        }

        int free_glyph = ftgl_texture_font_index_glyph(font, glyph, ucodepoint);
        if (!glyph_index) {
                if (!free_glyph) {
                        glyph = ftgl_texture_glyph_copy(glyph);
                }
                free_glyph = ftgl_texture_font_index_glyph(font, glyph, 0);
        }

        if (free_glyph) {
                free(glyph);
        }

        if (font->rendermode != FTGL_RENDER_NORMAL
            && font->rendermode != FTGL_RENDER_SIGNED_DISTANCE_FIELD) {
                FT_Done_Glyph(ft_glyph);
        }

        ftgl_texture_font_generate_kerning(font, &font->library->library, &font->face);
        ftgl_texture_font_close(font, FTGL_MODE_AUTO_CLOSE, FTGL_MODE_AUTO_CLOSE);
        return 1;
}

size_t
ftgl_texture_font_load_glyphs(ftgl_texture_font_t *font,
                              const char *codepoints)
{
        size_t i;

        font->mode++;
        for (i = 0; i < strlen(codepoints); i += utf8_surrogate_len(codepoints + i)) {
                if (!ftgl_texture_font_load_glyphs(font, codepoints + i)) {
                        font->mode--;
                        ftgl_texture_font_close(font, FTGL_MODE_AUTO_CLOSE, FTGL_MODE_AUTO_CLOSE);
                        return utf8_strlen(codepoints + i);
                }
        }

        font->mode--;
        ftgl_texture_font_close(font, FTGL_MODE_AUTO_CLOSE, FTGL_MODE_AUTO_CLOSE);
        return 0;
}

void
ftgl_texture_font_enlarge_atlas(ftgl_texture_font_t *font,
                                size_t width_new,
                                size_t height_new)
{
#ifdef FTGL_DEBUG
        assert(font);
        assert(font->atlas);
        assert(width_new >= font->atlas->width);
        assert(height_new >= font->atlas->height);
        assert(width_new + height_new > font->atlas->width + font->atlas->height);
#endif /* FTGL_DEBUG */

        ftgl_texture_atlas_t *ta = font->atlas;
        size_t width_old = ta->width;
        size_t height_old = ta->height;

        ftgl_texture_font_enlarge_texture(font, width_new, height_new);
        if (font->scaletex) {
                float mulw = (float) width_old / width_new;
                float mulh = (float) height_old / height_new;
                ftgl_texture_font_enlarge_glyphs(font, mulw, mulh);
        }
}

void
ftgl_texture_font_enlarge_glyphs(ftgl_texture_font_t *font,
                                 float mulw,
                                 float mulh)
{
        size_t i;
        ftgl_texture_glyph_t *g;
        
}

void
ftgl_texture_font_enlarge_texture(ftgl_texture_font_t *font,
                                  size_t width_new,
                                  size_t height_new)
{
#ifdef FTGL_DEBUG
        assert(font);
#endif /* FTGL_DEBUG */
        ftgl_texture_atlas_enlarge_texture(font->atlas, width_new, height_new);
}

float
ftgl_texture_glyph_get_kerning(const ftgl_texture_glyph_t *glyph,
                               const char *codepoint)
{
        uint32_t ucodepoint = utf8_to_utf32(codepoint);
        uint32_t i = ucodepoint >> 8;
        uint32_t j = ucodepoint & 0xFF;
        float *kern_index;

#ifdef FTGL_DEBUG
        assert(glyph);
#endif /* FTGL_DEBUG */

        if (ucodepoint == -1)
                return 0;

        if (glyph->kerning->size <= i)
                return 0;

        kern_index = *(float **) ftgl_vector_get(glyph->kerning, i);

        if (!kern_index)
                return 0;
        return kern_index[j];
}

ftgl_texture_glyph_t *
ftgl_texture_glyph_create(void)
{
        ftgl_texture_glyph_t *glyph = malloc(sizeof(*glyph));
        if (!glyph) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate glyph!");
#endif /* FTGL_DEBUG */
                return NULL;
        }

        glyph->codepoint = -1;
        glyph->width = 0;
        glyph->height = 0;

        /* attribute start */
        glyph->rendermode = FTGL_RENDER_NORMAL;
        glyph->outline_thickness = 0.0;
        glyph->glyphmode = FTGL_GLYPH_END;
        /* attribute end */

        glyph->offset_x = 0;
        glyph->offset_y = 0;
        glyph->advance_x = 0.0;
        glyph->advance_y = 0.0;
        glyph->s0 = 0.0;
        glyph->t0 = 0.0;
        glyph->s1 = 0.0;
        glyph->t1 = 0.0;
        glyph->kerning = ftgl_vector_create(sizeof(float **));
        if (!glyph->kerning) {
                free(glyph);
                return NULL;
        }
        return glyph;
}

void
ftgl_texture_glyph_free(ftgl_texture_glyph_t *glyph)
{
        int i;
#ifdef FTGL_DEBUG
        assert(glyph);
#endif /* FTGL_DEBUG */
        for (i = 0; i < glyph->kerning->size; i++)
                free(*(float **) ftgl_vector_get(glyph->kerning, i));
        ftgl_vector_free(glyph->kerning);
        free(glyph);
}

ftgl_texture_glyph_t *
ftgl_texture_glyph_copy(ftgl_texture_glyph_t *glyph)
{
        int i;
        ftgl_texture_glyph_t *new_glyph;
        float *source;
        float **target;
#ifdef FTGL_DEBUG
        assert(glyph);
#endif /* FTGL_DEBUG */

        new_glyph = ftgl_texture_glyph_create();
        if (!new_glyph) {
                return NULL;
        }
        
        for (i = 0; i < glyph->kerning->size; i++) {
                source = *(float **) ftgl_vector_get(glyph->kerning, i);
                *target = calloc(0x100, sizeof(float));
                if (!(*target)) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE("Failed to allocate kerning for glyph!");
#endif /* FTGL_DEBUG */
                        ftgl_texture_glyph_free(new_glyph);
                        return NULL;
                }
                memcpy(*target, source, 0x100);
                if (ftgl_vector_push_back(glyph->kerning, target)
                    != FTGL_NO_ERROR) {
                        ftgl_texture_glyph_free(new_glyph);
                        return NULL;
                }
        }

        return new_glyph;
}
