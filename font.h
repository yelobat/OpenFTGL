/**
 * @author: yelobat
 * 
 * @description: Use FreeType with OpenGL to render text using a single
 * texture buffer. This project was inspired by rougier's freetype-gl
 * repository found at: https://github.com/rougier/freetype-gl. This
 * project adapts the implementation seen in this repository with the
 * idea to enclose it inside of a single header file, similar to stb.
 *
 * @instructions: Add #FTGL_IMPLEMENTATION to the start of the
 * implementation file in order to add the implementation code
 * to the project.
 */

#ifndef FTGL_FONT_H_
#define FTGL_FONT_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H
#include FT_TRUETYPE_TABLES_H

#include <GL/glew.h>
#include <float.h>
#include <stdint.h>

#include "linear.h"

extern FT_Library ftgl_font_library;

#define FTGL_FONT_HRES  64
#define FTGL_FONT_HRESf 64.0f
#define FTGL_FONT_DPI   72

#ifndef FTGLDEF
#ifdef FTGLSTATIC
#define FTGLDEF static
#else /* !defined(FTGLSTATIC) */
#define FTGLDEF extern
#endif /* FTGLSTATIC */
#endif /* FTGLDEF */

#if !defined(FTGL_MALLOC) || !defined(FTGL_REALLOC)			\
	|| !defined(FTGL_CALLOC) || !defined(FTGL_FREE) || !defined(FTGL_STRDUP)
#define FTGL_MALLOC(sz) malloc(sz)
#define FTGL_REALLOC(x, newsz) realloc(x, newsz)
#define FTGL_CALLOC(nmemb, size) calloc(nmemb, size)
#define FTGL_STRDUP(s) strdup(s)
#define FTGL_FREE(x) free(x)
#endif

typedef enum ftgl_return_t {
	FTGL_NO_ERROR = 0,
	FTGL_MEMORY_ERROR,
	FTGL_ARGUMENT_ERROR,
	FTGL_FREETYPE_ERROR,
} ftgl_return_t;

typedef struct ftgl_glyph_t {
	/**
	 * The bounding box of the glyph in the texture
	 */
	union {
		ivec4_t bbox;
		struct {
			GLuint x;
			GLuint y;
			GLuint w;
			GLuint h;
		};
	};

	/**
	 * The codepoint that the glyph represents
	 */
	uint32_t codepoint;

	/**
	 * Glyph's left bearing expressed in integer pixels.
	 */
	GLint offset_x;

	/**
	 * GLyph's top bearing expressed in integer pixels.
	 */
	GLint offset_y;

	/**
	 * For horizontal text layouts, this is the horizontal
	 * distance (in fractional pixels) used to increment the
	 * pen position when the glyph is drawn as part of a string
	 * of text.
	 */
	GLfloat advance_x;

	/**
	 * For vertical text layouts, this is the verical distance
	 * (in fractional pixels) used to increment the pen position
	 * when the glyph is drawn as part of a string of text.
	 */
	GLfloat advance_y;
} ftgl_glyph_t;

typedef struct ftgl_glyphlist_t {
	ftgl_glyph_t *glyph;
	struct ftgl_glyphlist_t *next;
} ftgl_glyphlist_t;

#define FTGL_FONT_GLYPHMAP_CAPACITY 23

typedef struct ftgl_glyphmap_t {
	ftgl_glyphlist_t *map[FTGL_FONT_GLYPHMAP_CAPACITY];
} ftgl_glyphmap_t;

#define FTGL_FONT_ATLAS_WIDTH  1024
#define FTGL_FONT_ATLAS_HEIGHT 1024

typedef enum ftgl_rendermode_t {
	FTGL_RENDERMODE_NORMAL,
	FTGL_RENDERMODE_SDF,
} ftgl_rendermode_t;

typedef struct ftgl_font_t {
	/**
	 * Stores the texture for which
	 * the glyphs are stored inside of.
	 */
	GLuint texture;

	/**
	 * A face structure used to load glyphs,
	 * and faces.
	 */
	FT_Face face;

	/**
	 * A vector containing the (x, y) coordinates
	 * for the next location to store the next
	 * glyph in the current texture
	 */
	ivec2_t tbox;

	/**
	 * Internal Usage: The maximum height of a glyph in the
	 * currently row. When the current row is full, this value
	 * is used to move @tbox to the next row.
	 */
	GLuint tbox_yjump;

	/**
	 * The factor to scale fonts by.
	 */
	float scale;

	/**
	 * This is simply used to compute a default line spacing (i.e., the
	 * baseline-to-baseline distance) when writing text with this font. Note
	 * that it is usually larger than the sum of the ascender and descender
	 * taken as absolute values. There is also no guarantee that no glyphs
	 * extend above or below subsequence baselines when using this distance.
	 */
	float height;

	/**
	 * This field is the distance that must be placed between two lines of
	 * text. The baseline-to-baseline distance should be computed as:
	 * ascender - descender + linegap.
	 */
	float linegap;

	/**
	 * The ascender is the vertical distance from the horizontal baseline to
	 * the height 'character' coordinate in a font face. Unforunately, font
	 * formats define the ascender differently. For some, it represents the
	 * ascent of all capital latin characters (without accents), for
	 * others it is the ascent of the highest accented character, and
	 * finally, other formats define it as being equal to bbox.yMax.
	 */ 
	float ascender;

	/**
	 * The descender is the vertical distance from the horizontal baseline to
	 * the lowest 'character' coordinate in a font face. Unforunately, font
	 * formats define the descender differently. For some, it represents the
	 * descent of all capital latin characters (without accents), for others
	 * it is the ascent of the lowest accented character, and finally, other
	 * formats define it as being equal to bbox.yMin. This field is negative
	 * for values below the baseline.
	 */
	float descender;
	
	/**
	 * A hashmap containing glyph information.
	 */
	ftgl_glyphmap_t *glyphmap;

	/**
	 * FTGL_RENDERMODE_NORMAL - Normal Bitmap rendering
	 * FTGL_RENDERMODE_SDF    - Signed Distance Field (SDF) rendering
	 */
	ftgl_rendermode_t rendermode;
} ftgl_font_t;

