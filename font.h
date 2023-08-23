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

FTGLDEF void
ftgl_font_render_text(ftgl_font_t *font,
		       const char *text);

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
	glyph = malloc(sizeof(*glyph));
	if (!glyph) {
		
	}
}

static void
ftgl_glyph_free(ftgl_glyph_t *glyph)
{

}

static ftgl_glyphlist_t *
ftgl_glyphlist_create4iv(uint32_t codepoint, ivec4_t bbox, GLint offset_x,
			 GLint offset_y, GLfloat advance_x, GLfloat advance_y)
{

}

static void
ftgl_glyphlist_free(ftgl_glyphlist_t *glyphlist)
{

}

static ftgl_glyphmap_t *
ftgl_glyphmap_create(void)
{

}

static ftgl_return_t
ftgl_glyphmap_insert(ftgl_glyphmap_t *glyphmap,
		     uint32_t codepoint, ivec4_t bbox, GLint offset_x,
		     GLint offset_y, GLfloat advance_x, GLfloat advance_y)
{

}

static ftgl_glyph_t *
ftgl_glyphmap_find_glyph(ftgl_glyphmap_t *glyphmap,
			 uint32 codepoint)
{

}

static void
ftgl_glyphmap_free(ftgl_glyphmap_t *glyphmap)
{

}

FTGLDEF ftgl_return_t
ftgl_font_library_init(void)
{

}

FTGLDEF ftgl_font_t *
ftgl_font_create(void)
{

}

FTGLDEF ftgl_return_t
ftgl_font_bind(ftgl_font_t *font, const char *path)
{

}

FTGLDEF ftgl_return_t
ftgl_font_set_size(ftgl_font_t *font, float size)
{

}

FTGLDEF ftgl_glyph_t *
ftgl_font_load_codepoint(ftgl_font_t *font, uint32_t codepoint)
{

}

FTGLDEF ftgl_glyph_t *
ftgl_font_find_glyph(ftgl_font_t *font,
		      uint32_t codepoint)
{

}

FTGLDEF void
ftgl_font_render_text(ftgl_font_t *font,
		       const char *text)
{

}

FTGLDEF void
ftgl_font_free(ftgl_font_t *font)
{

}

#endif /* FTGL_IMPLEMENTATION */
#endif /* FTGL_FONT_H_ */
