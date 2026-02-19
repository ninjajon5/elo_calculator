#include "../utils/test.h"
#include <stdio.h>
#include <string.h>


int test_print_prints_file_contents( void ) {
    FILE *original_stdout = stdout ;
    FILE *temp_file = tmpfile() ;

    stdout = temp_file ;
    elo_print( "test/test_input.csv" ) ;
    stdout = original_stdout ;

    rewind( temp_file ) ; // Go to start of file
    char buffer[ 100 ] ;
    fgets( buffer, sizeof(buffer), temp_file ) ;
    fclose( temp_file ) ;

    TASSERT( 
        strcmp( buffer, "test input\n" ) == 0,
        "Expected to print 'test_input'"
    ) ;

    return 1 ;
}



test_function tests[] = {
} ;
int test_count = sizeof( tests ) / sizeof( tests[0] ) ;

int main( void ) {
    run_tests( tests, test_count ) ;
    return 0 ;
}
