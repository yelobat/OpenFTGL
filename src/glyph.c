#include "../include/glyph.h"

ftgl_glyph_t *
ftgl_glyph_create(void)
{
        ftgl_glyph_t *glyph = malloc(sizeof(*glyph));
        if (!glyph) {
#ifdef FTGL_DEBUG 
                FTGL_LOG_MESSAGE("Failed to allocate glyph!");
#endif /* FTGL_DEBUG */
                return NULL;
        }

        glyph->codepoint = -1;
        glyph->width = 0;
        glyph->height = 0;
        glyph->outline_thickness = 0.0;
        glyph->offset_x = 0;
        glyph->offset_y = 0;
        glyph->advance_x = 0.0;
        glyph->advance_y = 0.0;
        glyph->s0 = 0.0;
        glyph->t0 = 0.0;
        glyph->s1 = 0.0;
        glyph->t1 = 0.0;
        return glyph;
}

void
ftgl_glyph_free(ftgl_glyph_t *glyph)
{
        int i;
#ifdef FTGL_DEBUG
        assert(glyph);
#endif /* FTGL_DEBUG */
        free(glyph);
}
        
