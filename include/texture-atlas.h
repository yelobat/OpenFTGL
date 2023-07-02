#ifndef FTGL_TEXTURE_ATLAS_H_
#define FTGL_TEXTURE_ATLAS_H_

#include <stdlib.h>
#include "vector.h"
#include "linear.h"
#include "log.h"

typedef struct ftgl_texture_atlas_t {
        /**
         * Allocated nodes
         */
        ftgl_vector_t *nodes;

        /**
         * Width (in pixels) of the underlying texture
         */
        size_t width;

        /**
         * Height (in pixels) of the underlying texture
         */
        size_t height;

        /**
         * Depth (in bytes) of the underlying texture
         */
        size_t depth;

        /**
         * Allocated surface size.
         */
        size_t used;

        /**
         * Texture identity (OpenGL)
         */
        unsigned int id;

        /**
         * Atlas data
         */
        unsigned char *data;

        /**
         * Atlas has been modified.
         */
        unsigned char modified;

        /**
         * Atlas special glyph, this is a void *, and will be typecasted
         * as necessary.
         */
        void *special;
} ftgl_texture_atlas_t;

ftgl_texture_atlas_t *
ftgl_texture_atlas_create(const size_t width,
                          const size_t height,
                          const size_t depth);


void
ftgl_texture_atlas_free(ftgl_texture_atlas_t *atlas);

ivec4_t
ftgl_texture_atlas_get_region(ftgl_texture_atlas_t *atlas,
                              const size_t width,
                              const size_t height);

void
ftgl_texture_atlas_set_region(ftgl_texture_atlas_t *atlas,
                              const size_t x,
                              const size_t y,
                              const size_t width,
                              const size_t height,
                              const unsigned char *data,
                              const size_t stride);

ftgl_return_t
ftgl_texture_atlas_clear(ftgl_texture_atlas_t *atlas);

ftgl_return_t
ftgl_texture_atlas_enlarge_texture(ftgl_texture_atlas_t *atlas,
                                   size_t width_new,
                                   size_t height_new);
#endif /* FTGL_TEXTURE_ATLAS_H_ */
