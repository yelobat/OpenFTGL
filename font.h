/**
 * @author: wwotz
 *
 * @description: Use FreeType with OpenGL to render text using a single
 * texture buffer. This project was inspired by rougier's freetype-gl
 * repository found at: https://github.com/rougier/freetype-gl
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

#if !defined(FTGL_MALLOC) || !defined(FTGL_REALLOC) || !defined(FTGL_FREE)
#define FTGL_MALLOC(sz) malloc(sz)
#define FTGL_REALLOC(x, newsz) realloc(x, newsz)
#define FTGL_FREE(x) free(x)
#endif

typedef enum ftgl_return_t {
	FTGL_NO_ERROR = 0,
	FTGL_MEMORY_ERROR,
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

typedef struct ftgl_font_t {
	/**
	 * Stores all the textures for which
	 * the glyphs are stored inside of.
	 */
	GLuint *textures;

	/**
	 * The number of allocated textures for the glyphs.
	 */
	GLsizei count;

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
} ftgl_font_t;

FTGLDEF ftgl_return_t
ftgl_font_library_init(void);

FTGLDEF ftgl_font_t *
ftgl_font_create(void);

FTGLDEF ftgl_return_t
ftgl_font_bind(ftgl_font_t *font, const char *path);

FTGLDEF ftgl_return_t
ftgl_font_set_size(ftgl_font_t *font, float size);

FTGLDEF ftgl_glyph_t *
ftgl_font_load_codepoint(ftgl_font_t *font, uint32_t codepoint);

FTGLDEF ftgl_glyph_t *
ftgl_font_find_glyph(ftgl_font_t *font,
		      uint32_t codepoint);

FTGLDEF vec2_t
ftgl_font_string_dimensions(const char *source,
			    ftgl_font_t *font);

FTGLDEF void
ftgl_font_free(ftgl_font_t *font);

#ifdef FTGL_IMPLEMENTATION

FT_Library ftgl_font_library;

static float
ftgl_F26Dot6_to_float(FT_F26Dot6 value)
{
	return ((float) value) / 64.0;
}

static FT_F26Dot6
ftgl_float_to_F26Dot6(float value)
{
	return (FT_F26Dot6) (value * 64.0);
}

