#ifndef FTGL_TEXTURE_FONT_H_
#define FTGL_TEXTURE_FONT_H_

#include <stdlib.h>
#include <stdint.h>

#include "vector.h"
#include "texture-atlas.h"

#ifndef __THREAD
#if defined(__GNUC__) || defined(__clang__)
#define __THREAD __thread
#elif defined(_MSC_VER)
#define __THREAD __declspec( thread )
#else
#define __THREAD
#endif
#endif

typedef enum ftgl_rendermode_t {
        FTGL_RENDER_NORMAL,
        FTGL_RENDER_OUTLINE_EDGE,
        FTGL_RENDER_OUTLINE_POSITIVE,
        FTGL_RENDER_OUTLINE_NEGATIVE,
        FTGL_RENDER_SIGNED_DISTANCE_FIELD
} ftgl_rendermode_t;

typedef enum ftgl_glyphmode_t {
        FTGL_GLYPH_END = 0,
        FTGL_GLYPH_CONT = 1,
} ftgl_glyphmode_t;

typedef struct ftgl_texture_glyph_t {
        /**
         * Unicode codepoint this glyph represents in UTF-32 LE encoding.
         */
        uint32_t codepoint;

        /**
         * Glyph's width in pixels.
         */
        size_t width;

        /**
         * Glyph's height in pixels.
         */
        size_t height;

        /**
         * Glyph's left bearing expressed in integer pixels.
         */
        int offset_x;

        /**
         * Glyph's top bearing expressed in integer pixels.
         *
         * Remember that this is the distance from the baseline to the
         * top-most glyph scanline, upwards y coordinates being positive.
         */
        int offset_y;

        /**
         * For horizontal text layouts, this is the horizontal distance (in
         * fractional pixels) used to increment the pen position when the glyph
         * is drawn as part of a string of text
         */
        float advance_x;

        /**
         * For vertical text layouts, this is the vertical distance (in fractional
         * pixels) used to increment the pen position when the glyph is drawn as
         * part of a string of text.
         */
        float advance_y;

        /**
         * First normalized texture coordinate (x) of top-left corner
         */
        float s0;

        /**
         * Second normalized texture coordinate (y) of top-left corner
         */
        float t0;

        /**
         * First normalized texture coordinate (x) of bottom-right corner
         */
        float s1;

        /**
         * Second normalized texture coordinate (y) of bottom-right corner
         */
        float t1;

        /**
         * A vector of kerning pairs relative to this glyph.
         */
        ftgl_vector_t * kerning;

        /**
         * Mode this glyph was rendered
         */
        ftgl_rendermode_t rendermode;

        /**
         * Glyph outline thickness
         */
        float outline_thickness;

        /**
         * Glyph scan end mark
         */
        ftgl_glyphmode_t glyphmode;
} ftgl_texture_glyph_t;

typedef enum ftgl_font_location_t {
        FTGL_TEXTURE_FONT_FILE = 0,
        FTGL_TEXTURE_FONT_MEMORY
} ftgl_font_location_t;

typedef enum ftgl_font_mode_t {
        FTGL_MODE_AUTO_CLOSE = 0,
        FTGL_MODE_GLYPHS_CLOSE,
        FTGL_MODE_FREE_CLOSE,
        FTGL_MODE_MANUAL_CLOSE,
        FTGL_MODE_ALWAYS_OPEN
} ftgl_font_mode_t;

extern __THREAD ftgl_font_mode_t ftgl_mode_default;

void
ftgl_texture_font_default_mode(ftgl_font_mode_t mode);

/* If there is no Freetype included, just define that as incomplete pointer */
#if !defined(FT2BUILD_H_) && !defined(__FT2BUILD_H__) && !defined(FREETYPE_H_)
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_SizeRec_* FT_Size;
#endif

/* same for harfbuzz */
#ifndef HB_BUFFER_H
typedef struct hb_font_t hb_font_t;
#endif

typedef struct ftgl_texture_font_library_t {
        /**
         * Flag for mode
         */
        ftgl_font_mode_t mode;

        /**
         * Freetype library pointer
         */
        FT_Library library;
} ftgl_texture_font_library_t;

