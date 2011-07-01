/*
 * tw_error.h
 */

#ifndef TWERROR
#define TWERROR

typedef struct {
    char *value;
    void *next;
} tw_error_t;

void push_warning( const char *s );

/*
 * Pushes the given error string onto the error stack.
 */
void push_error( const char *s );

/*
 * Clears the error stack, printing out all of the errors in the order that they
 * were pushed onto the stack.
 */
void dump_stack_trace();

#endif
