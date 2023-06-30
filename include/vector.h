#ifndef FTGL_VECTOR_H_
#define FTGL_VECTOR_H_

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "log.h"

typedef struct ftgl_vector_t {
        /**
         * Pointer to dynamically allocated items.
         */
        void *items;

        /**
         * Number of items that can be held in currently
         * allocated storage.
         */
        size_t capacity;

        /**
         * Number of items.
         */
        size_t size;

        /**
         * Size (in bytes) of a single item.
         */
        size_t item_size;
} ftgl_vector_t;

ftgl_vector_t *
ftgl_vector_create(size_t item_size);

void
ftgl_vector_free(ftgl_vector_t *vec);

const void *
ftgl_vector_get(const ftgl_vector_t *vec,
                size_t index);

const void *
ftgl_vector_front(const ftgl_vector_t *vec);

const void *
ftgl_vector_back(const ftgl_vector_t *vec);

int
ftgl_vector_contains(const ftgl_vector_t *vec,
                     const void *item,
                     int (*cmp)(const void *, const void *));

int
ftgl_vector_empty(const ftgl_vector_t *vec);

size_t
ftgl_vector_size(const ftgl_vector_t *vec);

ftgl_return_t
ftgl_vector_reserve(ftgl_vector_t *vec,
                    const size_t size);

size_t
ftgl_vector_capacity(const ftgl_vector_t *vec);

ftgl_return_t
ftgl_vector_shrink(ftgl_vector_t *vec);

void
ftgl_vector_clear(ftgl_vector_t *vec);

void
ftgl_vector_set(ftgl_vector_t *vec,
                const size_t index,
                const void *item);

void
ftgl_vector_erase(ftgl_vector_t *vec,
                  const size_t index);

void
ftgl_vector_erase_range(ftgl_vector_t *vec,
                        const size_t first,
                        const size_t last);

ftgl_return_t
ftgl_vector_push_back(ftgl_vector_t *vec,
                      const void *item);

void
ftgl_vector_pop_back(ftgl_vector_t *vec);

ftgl_return_t
ftgl_vector_resize(ftgl_vector_t *vec,
                   const size_t size);

ftgl_return_t
ftgl_vector_insert(ftgl_vector_t *vec,
                   const size_t index,
                   const void *item);

ftgl_return_t
ftgl_vector_insert_data(ftgl_vector_t *vec,
                        const size_t index,
                        const void *data,
                        const size_t count);

ftgl_return_t
ftgl_vector_push_back_data(ftgl_vector_t *vec,
                           const void *data,
                           const size_t count);

void
ftgl_vector_sort(ftgl_vector_t *vec,
                 int (*cmp)(const void *, const void *));

#endif /* FTGL_VECTOR_H_ */