#define FTGL_STRING_CAPACITY (4)

typedef struct ftgl_string_t {
	GLfloat width;
	GLfloat height;
	char updated;
	
	size_t size;
	size_t capacity;
	char *data;
} ftgl_string_t;

FTGLDEF ftgl_return_t   ftgl_font_library_init(void);
FTGLDEF ftgl_return_t   ftgl_font_manager_insert(const char *name, const char *path, size_t ptsize);
FTGLDEF ftgl_font_t *   ftgl_font_manager_find(const char *name);
FTGLDEF ftgl_font_t *   ftgl_font_create(void);
FTGLDEF ftgl_return_t   ftgl_font_bind(ftgl_font_t *font, const char *path);
FTGLDEF ftgl_return_t   ftgl_font_set_size(ftgl_font_t *font, float size);
FTGLDEF void            ftgl_computegradient(double *img, int w, int h, double *gx, double *gy);
FTGLDEF double          ftgl_edgedf(double gx, double gy, double a);
FTGLDEF double          ftgl_distaa3(double *img, double *gximg, double *gyimg, int w, int c, int xc, int yc, int xi, int yi);
FTGLDEF void            ftgl_edtaa3(double *img, double *gx, double *gy, int w, int h, short *distx, short *disty, double *dist);
FTGLDEF double *        ftgl_distance_mapd(double *data, unsigned int width, unsigned int height);
FTGLDEF unsigned char * ftgl_distance_mapb(unsigned char *img, unsigned int width, unsigned int height);
FTGLDEF ftgl_glyph_t *  ftgl_font_load_codepoint(ftgl_font_t *font, uint32_t codepoint);
FTGLDEF ftgl_glyph_t *  ftgl_font_find_glyph(ftgl_font_t *font, uint32_t codepoint);
FTGLDEF vec2_t          ftgl_font_string_dimensions(const char *source, ftgl_font_t *font);
FTGLDEF ftgl_string_t * ftgl_string_create(size_t reserve);
FTGLDEF ftgl_return_t   ftgl_string_write_at(ftgl_string_t *s, ftgl_font_t *font, char *buffer, size_t buffer_len, size_t pos);
FTGLDEF ftgl_return_t   ftgl_string_write(ftgl_string_t *s, ftgl_font_t *font, char *buffer, size_t buffer_len);
FTGLDEF ftgl_return_t   ftgl_string_append(ftgl_string_t *s, ftgl_font_t *font, char *buffer, size_t buffer_len);
FTGLDEF vec2_t          ftgl_string_dimensions(ftgl_string_t *s, ftgl_font_t *font);
FTGLDEF void            ftgl_string_free(ftgl_string_t *s);
FTGLDEF void            ftgl_font_free(ftgl_font_t *font);
FTGLDEF void            ftgl_font_library_free(void);

#ifdef FTGL_IMPLEMENTATION

typedef struct ftgl_font_node_t {
	char *name;
	ftgl_font_t *font;
} ftgl_font_node_t;

struct {
	size_t size;
	size_t capacity;
	ftgl_font_node_t **nodes;
} ftgl_font_manager;

#define FTGL_FONT_MANAGER_CAPACITY (2)
#define FTGL_FONT_MANAGER_RRATIO (0.8)
#define FTGL_FONT_MANAGER_RESIZEP() \
	((ftgl_font_manager.size / (float) ftgl_font_manager.capacity) >= FTGL_FONT_MANAGER_RRATIO)

FT_Library ftgl_font_library;

static float ftgl_F26Dot6_to_float(FT_F26Dot6 value)
{
	return ((float) value) / 64.0;
}

static FT_F26Dot6 ftgl_float_to_F26Dot6(float value)
{
	return (FT_F26Dot6) (value * 64.0);
}

static ftgl_glyph_t *ftgl_glyph_create4iv(uint32_t codepoint, ivec4_t bbox, GLint offset_x,
		     GLint offset_y, GLfloat advance_x, GLfloat advance_y)
{
	ftgl_glyph_t *glyph;
	glyph = FTGL_MALLOC(sizeof(*glyph));
	if (!glyph) {
		return NULL;
	}

	glyph->bbox = bbox;
	glyph->codepoint = codepoint;
	glyph->offset_x = offset_x;
	glyph->offset_y = offset_y;
	glyph->advance_x = advance_x;
	glyph->advance_y = advance_y;
	return glyph;
}

