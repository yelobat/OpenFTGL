#include "../include/log.h"

#ifdef FTGL_DEBUG
static char ftgl_log_stack[FTGL_LOG_STACK_CAPACITY][FTGL_LOG_MESSAGE_CAPACITY];
static int ftgl_log_stack_ptr;
static int ftgl_log_stack_size;

static int
ftgl_log_empty(void)
{
        return ftgl_log_stack_size == 0;
}

static int
ftgl_log_full(void)
{
        return ftgl_log_stack_size == FTGL_LOG_STACK_CAPACITY;
}
#endif /* FTGL_DEBUG */

extern void
ftgl_log_push_message(const char *msg, const char *file, const int line)
{
#ifdef FTGL_DEBUG
        int c, i;
        if (!ftgl_log_full())
                ftgl_log_stack_size++;
        for (i = 0; (c = msg[i]) != '\0' && i < FTGL_LOG_MESSAGE_CAPACITY - 1; i++)
                ftgl_log_stack[ftgl_log_stack_ptr][i] = c;
        ftgl_log_stack[ftgl_log_stack_ptr][i] = '\0';
        ftgl_log_stack_ptr = (ftgl_log_stack_ptr + 1) % FTGL_LOG_STACK_CAPACITY;
#else /* !defined(FTGL_DEBUG) */
        (void ) 0;
#endif /* FTGL_DEBUG */
}

extern const char *
ftgl_log_pop_message(void)
{
#ifdef FTGL_DEBUG
        if (!ftgl_log_empty()) {
                ftgl_log_stack_size--;
                ftgl_log_stack_ptr--;
                if (ftgl_log_stack_ptr < 0)
                        ftgl_log_stack_ptr += FTGL_LOG_STACK_CAPACITY;
                return (const char *) ftgl_log_stack[ftgl_log_stack_ptr];
        }
        return NULL;
#else /* !defined(FTGL_DEBUG) */
        return "Debugging disabled!";
#endif /* FTGL_DEBUG */
}