static ftgl_glyph_t *
ftgl_glyph_create4iv(uint32_t codepoint, ivec4_t bbox, GLint offset_x,
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

static void
ftgl_glyph_free(ftgl_glyph_t *glyph)
{
	glyph->bbox = ll_ivec4_create4i(0,0,0,0);
	glyph->codepoint = 0;
	glyph->offset_x = 0;
	glyph->offset_y = 0;
	glyph->advance_x = 0;
	glyph->advance_y = 0;
	FTGL_FREE(glyph);
}

static ftgl_glyphlist_t *
ftgl_glyphlist_create4iv(uint32_t codepoint, ivec4_t bbox, GLint offset_x,
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

static void
ftgl_glyphlist_free(ftgl_glyphlist_t *glyphlist)
{
	ftgl_glyph_free(glyphlist->glyph);
	glyphlist->glyph = NULL;
	glyphlist->next = NULL;
	FTGL_FREE(glyphlist);
}

static ftgl_glyphmap_t *
ftgl_glyphmap_create(void)
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

static ftgl_glyph_t *
ftgl_glyphmap_find_glyph(ftgl_glyphmap_t *glyphmap,
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

static ftgl_return_t
ftgl_glyphmap_insert(ftgl_glyphmap_t *glyphmap,
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

static void
ftgl_glyphmap_free(ftgl_glyphmap_t *glyphmap)
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

FTGLDEF ftgl_return_t
ftgl_font_library_init(void)
{
	FT_Error ft_error;
	if ((ft_error = FT_Init_FreeType(&ftgl_font_library)) != FT_Err_Ok) {
		return FTGL_FREETYPE_ERROR;
	}

	return FTGL_NO_ERROR;
}

FTGLDEF ftgl_font_t *
ftgl_font_create(void)
{
	GLenum gl_error;
	ftgl_font_t *font;
	size_t i;

	font = FTGL_MALLOC(sizeof(*font));
	if (!font) {
		return NULL;
	}

	font->count = 1;
	font->textures = FTGL_MALLOC(sizeof(*font->textures)
				     * font->count);
	if (!font->textures) {
		font->count = 0;
		FTGL_FREE(font);
		return NULL;
	}

	font->tbox = ll_ivec2_create2i(5,5);
	font->tbox_yjump = 0;

	font->glyphmap = ftgl_glyphmap_create();
	if (!font->glyphmap) {
		FTGL_FREE(font->textures);
		FTGL_FREE(font);
		return NULL;
	}

	glGenTextures(1, font->textures+0);
	if ((gl_error = glGetError()) != GL_NO_ERROR) {
		FTGL_FREE(font->textures);
		ftgl_glyphmap_free(font->glyphmap);
		FTGL_FREE(font);
		return NULL;
	}

	glBindTexture(GL_TEXTURE_2D, font->textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FTGL_FONT_ATLAS_WIDTH,
		     FTGL_FONT_ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	if ((gl_error = glGetError()) != GL_NO_ERROR) {
		glBindTexture(GL_TEXTURE_2D, 0);
		FTGL_FREE(font->textures);
		ftgl_glyphmap_free(font->glyphmap);
		FTGL_FREE(font);
		return NULL;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	font->scale = 1.0;
	font->face = NULL;
	return font;
}

FTGLDEF ftgl_return_t
ftgl_font_bind(ftgl_font_t *font, const char *path)
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

FTGLDEF ftgl_return_t
ftgl_font_set_size(ftgl_font_t *font, float size)
{
	FT_Error ft_error;
	FT_Matrix matrix = {
		(int)((1.0/FTGL_FONT_HRES) * 0x10000L),
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

FTGLDEF ftgl_glyph_t *
ftgl_font_load_codepoint(ftgl_font_t *font, uint32_t codepoint)
{
	FT_Error ft_error;
	FT_GlyphSlot slot;
	ftgl_glyph_t *glyph;
	ivec4_t glyph_bbox;

	if ((glyph = ftgl_glyphmap_find_glyph(font->glyphmap, codepoint)) != NULL) {
		return glyph;
	}

	ft_error = FT_Load_Char(font->face, codepoint, FT_LOAD_RENDER);
	if (ft_error != FT_Err_Ok) {
		return NULL;
	}

	slot = font->face->glyph;

	if (font->tbox.x + slot->bitmap.width >= FTGL_FONT_ATLAS_WIDTH) {
		font->tbox.y += font->tbox_yjump + 5;
		font->tbox.x = 5;
		font->tbox_yjump = 0;
	}

	if (font->tbox.y + slot->bitmap.rows >= FTGL_FONT_ATLAS_HEIGHT) {
		return NULL;
	}

	glyph_bbox = ll_ivec4_create4i(font->tbox.x, font->tbox.y,
				     slot->bitmap.width, slot->bitmap.rows);
	if (ftgl_glyphmap_insert(font->glyphmap, codepoint, glyph_bbox,
				 slot->bitmap_left, slot->bitmap_top,
				 ftgl_F26Dot6_to_float(slot->advance.x),
				 ftgl_F26Dot6_to_float(slot->advance.y)) != FTGL_NO_ERROR) {
		return NULL;
	}
	
	glyph = ftgl_glyphmap_find_glyph(font->glyphmap, codepoint);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, font->textures[0]);

	glTexSubImage2D(GL_TEXTURE_2D, 0, font->tbox.x,
			font->tbox.y, slot->bitmap.width,
			slot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
			slot->bitmap.buffer);

	font->tbox.x += slot->bitmap.width + 5;
	if (slot->bitmap.rows > font->tbox_yjump) {
		font->tbox_yjump = slot->bitmap.rows;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	return glyph;
}

FTGLDEF ftgl_glyph_t *
ftgl_font_find_glyph(ftgl_font_t *font,
		     uint32_t codepoint)
{
	return ftgl_glyphmap_find_glyph(font->glyphmap, codepoint);
}

FTGLDEF vec2_t
ftgl_font_string_dimensions(const char *source,
			    ftgl_font_t *font)
{
	vec2_t vec;
	const char *p;
	ftgl_glyph_t *glyph;

	vec = ll_vec2_origin();
	p = source;
	while (*p++ != '\0') {
		glyph = ftgl_font_find_glyph(font, *p);
		if (!glyph) {
			return vec;
		}
		vec.x += glyph->advance_x;
		if (vec.y < glyph->advance_y)
			vec.y = glyph->advance_y;
	}

	return vec;
}

FTGLDEF void
ftgl_font_free(ftgl_font_t *font)
{
	glDeleteTextures(font->count, font->textures);
	FT_Done_Face(font->face);
	FTGL_FREE(font->textures);
	ftgl_glyphmap_free(font->glyphmap);
	font->face = NULL;
	font->textures = NULL;
	font->glyphmap = NULL;
	font->tbox = ll_ivec2_create2i(0,0);
	font->tbox_yjump = 0;
	font->scale = 0.0;
	FTGL_FREE(font);
}

#endif /* FTGL_IMPLEMENTATION */
#endif /* FTGL_FONT_H_ */