static void ftgl_glyph_free(ftgl_glyph_t *glyph)
{
	glyph->bbox = ll_ivec4_create4i(0,0,0,0);
	glyph->codepoint = 0;
	glyph->offset_x = 0;
	glyph->offset_y = 0;
	glyph->advance_x = 0;
	glyph->advance_y = 0;
	FTGL_FREE(glyph);
}

static ftgl_glyphlist_t *ftgl_glyphlist_create4iv(uint32_t codepoint, ivec4_t bbox, GLint offset_x,
			 GLint offset_y, GLfloat advance_x, GLfloat advance_y)
{
	ftgl_glyphlist_t *glyphlist;
	ftgl_glyph_t *glyph;

	glyph = ftgl_glyph_create4iv(codepoint, bbox, offset_x,
				     offset_y, advance_x, advance_y);
	if (!glyph) {
		return NULL;
	}

	glyphlist = FTGL_MALLOC(sizeof(*glyphlist));
	if (!glyphlist) {
		FTGL_FREE(glyph);
		return NULL;
	}

	glyphlist->glyph = glyph;
	glyphlist->next = NULL;
	return glyphlist;
}

static void ftgl_glyphlist_free(ftgl_glyphlist_t *glyphlist)
{
	ftgl_glyph_free(glyphlist->glyph);
	glyphlist->glyph = NULL;
	glyphlist->next = NULL;
	FTGL_FREE(glyphlist);
}

static ftgl_glyphmap_t *ftgl_glyphmap_create(void)
{
	ftgl_glyphmap_t *glyphmap;
	glyphmap = FTGL_MALLOC(sizeof(*glyphmap));
	if (!glyphmap) {
		return NULL;
	}

	memset(glyphmap->map, 0, sizeof(*glyphmap->map)
	       * FTGL_FONT_GLYPHMAP_CAPACITY);
	return glyphmap;
}

static ftgl_glyph_t *ftgl_glyphmap_find_glyph(ftgl_glyphmap_t *glyphmap,
			 uint32_t codepoint)
{
	ftgl_glyph_t *glyph;
	ftgl_glyphlist_t *glyphlist;
	size_t hash;

	hash = codepoint % FTGL_FONT_GLYPHMAP_CAPACITY;
	glyphlist = glyphmap->map[hash];
	while (glyphlist != NULL) {
		glyph = glyphlist->glyph;
		if (glyph->codepoint == codepoint) {
			return glyph;
		}
		glyphlist = glyphlist->next;
	}
	return NULL;
}

static ftgl_return_t ftgl_glyphmap_insert(ftgl_glyphmap_t *glyphmap,
		     uint32_t codepoint, ivec4_t bbox, GLint offset_x,
		     GLint offset_y, GLfloat advance_x, GLfloat advance_y)
{
	size_t hash;
	ftgl_glyphlist_t *glyphlist;
	if (ftgl_glyphmap_find_glyph(glyphmap, codepoint)) {
		return FTGL_NO_ERROR;
	}

	glyphlist = ftgl_glyphlist_create4iv(codepoint, bbox, offset_x,
					     offset_y, advance_x, advance_y);
	if (!glyphlist) {
		return FTGL_MEMORY_ERROR;
	}

	hash = codepoint % FTGL_FONT_GLYPHMAP_CAPACITY;
	if (glyphmap->map[hash]) {
		glyphlist->next = glyphmap->map[hash];
		glyphmap->map[hash] = glyphlist;
	} else {
		glyphmap->map[hash] = glyphlist;
	}
	return FTGL_NO_ERROR;
}

static void ftgl_glyphmap_free(ftgl_glyphmap_t *glyphmap)
{
	size_t i;
	ftgl_glyphlist_t *glyphlist;

	for (i = 0; i < FTGL_FONT_GLYPHMAP_CAPACITY; i++) {
		glyphlist = glyphmap->map[i];
		while (glyphlist != NULL) {
			ftgl_glyphlist_t *next;
			next = glyphlist->next;
			ftgl_glyphlist_free(glyphlist);
			glyphlist = next;
		}
	}

	memset(glyphmap->map, 0, sizeof(*glyphmap->map)
	       * FTGL_FONT_GLYPHMAP_CAPACITY);
	FTGL_FREE(glyphmap);
}

// meiyan hash function
// Source: http://www.sanmayce.com/Fastest_Hash/
static inline uint32_t ftgl_string_hash(const char *s, size_t len)
{
	uint32_t hash = 0x811c9dc5;
	while (len >= 8) {
		hash = (hash ^ ((((*(uint32_t *) s) << 5) | ((*(uint32_t *) s) >> 27))
				^ *(uint32_t *)(s + 4))) * 0xad3e7;
		len -= 8;
		s += 8;
	}

	if (len & 4) {
		hash = (hash ^ *(uint16_t *) s) * 0xad3e7;
		s += 2;
		hash = (hash ^ *(uint16_t *) s) * 0xad3e7;
		s += 2;
	}

	if (len & 2) {
		hash = (hash ^ *(uint16_t *) s) * 0xad3e7;
		s += 2;
	}

	if (len & 1) {
		hash = (hash ^ *s) * 0xad3e7;
	}

	return hash ^ (hash >> 16);
}

