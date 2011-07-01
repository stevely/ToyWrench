/*
 * tw_directory.c
 *
 * This file contains the source code pertaining to the directory handling
 * functions used in the ToyWrench application. The main purpose of this group
 * of functions is to facilitate the mandated directory structure used in
 * grouping games and their related resources. Any time in the ToyWrench engine
 * a file needs to be opened its path should be retrieved using one of the
 * directory handler functions to ensure portability and consistency.
 *
 * NOTE: The current implmentation of the directory handling functions suffers
 * from a number of weaknesses, notably:
 *
 * 1) Not being portable outside Linux/OS X
 * 2) Assumes the executable is located in the default directory
 * 3) Will not work through symbolic links
 *
 * #1 is ultimately unavoidable as other operating systems ultimately handle
 * directories (or whatever substitute is provided) in different manners.
 * In the future, a check for Microsoft Windows will have to be made, and
 * separate code will have to be used to handle directories.
 */

#include "tw_directory.h"

static char *executedpath;
static int initialized = 0;

/*
 * Returns the length of the given string.
 * This function differs from the standard strlen() function due to the fact
 * that the returned length includes the trailing '\0' as part of the string.
 */
static int qstrlen( const char *s ) {
    int l = 0;
    if( s == NULL ) {
        return 0;
    }
    while( s[l++] );
    return l;
}

/*
 * Returns a string representing the executed directory, retrieved by extracting
 * the relevant information from the given current working directory and the
 * executed path used to start the ToyWrench application.
 */
static char * get_executed_directory( const char *cwd, const char *exp ) {
    int i,j;
    if( cwd == NULL || exp == NULL ) {
        return NULL;
    }
    char *result = (char*)malloc(qstrlen(cwd)+qstrlen(exp));
    if( exp[0] == '/' ) {
        for( i = 0, j = qstrlen(exp)-1; i < qstrlen(exp); i++ ) {
            result[i] = exp[i];
            if( result[i] == '/' ) {
                j = i;
            }
        }
        result[j] = '\0';
        return result;
    }
    for( i = 0; i < qstrlen(cwd); i++ ) {
        result[i] = cwd[i];
    }
    result[i-1] = '/';
    for( j = 0; j < qstrlen(exp); j++ ) {
        result[i+j] = exp[j];
    }
    for( i = 0, j = qstrlen(result)-1; i < qstrlen(result); i++ ) {
        if( result[i] == '/' ) {
            j = i;
        }
    }
    result[j] = '\0';
    return result;
}

/*
 * Initializes the directory handler interface.
 */
int directory_init( const char *path ) {
    if( initialized ) {
        push_warning("Directory interface already initialized!");
        return 0;
    }
    executedpath = get_executed_directory(getcwd(NULL,0),path);
    /* TODO: change directory to the new path, set up available games */
    initialized = 1;
    return 0;
}
