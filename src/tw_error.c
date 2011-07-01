/*
 * tw_error.c
 *
 * This file contains the source code pertaining to the error handing functions
 * used in the ToyWrench application. When ever a piece of code in the ToyWrench
 * engine fails to successfully execute it should use the appropriate functions
 * found in this file to alert the engine that an error has occurred. It is
 * strongly encouraged that every function have the return type "int", and
 * return 0 if no errors have occurred and some other value if errors did occur.
 */

#include "tw_error.h"
#include <stdio.h>
#include <stdlib.h>

static tw_error_t *error_stack = NULL;
static tw_error_t *last_error = NULL;

/*
 * Returns the length of the given string.
 */
static int tw_strlen( const char *s ) {
    int l;
    l = 0;
    if( s == NULL ) {
        return 0;
    }
    while( s[l++] );
    return l;
}

/*
 * Returns a string that is a copy of the given string.
 */
static char * tw_strcpy( const char *s ) {
    int i;
    int len;
    char *cpy;
    len = tw_strlen(s);
    cpy = (char*)malloc(sizeof(char) * len);
    for( i = 0; i < len; i++ ) {
        cpy[i] = s[i];
    }
    return cpy;
}

/*
 * Prints the given warning messge to stderr.
 */
void push_warning( const char *s ) {
    fprintf(stderr, "WARNING: ");
    fprintf(stderr, "%s", s);
    fprintf(stderr, "\n");
}

/*
 * Pushes the given error string onto the error stack.
 */
void push_error( const char *s ) {
    tw_error_t *error;
    char *cpy;
    if( s == NULL ) {
        return;
    }
    cpy = tw_strcpy(s);
    error = (tw_error_t*)malloc(sizeof(tw_error_t));
    error->value = cpy;
    error->next = NULL;
    if( error_stack == NULL ) {
        error_stack = error;
        last_error = error;
    }
    else {
        last_error->next = (void*)error;
        last_error = error;
    }
}

/*
 * Clears the error stack, printing out all of the errors in the order that they
 * were pushed onto the stack.
 */
void dump_stack_trace() {
    tw_error_t *e;
    while( error_stack != NULL ) {
        fprintf(stderr, "ERROR: ");
        fprintf(stderr, "%s", error_stack->value);
        fprintf(stderr, "\n");
        free(error_stack->value);
        e = error_stack;
        error_stack = (tw_error_t*)(error_stack->next);
        free(e);
    }
}
