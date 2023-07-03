#include "../include/vector.h"

ftgl_vector_t *
ftgl_vector_create(size_t item_size)
{
        ftgl_vector_t *vec = malloc(sizeof(*vec));
        if (!vec) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate vector!");
#endif /* FTGL_DEBUG */
                return NULL;
        }
        vec->item_size = item_size;
        vec->size = 0;
        vec->capacity = 1;
        vec->items = calloc(vec->item_size, vec->capacity);
        if (!vec->items) {
#ifdef FTGL_DEBUG
                FTGL_LOG_MESSAGE("Failed to allocate items in vector!");
#endif /* FTGL_DEBUG */
                free(vec);
                return NULL;
        }
        return vec;
}

void
ftgl_vector_free(ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */
        vec->item_size = 0;
        vec->size = 0;
        vec->capacity = 0;
        free(vec->items);
        free(vec);
}

const void *
ftgl_vector_get(const ftgl_vector_t *vec,
                size_t index)
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(vec->size);
        assert(index < vec->size);
#endif /* FTGL_DEBUG */
        return (char *)(vec->items) + index * vec->item_size;
}

const void *
ftgl_vector_front(const ftgl_vector_t *vec)
{
        return ftgl_vector_get(vec, 0);
}

const void *
ftgl_vector_back(const ftgl_vector_t *vec)
{
        return ftgl_vector_get(vec, vec->size-1);
}

int
ftgl_vector_contains(const ftgl_vector_t *vec,
                     const void *item,
                     int (*cmp)(const void *, const void *))
{
        size_t i;
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */

        for (i = 0; i < vec->size; ++i) {
                if ((*cmp)(item, ftgl_vector_get(vec, i)) == 0) {
                        return 1;
                }
        }
        return 0;
}

int
ftgl_vector_empty(const ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */
        return vec->size == 0;
}

size_t
ftgl_vector_size(const ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */
        return vec->size;
}

ftgl_return_t
ftgl_vector_reserve(ftgl_vector_t *vec,
                    const size_t size)
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(vec->items);
#endif /* FTGL_DEBUG */

        if (vec->capacity < size) {
                void *new_items = realloc(vec->items, size * vec->item_size);
                if (!new_items) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE("Failed to reserve more items for vector!");
#endif /* FTGL_DEBUG */
                        return FTGL_MEMORY_ERROR;
                }
                vec->items = new_items;
                memset((char *)(vec->items) + (vec->capacity * vec->item_size), 0,
                       (size - vec->capacity) * vec->item_size);
                vec->capacity = size;
        }

        return FTGL_NO_ERROR;
}

size_t
ftgl_vector_capacity(const ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */
        return vec->capacity;
}

ftgl_return_t 
ftgl_vector_shrink(ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */
        if (vec->capacity > vec->size) {
                void *new_items = realloc(vec->items, vec->size * vec->item_size);
                if (!new_items) {
#ifdef FTGL_DEBUG
                        FTGL_LOG_MESSAGE("Failed to shrink vector item storage!");
#endif /* FTGL_DEBUG */
                        return FTGL_MEMORY_ERROR;
                }
                vec->items = new_items;
        }
        vec->capacity = vec->size;
        return FTGL_NO_ERROR;
}

void
ftgl_vector_clear(ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */
        memset((char *)(vec->items), 0, vec->size * vec->item_size);
        vec->size = 0;
}

void
ftgl_vector_set(ftgl_vector_t *vec,
                const size_t index,
                const void *item)
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(vec->size);
        assert(index < vec->size);
#endif /* FTGL_DEBUG */
        memcpy((char *)(vec->items) + index * vec->item_size,
               item, vec->item_size);
}

void
ftgl_vector_erase(ftgl_vector_t *vec,
                  const size_t index)
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(index < vec->size);
#endif /* FTGL_DEBUG */
        ftgl_vector_erase_range(vec, index, index+1);
}