typedef struct ftgl_texture_font_t {
        /**
         * Vector of glyphs contained in this font. This is
         * actually a two-stage table, indexing into 256 glyphs each.
         */
        ftgl_vector_t *glyphs;

        /**
         * Atlas structure to store glyphs data.
         */
        ftgl_texture_atlas_t *atlas;

        /**
         * font location
         */
        ftgl_font_location_t location;

        union {
                /**
                 * Font filename, for when
                 * location == FTGL_TEXTURE_FONT_FILE
                 */
                char *filename;

                /**
                 * Font memory address, for when
                 * location == FTGL_TEXTURE_FONT_MEMORY
                 */
                struct {
                        const void *base;
                        size_t size;
                } memory;
        };

        /**
         * Texture font library
         */
        ftgl_texture_font_library_t *library;

        /**
         * Font size
         */
        float size;

        /**
         * Mode the font is rendering its next glyph.
         */
        ftgl_rendermode_t rendermode;

        /**
         * Outline thickness
         */
        float outline_thickness;

        /**
         * Whether to use our own lcd filter.
         */
        unsigned char filtering;

        /**
         * Whether to use kerning if available
         */
        unsigned char kerning;

        /**
         * Whether to use autohint when rendering font
         */
        unsigned char hinting;

        /**
         * Whether to scale texture coordinates
         */
        unsigned char scaletex;

        /**
         * LCD filter weights
         */
        unsigned char lcd_weights[5];

        /**
         * This field is simply used to compute a default line spacing (i.e., the
         * baseline-to-baseline distance) when writing text with this font. Note
         * that is usually is larger than the sum of the ascender and descender
         * taken as absolute values. There is also no guarantee that no glyphs
         * extend above or below subsequence baselines when using this distance.
         */
        float height;

        /**
         * This field is the distance that must be placed between two lines of
         * text. The baseline-to-baseline distance should be computed as:
         * ascender - descender + linegap
         */
        float linegap;

        /**
         * The ascender is the vertical distance from the horizontal baseline to
         * the highest 'character' coordinate in a font face. Unfortunately, font
         * formats define the ascender differently. For some, it represents the
         * ascent of all capital latin characters (without accents), for
         * others it is the ascent of the highest accented character, and
         * finally, other formats define it as being equal to bbox.yMax.
         */
        float ascender;

        /**
         * The descender is the vertical distance from the horizontal baseline to
         * the lowest 'character' coordinate in a font face. Unfortunately, font
         * formats define the descender differently. For some, it represents the
         * descent of all capital latin characters (without accents),
         * for others it is the ascent of the lowest accented character, and
         * finally, other formats define it as being equal to bbox.yMin.
         * This field is negative for values below the baseline.
         */
        float descender;

        /**
         * The position of the underline line for this face. It is the center of
         * the underlining stem. Only relevant for scalable formats.
         */
        float underline_position;

        /**
         * The thickness of the underline for this face.
         * Only relevant for scalable  formats.
         */
        float underline_thickness;

        /**
         * The padding to be add to the glyph's texture that are loaded
         * by this font. Usefull when adding effects with shaders.
         */
        int padding;

        /**
         * Flag for mode
         */
        ftgl_font_mode_t mode;

        /**
         * Freetype face pointer
         */
        FT_Face face;

        /**
         * Freetype size pointer
         */
        FT_Size ft_size;

        /**
         * Harfbuzz font pointer
         */
        hb_font_t *hb_font;

        /**
         * factor to scale font coordinates.
         */
        float scale;
} ftgl_texture_font_t;

ftgl_texture_font_library_t *
ftgl_texture_font_library_create(void);

extern __THREAD ftgl_texture_font_library_t * freetype_gl_library;

ftgl_texture_font_t *
ftgl_texture_font_new_from_file(ftgl_texture_atlas_t *atlas,
                                const float pt_size,
                                const char *filename);

ftgl_texture_font_t *
ftgl_texture_font_new_from_memory(ftgl_texture_atlas_t *atlas,
                                  float pt_size,
                                  const void *memory_base,
                                  size_t memory_size);

ftgl_texture_font_t *
ftgl_texture_font_copy(ftgl_texture_font_t *font,
                       float size);

void
ftgl_texture_font_close(ftgl_texture_font_t *font,
                        ftgl_font_mode_t face_mode,
                        ftgl_font_mode_t library_mode);

void
ftgl_texture_font_free(ftgl_texture_font_t *font);

ftgl_return_t
ftgl_texture_font_load_face(ftgl_texture_font_t *font,
                            float size);

ftgl_texture_glyph_t *
ftgl_texture_font_get_glyph(ftgl_texture_font_t *font,
                            const char *codepoint);

ftgl_return_t
ftgl_texture_font_index_glyph(ftgl_texture_font_t *font,
                              ftgl_texture_glyph_t *glyph,
                              uint32_t codepoint);

ftgl_return_t
ftgl_texture_font_load_glyph(ftgl_texture_font_t *font,
                             const char *codepoint);

ftgl_texture_glyph_t *
ftgl_texture_font_get_glyph_gi(ftgl_texture_font_t *font,
                               uint32_t glyph_index);

ftgl_texture_glyph_t *
ftgl_texture_font_find_glyph_gi(ftgl_texture_font_t *font,
                                uint32_t glyph_index);

int
ftgl_texture_font_load_glyph_gi(ftgl_texture_font_t *font,
                                uint32_t glyph_index,
                                uint32_t ucodepoint);

size_t
ftgl_texture_font_load_glyphs(ftgl_texture_font_t *font,
                              const char *codepoints);

void
ftgl_texture_font_enlarge_atlas(ftgl_texture_font_t *font,
                                size_t width_new,
                                size_t height_new);

void
ftgl_texture_font_enlarge_glyphs(ftgl_texture_font_t *font,
                                 float mulw,
                                 float mulh);

void
ftgl_texture_font_enlarge_texture(ftgl_texture_font_t *font,
                                  size_t width_new,
                                  size_t height_new);

float
ftgl_texture_glyph_get_kerning(const ftgl_texture_glyph_t *glyph,
                               const char *codepoint);

ftgl_texture_glyph_t *
ftgl_texture_glyph_create(void);

void
ftgl_texture_glyph_free(ftgl_texture_glyph_t *glyph);

ftgl_texture_glyph_t *
ftgl_texture_glyph_copy(ftgl_texture_glyph_t *glyph);
#endif /* FTGL_TEXTURE_FONT_H_ */