static ftgl_return_t ftgl_font_manager_init(void)
{
	ftgl_font_manager.size = 0;
	ftgl_font_manager.capacity = FTGL_FONT_MANAGER_CAPACITY;
	ftgl_font_manager.nodes = FTGL_CALLOC(ftgl_font_manager.capacity,
					      sizeof(*ftgl_font_manager.nodes));
	if (!ftgl_font_manager.nodes) {
		return FTGL_MEMORY_ERROR;
	}

	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_return_t ftgl_font_library_init(void)
{
	FT_Error ft_error;
	ftgl_return_t ret;
	if ((ft_error = FT_Init_FreeType(&ftgl_font_library)) != FT_Err_Ok) {
		return FTGL_FREETYPE_ERROR;
	}

	if ((ret = ftgl_font_manager_init()) != FTGL_NO_ERROR) {
		return ret;
	}
	return FTGL_NO_ERROR;
}

static ftgl_font_node_t *ftgl_font_node_create(const char *name, const char *path,
		      size_t ptsize)
{
	ftgl_return_t ret;
	ftgl_font_node_t *font_node;
	font_node = FTGL_MALLOC(sizeof(*font_node));
	if (!font_node) {
		return NULL;
	}

	font_node->name = FTGL_STRDUP(name);
	if (!font_node->name) {
		FTGL_FREE(font_node);
		return NULL;
	}

	font_node->font = ftgl_font_create();
	if (!font_node->font) {
		FTGL_FREE(font_node->name);
		FTGL_FREE(font_node);
		return NULL;
	}

	ret = ftgl_font_bind(font_node->font, path);
	if (ret != FTGL_NO_ERROR) {
		FTGL_FREE(font_node->name);
		ftgl_font_free(font_node->font);
		FTGL_FREE(font_node);
		return NULL;
	}

	ret = ftgl_font_set_size(font_node->font, ptsize);
	if (ret != FTGL_NO_ERROR) {
		FTGL_FREE(font_node->name);
		ftgl_font_free(font_node->font);
		FTGL_FREE(font_node);
		return NULL;
	}

	return font_node;
}

static ftgl_return_t ftgl_font_manager_resize(void)
{
	size_t new_capacity, i, j;
	ftgl_font_node_t **new_nodes;
	uint32_t idx0, idx1, real_idx;
	new_capacity = ftgl_font_manager.capacity << 1;
	new_nodes = FTGL_CALLOC(new_capacity, sizeof(*new_nodes));
	if (!new_nodes) {
		return FTGL_MEMORY_ERROR;
	}

	for (i = 0; i < ftgl_font_manager.capacity; i++) {
		ftgl_font_node_t *font_node = ftgl_font_manager.nodes[i];
		if (!font_node) continue;
		idx0 = ftgl_string_hash(font_node->name, strlen(font_node->name)) \
			& (new_capacity - 1);
		idx1 = idx0 | 1;

		for (j = 0; j <= new_capacity; j++) {
			real_idx = (idx0 + idx1 * j) & (new_capacity - 1);
			if (new_nodes[real_idx]) continue;
			new_nodes[real_idx] = font_node;
			break;
		}
	}

	FTGL_FREE(ftgl_font_manager.nodes);
	ftgl_font_manager.nodes = new_nodes;
	ftgl_font_manager.capacity = new_capacity;
	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_return_t ftgl_font_manager_insert(const char *name, const char *path,
			 size_t ptsize)
{
	ftgl_return_t ret;
	size_t idx0, idx1, real_idx, i;
	ftgl_font_node_t *font_node, *new_node;
	if (!name || strlen(name) <= 0 || !path) {
		return FTGL_ARGUMENT_ERROR;
	}
	
	if (FTGL_FONT_MANAGER_RESIZEP()) {
		if ((ret = ftgl_font_manager_resize()) != FTGL_NO_ERROR) {
			return ret;
		}
	}

	new_node = ftgl_font_node_create(name, path, ptsize);
	if (!new_node) {
		return FTGL_MEMORY_ERROR;
	}

	idx0 = ftgl_string_hash(name, strlen(name)) & (ftgl_font_manager.capacity - 1);
	idx1 = idx0 | 1;
	for (i = 0; i <= ftgl_font_manager.capacity; i++) {
		real_idx = (idx0 + idx1 * i) & (ftgl_font_manager.capacity - 1);
		font_node = ftgl_font_manager.nodes[real_idx];
		if (font_node) {
			if (strcmp(font_node->name, name) == 0)
				return FTGL_NO_ERROR;
			continue;
		}

		ftgl_font_manager.nodes[real_idx] = new_node;
		break;
	}

	ftgl_font_manager.size++;
	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_font_t * ftgl_font_manager_find(const char *name)
{
	ftgl_font_node_t *font_node;
	size_t idx0, idx1, real_idx, i;

	idx0 = ftgl_string_hash(name, strlen(name));
	idx0 = idx0 & (ftgl_font_manager.capacity - 1);
	idx1 = idx0 | 1;
	for (i = 0; i < ftgl_font_manager.capacity; i++) {
		real_idx = (idx0 + idx1 * i) & (ftgl_font_manager.capacity - 1);
		font_node = ftgl_font_manager.nodes[real_idx];
		if (!font_node) return NULL;
		if (font_node && strcmp(name, font_node->name) == 0) {
			return font_node->font;
		}
	}
	
	return NULL;
}

FTGLDEF ftgl_font_t * ftgl_font_create(void)
{
	GLenum gl_error;
	ftgl_font_t *font;

	font = FTGL_MALLOC(sizeof(*font));
	if (!font) {
		return NULL;
	}

	font->rendermode = FTGL_RENDERMODE_NORMAL;

	font->tbox = ll_ivec2_create2i(5,5);
	font->tbox_yjump = 0;

	font->glyphmap = ftgl_glyphmap_create();
	if (!font->glyphmap) {
		FTGL_FREE(font);
		return NULL;
	}

	glGenTextures(1, &font->texture);
	if ((gl_error = glGetError()) != GL_NO_ERROR) {
		ftgl_glyphmap_free(font->glyphmap);
		FTGL_FREE(font);
		return NULL;
	}

	glBindTexture(GL_TEXTURE_2D, font->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FTGL_FONT_ATLAS_WIDTH,
		     FTGL_FONT_ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	if ((gl_error = glGetError()) != GL_NO_ERROR) {
		glBindTexture(GL_TEXTURE_2D, 0);
		ftgl_glyphmap_free(font->glyphmap);
		FTGL_FREE(font);
		return NULL;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	font->scale = 1.0;
	font->face = NULL;
	return font;
}

FTGLDEF ftgl_return_t ftgl_font_bind(ftgl_font_t *font, const char *path)
{
	FT_Error ft_error;

	if (font->face) {
		if ((ft_error = FT_Done_Face(font->face)) != FT_Err_Ok) {
			return FTGL_FREETYPE_ERROR;
		}

		font->face = NULL;
	}

	if ((ft_error = FT_New_Face(ftgl_font_library, path,
				    0, &font->face)) != FT_Err_Ok) {
		return FTGL_FREETYPE_ERROR;
	}

	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_return_t ftgl_font_set_size(ftgl_font_t *font, float size)
{
	FT_Error ft_error;
	FT_Matrix matrix = {
		(int)((1.0/FTGL_FONT_HRES)  * 0x10000L),
		(int)((0.0)                 * 0x10000L),
		(int)((0.0)                 * 0x10000L),
		(int)((1.0)                 * 0x10000L)
	};

	if (FT_HAS_FIXED_SIZES(font->face)) {
		return FTGL_FREETYPE_ERROR;
	} else {
		ft_error = FT_Set_Char_Size(font->face, ftgl_float_to_F26Dot6(size),
					    0, FTGL_FONT_DPI * FTGL_FONT_HRES,
					    FTGL_FONT_DPI);
		if (ft_error != FT_Err_Ok) {
			return FTGL_FREETYPE_ERROR;
		}
	}

	FT_Size_Metrics metrics = font->face->size->metrics;
	font->ascender = metrics.ascender >> 6;
	font->descender = metrics.descender >> 6;
	font->height = metrics.height >> 6;
	font->linegap = font->height - font->ascender + font->descender;

	FT_Activate_Size(font->face->size);
	FT_Set_Transform(font->face, &matrix, NULL);
	return FTGL_NO_ERROR;
}

FTGLDEF void ftgl_computegradient(double *img, int w, int h, double *gx, double *gy)
{
	int i, j, k;
	double glength;
#define SQRT2 1.4142136
	// Avoid edges where the kernels would spill over
	for (i = 1; i < h - 1; i++) {
		for (j = 1; j < w - 1; j++) {
			k = i*w + j;
			// Compute gradient for edge pixels only
			if ((img[k] > 0.0) && (img[k] < 1.0)) {
				gx[k] = -img[k-w-1] - SQRT2*img[k-1] - img[k+w-1] + img[k-w+1]
					+ SQRT2*img[k+1] + img[k+w+1];
				gy[k] = -img[k-w-1] - SQRT2*img[k-w] - img[k-w+1] + img[k+w-1]
					+ SQRT2*img[k+w] + img[k+w+1];
				glength = gx[k]*gx[k] + gy[k]*gy[k];
				// Avoid division by zero
				if (glength > 0.0) {
					glength = sqrt(glength);
					gx[k] = gx[k] / glength;
					gy[k] = gy[k] / glength;
				}
			}
		}
	}
#undef SQRT2
}

FTGLDEF double ftgl_edgedf(double gx, double gy, double a)
{
	double df, glength, temp, a1;

	// Either
	// A) gu or gv are zero, or
	// B) both
	if ((gx == 0) || (gy == 0)) {
		df = 0.5-a;
	} else {
		glength = sqrt(gx*gx + gy*gy);
		if (glength > 0) {
			gx = gx/glength;
			gy = gy/glength;
		}

		/* Everything is symmetric wrt sign and transposition
		 * so move to first octant (gx >= 0, gy >= 0, gx >= gy) to
		 * avoid handling all possible edge cases
		 */
		gx = fabs(gx);
		gy = fabs(gy);
		if (gx < gy) {
			temp = gx;
			gx = gy;
			gy = temp;
		}

		a1 = 0.5*gy/gx;
		// 0 <= a < a1
		if (a < a1) {
			df = 0.5*(gx + gy) - sqrt(2.0 * gx * gy * a);
		} else if (a < (1.0-a1)) {
			// a1 <= a <= 1 - a1
			df = (0.5-a) * gx;
		} else {
			// 1-a1 < a <= 1
			df = -0.5 * (gx + gy) + sqrt(2.0 * gx * gy * (1.0-a));
		}
	}

	return df;
}

FTGLDEF double ftgl_distaa3(double *img, double *gximg, double *gyimg, int w,
	     int c, int xc, int yc, int xi, int yi)
{
	double di, df, dx, dy, gx, gy, a;
	int closest;

	// Index to the edge pixel pointed to from c
	closest = c-xc-yc*w;

	// Grayscale value at the edge pixel
	a = img[closest];

	// X gradient component at the edge pixel
	gx = gximg[closest];

	// Y gradient component at the edge pixel
	gy = gyimg[closest];

	if (a > 1.0) a = 1.0;
	
	// Clip grayscale values outside the range [0, 1]
	if (a < 0.0) a = 0.0;

	// Not an object pixel, return "very far" ("don't know yet")
	if (a == 0.0) return 1000000.0;

	dx = (double) xi;
	dy = (double) yi;

	// Length of integer vector, like a traditional EDT
	di = sqrt(dx*dx + dy*dy);
	if (di == 0) {
		//Use local gradient only at edges
		// Estimate based on local gradient only
		df = ftgl_edgedf(gx, gy, a);
	} else {
		// Estimate gradient based on direction to edge (accurate for large di)
		df = ftgl_edgedf(dx, dy, a);
	}

	// Same metric as ftgl_edtaa3, except at edges (where di = 0)
	return di + df; 
}

// Shorthand macro: add ubiquitous parameters dist, gx, gy, img and w and call distaa3()
#define DISTAA(c,xc,yc,xi,yi) (ftgl_distaa3(img, gx, gy, w, c, xc, yc, xi, yi))

FTGLDEF void ftgl_edtaa3(double *img, double *gx, double *gy, int w, int h,
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
		// At first, all pixels point to
		// themselves as the closest known.
		distx[i] = 0;
		disty[i] = 0; 
		if(img[i] <= 0.0) {
			// Big value, means "not set yet"
			dist[i]= 1000000.0; 
		} else if (img[i]<1.0) {
			// Gradient-assisted estimate
			dist[i] = ftgl_edgedf(gx[i], gy[i], img[i]); 
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

			// If non-zero distance or not set yet
			if(olddist > 0)  {
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

			// If not already zero distance
			if(olddist > 0) { 
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

			// If not already zero distance 
			if(olddist > 0) {
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
					
			// If not already zero distance
			if(olddist > 0) { 
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
				if(newdist < olddist-epsilon) {
					distx[i]=newdistx;
					disty[i]=newdisty;
					dist[i]=newdist;
					changed = 1;
				}
			}
		}
	}
	while(changed); // Sweep until no more updates are made

	/* The transformation is completed. */
}

FTGLDEF double * ftgl_distance_mapd(double *data, unsigned int width,
		   unsigned int height)
{
	short *xdist = FTGL_MALLOC(width * height * sizeof(*xdist));
	short *ydist = FTGL_MALLOC(width * height * sizeof(*ydist));
	double *gx = FTGL_MALLOC(width * height * sizeof(*gx));
	double *gy = FTGL_MALLOC(width * height * sizeof(*gy));
	double *outside = FTGL_MALLOC(width * height * sizeof(*outside));
	double *inside = FTGL_MALLOC(width * height * sizeof(*inside));
	double vmin = DBL_MAX;
	unsigned int i;

	// Compute outside = edtaa3(bitmap); % Transform background (0's)
	ftgl_computegradient(data, width, height, gx, gy);
	ftgl_edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
	for (i = 0; i < width * height; i++) {
		if (outside[i] < 0.0) outside[i] = 0.0;
	}

	// Compute inside = ftgl_edtaa3(1 - bitmap); % Transform background (1's)
	memset(gx, 0, sizeof(*gx)*width*height);
	memset(gy, 0, sizeof(*gy)*width*height);
	for (i = 0; i < width * height; i++) {
		data[i] = 1 - data[i];
	}

	ftgl_computegradient(data, width, height, gx, gy);
	ftgl_edtaa3(data, gx, gy, width, height, xdist, ydist, inside);
	for (i = 0; i < width * height; i++) {
		if (inside[i] < 0.0) {
			inside[i] =  0.0;
		}
	}

	// distmap = outside - inside % Bipolar distance field
	for (i = 0; i < width * height; i++) {
		outside[i] -= inside[i];
		if (outside[i] < vmin) {
			vmin = outside[i];
		}
	}

	vmin = fabs(vmin);

	for (i = 0; i < width * height; i++) {
		double v = outside[i];
		if (v < -vmin) outside[i] = -vmin;
		else if (v > +vmin) outside[i] = +vmin;
		data[i] = (outside[i] + vmin) / (2 * vmin);
	}

	FTGL_FREE(xdist);
	FTGL_FREE(ydist);
	FTGL_FREE(gx);
	FTGL_FREE(gy);
	FTGL_FREE(outside);
	FTGL_FREE(inside);
	return data;
}

FTGLDEF unsigned char * ftgl_distance_mapb(unsigned char *img, unsigned int width,
		   unsigned int height)
{
	double *data = FTGL_CALLOC(width * height, sizeof(*data));
	unsigned char *out = FTGL_MALLOC(width * height * sizeof(*out));
	unsigned int i;

	// find minimum and maximum values
	double img_min = DBL_MAX;
	double img_max = DBL_MIN;

	for (i = 0; i < width * height; i++) {
		double v = img[i];
		data[i] = v;
		if (v > img_max) img_max = v;
		if (v < img_min) img_min = v;
	}

	// Map values from 0 - 255 to 0.0 - 1.0
	for (i = 0; i < width * height; i++)
		data[i] = (img[i]-img_min)/img_max;

	data = ftgl_distance_mapd(data, width, height);

	for (i = 0; i < width * height; i++)
		out[i] = (unsigned char)(255 * (1  - data[i]));
	
	FTGL_FREE(data);
	return out;
}

FTGLDEF ftgl_glyph_t * ftgl_font_load_codepoint(ftgl_font_t *font, uint32_t codepoint)
{
	FT_Error ft_error;
	FT_GlyphSlot slot;
	ftgl_glyph_t *glyph;
	ivec4_t glyph_bbox;
	size_t src_w, src_h, tgt_w, tgt_h;

#define FTGL_GLYPH_OFFSET (1)
	ivec4_t padding = ll_ivec4_create4i( FTGL_GLYPH_OFFSET,
					     FTGL_GLYPH_OFFSET,
					     FTGL_GLYPH_OFFSET,
					     FTGL_GLYPH_OFFSET);

	if ((glyph = ftgl_glyphmap_find_glyph(font->glyphmap, codepoint)) != NULL) {
		return glyph;
	}

	ft_error = FT_Load_Char(font->face, codepoint, FT_LOAD_RENDER);
	if (ft_error != FT_Err_Ok) {
		return NULL;
	}

	slot = font->face->glyph;

	if (font->tbox.x + slot->bitmap.width >= FTGL_FONT_ATLAS_WIDTH) {
		font->tbox.y += font->tbox_yjump + FTGL_GLYPH_OFFSET;
		font->tbox.x = FTGL_GLYPH_OFFSET;
		font->tbox_yjump = 0;
	}

	src_w = slot->bitmap.width;
	src_h = slot->bitmap.rows;

	tgt_w = src_w + padding.x + padding.z;
	tgt_h = src_h + padding.y + padding.w;

	if (font->tbox.y + tgt_h >= FTGL_FONT_ATLAS_HEIGHT) {
		return NULL;
	}

	glyph_bbox = ll_ivec4_create4i(font->tbox.x, font->tbox.y,
				       tgt_w, tgt_h);
	if (ftgl_glyphmap_insert(font->glyphmap, codepoint, glyph_bbox,
				 slot->bitmap_left, slot->bitmap_top,
				 ftgl_F26Dot6_to_float(slot->advance.x),
				 ftgl_F26Dot6_to_float(slot->advance.y)) != FTGL_NO_ERROR) {
		return NULL;
	}

	glyph = ftgl_glyphmap_find_glyph(font->glyphmap, codepoint);

	unsigned char *buffer = FTGL_CALLOC(tgt_w * tgt_h, sizeof(*buffer));
	unsigned char *dst_ptr = buffer + (padding.x * tgt_w + padding.w);
	unsigned char *src_ptr = slot->bitmap.buffer;

	for (size_t i = 0; i < src_h; i++) {
		memcpy(dst_ptr, src_ptr, slot->bitmap.width);
		dst_ptr += tgt_w;
		src_ptr += slot->bitmap.pitch;
	}

	if (font->rendermode == FTGL_RENDERMODE_SDF) {
		unsigned char *sdf = ftgl_distance_mapb(buffer, tgt_w, tgt_h);
		FTGL_FREE(buffer);
		buffer = sdf;
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, font->texture);

	glTexSubImage2D(GL_TEXTURE_2D, 0, font->tbox.x,
			font->tbox.y, tgt_w, tgt_h,
			GL_RED, GL_UNSIGNED_BYTE,
			buffer);
	FTGL_FREE(buffer);

	font->tbox.x += tgt_w + FTGL_GLYPH_OFFSET;
	if (tgt_h > font->tbox_yjump) {
		font->tbox_yjump = tgt_h;
	}
	
#undef FTGL_GLYPH_OFFSET
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	return glyph;
}

FTGLDEF ftgl_glyph_t * ftgl_font_find_glyph(ftgl_font_t *font,
		     uint32_t codepoint)
{
	return ftgl_glyphmap_find_glyph(font->glyphmap, codepoint);
}

FTGLDEF vec2_t ftgl_font_string_dimensions(const char *source, ftgl_font_t *font)
{
	char c;
	vec2_t v;
	size_t i;
	ftgl_glyph_t *glyph;
	float glyph_height;
	v = ll_vec2_origin();
	for (i = 0; (c = source[i]) != '\0'; i++) {
		glyph = ftgl_font_find_glyph(font, c);
		if (!glyph) {
			return ll_vec2_create2f(-1, -1);
		}

		glyph_height = glyph->offset_y;
		if (glyph_height > v.y) {
			v.y = glyph_height;
		}
		v.x += glyph->advance_x;
	}

	return v;
}

FTGLDEF ftgl_string_t * ftgl_string_create(size_t reserve)
{
	ftgl_string_t *s;
	reserve = reserve == 0 ? FTGL_STRING_CAPACITY : reserve;
	s = FTGL_MALLOC(sizeof(*s));
	if (!s) {
		return NULL;
	}

	s->size = 0;
	s->capacity = reserve;
	s->data = FTGL_CALLOC(s->capacity, sizeof(*s->data));
	if (!s->data) {
		FTGL_FREE(s);
		return NULL;
	}

	s->width = 0.0;
	s->height = 0.0;
	s->updated = 0;
	return s;
}

static size_t ftgl_npo2(size_t n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;
	return n;
}

static ftgl_return_t ftgl_string_resize(ftgl_string_t *s)
{
	char *new_data;
	size_t new_capacity;

	new_capacity = s->capacity << 1;
	new_data = FTGL_REALLOC(s->data, sizeof(*new_data) * new_capacity);
	if (!new_data) {
		return FTGL_MEMORY_ERROR;
	}

	s->data = new_data;
	memset(s->data + s->capacity, 0,
	       sizeof(*s->data) * (new_capacity - s->capacity));
	s->capacity = new_capacity;
	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_return_t ftgl_string_write_at(ftgl_string_t *s, ftgl_font_t *font,
		     char *buffer, size_t buffer_len, size_t pos)
{
	ftgl_return_t ret;
	while (buffer_len + pos >= s->capacity) {
		if ((ret = ftgl_string_resize(s)) != FTGL_NO_ERROR) {
			return ret;
		}
	}

	memcpy(s->data + pos, buffer, buffer_len);
	if (buffer_len + pos > s->size) 
		s->size = buffer_len + pos;
	s->data[s->size] = '\0';
	s->updated = 1;
	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_return_t ftgl_string_write(ftgl_string_t *s, ftgl_font_t *font,
		  char *buffer, size_t buffer_len)
{
	ftgl_return_t ret;
	while (buffer_len >= s->capacity) {
		if ((ret = ftgl_string_resize(s)) != FTGL_NO_ERROR) {
			return ret;
		}
	}

	memcpy(s->data, buffer, buffer_len);
	s->size = buffer_len;
	s->data[s->size] = '\0';
	s->updated = 1;
	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_return_t ftgl_string_append(ftgl_string_t *s, ftgl_font_t *font,
		   char *buffer, size_t buffer_len)
{
	return ftgl_string_write_at(s, font, buffer, buffer_len, s->size);
}

FTGLDEF vec2_t ftgl_string_dimensions(ftgl_string_t *s, ftgl_font_t *font)
{
	vec2_t v;
	size_t i;
	ftgl_glyph_t *glyph;
	float glyph_height;
	if (!s->updated) {
		return ll_vec2_create2f(s->width, s->height);
	}

	v = ll_vec2_origin();
	for (i = 0; i < s->size; i++) {
		glyph = ftgl_font_find_glyph(font, s->data[i]);
		if (!glyph) {
			return ll_vec2_create2f(-1, -1);
		}

		glyph_height = glyph->offset_y;
		if (glyph_height > v.y) {
			v.y = glyph_height;
		}
		v.x += glyph->advance_x;
	}

	s->width = v.x;
	s->height = v.y;
	s->updated = 0;
	return v;
}

FTGLDEF void ftgl_string_free(ftgl_string_t *s)
{
	FTGL_FREE(s->data);
	s->data = NULL;
	s->size = 0;
	s->capacity = 0;
	FTGL_FREE(s);
}

FTGLDEF void ftgl_font_free(ftgl_font_t *font)
{
	glDeleteTextures(1, &font->texture);
	FT_Done_Face(font->face);
	ftgl_glyphmap_free(font->glyphmap);
	font->face = NULL;
	font->glyphmap = NULL;
	font->tbox = ll_ivec2_create2i(0,0);
	font->tbox_yjump = 0;
	font->scale = 0.0;
	FTGL_FREE(font);
}

static void ftgl_font_node_free(ftgl_font_node_t *font_node)
{
	FTGL_FREE(font_node->name);
	ftgl_font_free(font_node->font);
	FTGL_FREE(font_node);
}	

static void ftgl_font_manager_free(void)
{
	size_t i;
	ftgl_font_node_t *font_node;
	for (i = 0; i < ftgl_font_manager.capacity; i++) {
		font_node = ftgl_font_manager.nodes[i];
		if (font_node) {
			ftgl_font_node_free(font_node);
		}
	}
	FTGL_FREE(ftgl_font_manager.nodes);
	memset(&ftgl_font_manager, 0, sizeof(ftgl_font_manager));
}

FTGLDEF void ftgl_font_library_free(void)
{
	ftgl_font_manager_free();
}

#endif /* FTGL_IMPLEMENTATION */
#endif /* FTGL_FONT_H_ */
