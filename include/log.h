#ifndef FTGL_LOG_H_
#define FTGL_LOG_H_

#include <stddef.h>

typedef enum ftgl_return_t {
        FTGL_NO_ERROR = 0,
        FTGL_MEMORY_ERROR,
} ftgl_return_t;

#define FTGL_DEBUG

#ifdef FTGL_DEBUG
#include <assert.h>
#endif /* FTGL_DEBUG */

#define FTGL_LOG_MESSAGE_CAPACITY 256
#define FTGL_LOG_STACK_CAPACITY 20

#define FTGL_LOG_MESSAGE(msg) \
        ftgl_log_push_message(msg, __FILE__, __LINE__)

extern void
ftgl_log_push_message(const char *msg, const char *file, const int line);
extern const char *
ftgl_log_pop_message(void);

#endif /* FTGL_LOG_H_ */
