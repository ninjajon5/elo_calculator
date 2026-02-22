#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define TASSERT( assertion, message ) \
do { \
    if ( !(assertion) ) { \
        fprintf( stderr, __FILE__ ":%d: assertion %s failed: %s\n", \
            __LINE__, #assertion, message ) ; \
        return 0 ; \
    } \
} while ( 0 )

typedef int ( *test_function )( void ) ;

int run_tests( test_function *tests, int test_count ) ;

#endif
