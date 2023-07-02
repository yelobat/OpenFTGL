#include "../include/texture-atlas.h"
#include "../include/texture-font.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static ftgl_return_t
ftgl_texture_atlas_special(ftgl_texture_atlas_t *atlas)
{
        ivec4_t region = ftgl_texture_atlas_get_region(atlas, 5, 5);
        ftgl_texture_glyph_t *glyph = ftgl_texture_glyph_create();
        if (!glyph) {
                return FTGL_MEMORY_ERROR;
        }
        
        static unsigned char data[4*4*3] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                                            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
        if (region.x < 0) {
                return FTGL_ATLAS_FULL_ERROR;
        }

        ftgl_texture_atlas_set_region(atlas, region.x, region.y,
                                      4, 4, data, 0);
        glyph->codepoint = -1;
        glyph->s0 = (region.x + 2) / (float) atlas->width;
        glyph->t0 = (region.y + 2) / (float) atlas->height;
        glyph->s1 = (region.x + 3) / (float) atlas->width;
        glyph->t1 = (region.y + 3) / (float) atlas->height;

        atlas->special = (void *) glyph;
        return FTGL_NO_ERROR;
}

static int
ftgl_texture_atlas_fit(ftgl_texture_atlas_t *atlas,
                       const size_t index,
                       const size_t width,
                       const size_t height)
{
        ivec3_t *node;
        int x, y, width_left;
        size_t i;

#ifdef FTGL_DEBUG
        assert(atlas);
#endif /* FTGL_DEBUG */

        node = (ivec3_t *) (ftgl_vector_get(atlas->nodes, index));
        x = node->x;
        y = node->y;
        width_left = width;
        i = index;

        if ((x + width) > (atlas->width - 1)) {
                return -1;
        }

        y = node->y;
        while (width_left > 0) {
                node = (ivec3_t *) (ftgl_vector_get(atlas->nodes, i));
                if (node->y > y) {
                        y = node->y;
                }

                if ((y + height) > (atlas->height - 1)) {
                        return -1;
                }

                width_left -= node->z;
                ++i;
        }

        return y;
}

static void
ftgl_texture_atlas_merge(ftgl_texture_atlas_t *atlas)
{
        ivec3_t *node, *next;
        size_t i;

#ifdef FTGL_DEBUG
        assert(atlas);
#endif /* FTGL_DEBUG */

        for (i = 0; i < atlas->nodes->size - 1; ++i) {
                node = (ivec3_t *) ftgl_vector_get(atlas->nodes, i);
                next = (ivec3_t *) ftgl_vector_get(atlas->nodes, i + 1);
                if (node->y == next->y) {
                        node->z += next->z;
                        ftgl_vector_erase(atlas->nodes, i + 1);
                        --i;
                }
        }
}

ftgl_texture_atlas_t *
ftgl_texture_atlas_create(const size_t width,
                          const size_t height,
                          const size_t depth)
{
#ifdef FTGL_DEBUG
        assert((depth == 1) || (depth == 3) || (depth == 4));
#endif /* FTGL_DEBUG */
        ftgl_texture_atlas_t *atlas = malloc(sizeof(*atlas));
        if (!atlas) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate texture atlas!");
#endif /* FTGL_DEBUG */
                return NULL;
        }

        ivec3_t node = ll_ivec3_create(1, 1, width-2);
        atlas->nodes = ftgl_vector_create(sizeof(ivec3_t));
        if (!atlas->nodes) {
                free(atlas);
                return NULL;
        }

        atlas->used = 0;
        atlas->width = width;
        atlas->height = height;
        atlas->depth = depth;
        atlas->id = 0;
        atlas->modified = 1;

        if (ftgl_vector_push_back(atlas->nodes, &node) != FTGL_NO_ERROR) {
                ftgl_vector_free(atlas->nodes);
                free(atlas);
                return NULL;
        }

        atlas->data = calloc(width*height*depth, sizeof(*atlas->data));
        if (!atlas->data) {
                ftgl_vector_free(atlas->nodes);
                free(atlas);
                return NULL;
        }

        if (ftgl_texture_atlas_special(atlas) != FTGL_NO_ERROR) {
                ftgl_vector_free(atlas->nodes);
                free(atlas);
                return NULL;
        }
        
        return atlas;
}


void
ftgl_texture_atlas_free(ftgl_texture_atlas_t *atlas)
{
#ifdef FTGL_DEBUG
        assert(atlas);
#endif /* FTGL_DEBUG */
        ftgl_vector_free(atlas->nodes);
        ftgl_texture_glyph_free(atlas->special);
        if (atlas->data) {
                free(atlas->data);
        }
        free(atlas);
}