void
ftgl_vector_erase_range(ftgl_vector_t *vec,
                        const size_t first,
                        const size_t last)
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(first < vec->size);
        assert(last < vec->size + 1);
        assert(first < last);
#endif /* FTGL_DEBUG */

        memmove((char *)(vec->items) + first * vec->item_size,
                (char *)(vec->items) + last * vec->item_size,
                (vec->size - last) * vec->item_size);
        vec->size -= (last-first);
}

ftgl_return_t
ftgl_vector_push_back(ftgl_vector_t *vec,
                      const void *item)
{
        return ftgl_vector_insert(vec, vec->size, item);
}

void
ftgl_vector_pop_back(ftgl_vector_t *vec)
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(vec->size);
#endif /* FTGL_DEBUG */
        vec->size--;
}

ftgl_return_t
ftgl_vector_resize(ftgl_vector_t *vec,
                   const size_t size)
{
        ftgl_return_t ret;
#ifdef FTGL_DEBUG
        assert(vec);
#endif /* FTGL_DEBUG */

        if (size > vec->capacity) {
                if ((ret = ftgl_vector_reserve(vec, size)) != FTGL_NO_ERROR)
                        return ret;
                vec->size = vec->capacity;
        } else {
                vec->size = size;
        }

        return FTGL_NO_ERROR;
}

ftgl_return_t 
ftgl_vector_insert(ftgl_vector_t *vec,
                   const size_t index,
                   const void *item)
{
        ftgl_return_t ret;
#ifdef FTGL_DEBUG
        assert(vec);
        assert(index <= vec->size);
        assert(item);
#endif /* FTGL_DEBUG */

        if (vec->capacity <= vec->size) {
                if ((ret = ftgl_vector_reserve(vec, 2 * vec->capacity))
                    != FTGL_NO_ERROR)
                        return ret;
        }

        if (index < vec->size) {
                memmove((char *)(vec->items) + ((index + 1) * vec->item_size),
                        (char *)(vec->items) + ((index + 0) * vec->item_size),
                        (vec->size - index) * vec->item_size);
        }
        
        vec->size++;
        ftgl_vector_set(vec, index, item);
        return FTGL_NO_ERROR;
}

ftgl_return_t
ftgl_vector_insert_data(ftgl_vector_t *vec,
                        const size_t index,
                        const void *data,
                        const size_t count)
{
        ftgl_return_t ret;
#ifdef FTGL_DEBUG
        assert(vec);
        assert(index < vec->size);
        assert(data);
        assert(count);
#endif /* FTGL_DEBUG */

        if (vec->capacity < (vec->size + count)) {
                if ((ret = ftgl_vector_reserve(vec, vec->size + count))
                    != FTGL_NO_ERROR)
                        return ret;
        }

        memmove((char *)(vec->items) + (index + count) * vec->item_size,
                (char *)(vec->items) + (index +     0) * vec->item_size,
                count * vec->item_size);
        memmove((char *)(vec->items) + index * vec->item_size, data,
                count * vec->item_size);
        vec->size += count;
        return FTGL_NO_ERROR;
}

ftgl_return_t
ftgl_vector_push_back_data(ftgl_vector_t *vec,
                           const void *data,
                           const size_t count)
{
        ftgl_return_t ret;
#ifdef FTGL_DEBUG
        assert(vec);
        assert(data);
        assert(count);
#endif /* FTGL_DEBUG */
        if (vec->capacity < (vec->size + count)) {
                if ((ret = ftgl_vector_reserve(vec, vec->size + count))
                    != FTGL_NO_ERROR)
                        return ret;
        }
        memmove((char *)(vec->items) + vec->size * vec->item_size, data,
                count * vec->item_size);
        vec->size += count;
}

void
ftgl_vector_sort(ftgl_vector_t *vec,
                 int (*cmp)(const void *, const void *))
{
#ifdef FTGL_DEBUG
        assert(vec);
        assert(vec->size);
#endif /* FTGL_DEBUG */
        qsort(vec->items, vec->size, vec->item_size, cmp);
}
