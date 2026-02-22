#include "test.h"

int run_tests( test_function *tests, int test_count ) {
    int passed_test_count = 0 ;
    int failed_test_count = 0 ;

    for ( int i = 0 ; i < test_count ; i++ ) {
        printf("Running test %d... ", i + 1 ) ;

        if( tests[ i ]() ) {
            printf( "\033[32mOK\033[0m\n" ) ;
            passed_test_count++ ;
        } else {
            printf( "\033[31mFAILED\033[0m\n" ) ;
            failed_test_count++ ;
        }
    }

    printf( "\n==============================\n" ) ;
    if( passed_test_count == test_count ) {
        printf("All tests pass!") ;
    } else {
        printf( "Tests passed:%d\n", passed_test_count ) ;
        printf( "Tests failed: %d\n", failed_test_count ) ;
    }

    return failed_test_count ;
}
