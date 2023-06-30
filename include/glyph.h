#ifndef FTGL_GLYPH_H_
#define FTGL_GLYPH_H_

#include "log.h"
#include <stdlib.h>
#include <stdint.h>

/**
 * The glyph structure
 */
typedef struct ftgl_glyph_t {
        /**
         * Unicode codepoint this glyph represents in UTF-32 LE encoding.
         */ 
        uint32_t codepoint;

        /**
         * Glyph's width in pixels
         */
        size_t width;

        /**
         * Glyph's height in pixels
         */
        size_t height;

        /**
         * Glyph's left bearing expressed in integer pixels.
         */
        int offset_x;

        /**
         * Glyph's top bearing expressed in integer pixels
         *
         * Remember that this is the distance from the baseline to the top-most
         * glyph scanline, upwards y coordinates being positive.
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
         * Glyph outline thickness
         */
        float outline_thickness;
} ftgl_glyph_t;

/**
 * Creates a glyph structure 
 */ 
ftgl_glyph_t *
ftgl_glyph_create(void);

/**
 * Free's the storage held by @glyph.
 */
void
ftgl_glyph_free(ftgl_glyph_t *glyph);

#endif /* FTGL_GLYPH_H_ */
