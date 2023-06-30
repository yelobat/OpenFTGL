#include "../include/vector.h"
#include <stdio.h>

int
cmp(const void *a, const void *b)
{
        const char *a_ = (const char *) a;
        const char *b_ = (const char *) b;
        return strcmp(a_, b_);
}


int main(int argc, char **argv)
{
        char *words[5] = {
                "hello world",
                "we live in a society",
                "random sentence",
                "holy shit, a string",
                "and this is something else"
        };
        ftgl_vector_t *vec = ftgl_vector_create(sizeof(*words));
        ftgl_vector_push_back_data(vec, words, 5);
        ftgl_vector_pop_back(vec);
        ftgl_vector_sort(vec, cmp);
        for (int i = 0; i < ftgl_vector_size(vec); i++) {
                printf("%s ", *(char **)ftgl_vector_get(vec, i));
        }
        printf("\n");
        ftgl_vector_shrink(vec);
        ftgl_vector_free(vec);
        return 0;
}