ivec4_t
ftgl_texture_atlas_get_region(ftgl_texture_atlas_t *atlas,
                              const size_t width,
                              const size_t height)
{
        int y, best_index;
        size_t best_height, best_width;
        ivec3_t *node, *prev;
        ivec4_t region;
        size_t i;

#ifdef FTGL_DEBUG
        assert(atlas);
#endif /* FTGL_DEBUG */

        region = ll_ivec4_create(0, 0, width, height);
        best_height = UINT_MAX;
        best_index = -1;
        best_width = UINT_MAX;
        for (i = 0; i < atlas->nodes->size; ++i) {
                y = ftgl_texture_atlas_fit(atlas, i, width, height);
                if (y >= 0) {
                        node = (ivec3_t *) ftgl_vector_get(atlas->nodes, i);
                        if(((y + height) < best_height ) ||
                           (((y + height) == best_height) && (node->z > 0 && (size_t)node->z < best_width)) ) {
                                best_height = y + height;
                                best_index = i;
                                best_width = node->z;
                                region.x = node->x;
                                region.y = y;
                        }
                }
        }

        if (best_index == -1) {
                region.x = -1;
                region.y = -1;
                region.z = 0;
                region.w = 0;
                return region;
        }

        node = malloc(sizeof(ivec3_t));
        if (!node) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate ivec3 structure!\n");
#endif /* FTGL_DEBUG */
                return (ivec4_t) {{ -1, -1, 0, 0 }};
        }

        node->x = region.x;
        node->y = region.y + height;
        node->z = width;
        if (ftgl_vector_insert(atlas->nodes, best_index, node) != FTGL_NO_ERROR) {
                free(node);
                return (ivec4_t) {{ -1, -1, 0, 0 }};
        }
        free(node);

        for (i = best_index + 1; i < atlas->nodes->size; ++i) {
                node = (ivec3_t *) ftgl_vector_get(atlas->nodes, i);
                prev = (ivec3_t *) ftgl_vector_get(atlas->nodes, i - 1);

                if (node->x < (prev->x + prev->z)) {
                        int shrink = prev->x + prev->z - node->x;
                        node->x += shrink;
                        node->z -= shrink;
                        if (node->z <= 0) {
                                ftgl_vector_erase(atlas->nodes, i);
                                --i;
                        } else {
                                break;
                        }
                } else {
                        break;
                }
        }
        ftgl_texture_atlas_merge(atlas);
        atlas->used += width * height;
        atlas->modified = 1;
        return region;
}

void
ftgl_texture_atlas_set_region(ftgl_texture_atlas_t *atlas,
                              const size_t x,
                              const size_t y,
                              const size_t width,
                              const size_t height,
                              const unsigned char *data,
                              const size_t stride)
{
        size_t i;
        size_t depth;
        size_t charsize;

#ifdef FTGL_DEBUG
        assert(atlas);
        assert(x > 0);
        assert(y > 0);
        assert(x < (atlas->width -1));
        assert((x + width) <= (atlas->width - 1));
        assert(y < (atlas->height - 1));
        assert((y + height) <= (atlas->height - 1));
        assert(height == 0 || (data != NULL && width > 0));
#endif /* FTGL_DEBUG */

        depth = atlas->depth;
        charsize = sizeof(char);
        for (i = 0; i < height; ++i) {
                memcpy(atlas->data + ((y + i) * atlas->width + x) * charsize * depth,
                       data + (i*stride) * charsize, width * charsize * depth);
        }

        atlas->modified = 1;
}

ftgl_return_t
ftgl_texture_atlas_clear(ftgl_texture_atlas_t *atlas)
{
        ftgl_return_t ret;
        ivec3_t node;

#ifdef FTGL_DEBUG
        assert(atlas);
        assert(atlas->data);
#endif /* FTGL_DEBUG */

        node = ll_ivec3_create(1, 1, atlas->width - 2);

        ftgl_vector_clear(atlas->nodes);
        atlas->used = 0;

        if ((ret = ftgl_vector_push_back(atlas->nodes, &node)) != FTGL_NO_ERROR)
                return ret;
        memset(atlas->data, 0, atlas->width * atlas->height * atlas->depth);
        return FTGL_NO_ERROR;
}

ftgl_return_t
ftgl_texture_atlas_enlarge_texture(ftgl_texture_atlas_t *atlas,
                                   size_t width_new,
                                   size_t height_new)
{
        ftgl_return_t ret;
#ifdef FTGL_DEBUG
        assert(atlas);
        assert(width_new >= atlas->width);
        assert(height_new >= atlas->height);
        assert(width_new + height_new > atlas->width + atlas->height);
#endif /* FTGL_DEBUG */

        size_t width_old = atlas->width;
        size_t height_old = atlas->height;

        unsigned char *data_old = atlas->data;
        atlas->data = calloc(1, width_new * height_new * atlas->depth);
        if (!atlas->data) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to enlarge atlas texture!");
#endif /* FTGL_DEBUG */
                atlas->data = data_old;
                return FTGL_MEMORY_ERROR;
        }

        atlas->width = width_new;
        atlas->height = height_new;

        if (width_new > width_old) {
                ivec3_t node;
                node.x = width_old - 1;
                node.y = 1;
                node.z = width_new - width_old;
                if ((ret = ftgl_vector_push_back(atlas->nodes, &node)
                     ) != FTGL_NO_ERROR) {
                        free(atlas->data);
                        atlas->data = data_old;
                        atlas->width = width_old;
                        atlas->height = height_old;
                        return ret;
                }
        }

        size_t pixel_size = sizeof(char) * atlas->depth;
        size_t old_row_size = width_old * pixel_size;
        ftgl_texture_atlas_set_region(atlas, 1, 1, width_old - 2, height_old - 2,
                                      data_old + old_row_size + pixel_size, old_row_size);
        free(data_old);
        return FTGL_NO_ERROR;
}
