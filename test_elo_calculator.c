#include "test/test.h"
#include "elo_calculator.h"
#include "utils/dict.h"
#include <stdio.h>
#include <string.h>


int _1_test_print_prints_file_contents( void ) {
    FILE *original_stdout = stdout ;
    char temp_filename[] = "test_output.tmp" ;
    
    freopen( temp_filename, "w", stdout ) ; // write a file with temp_filename, and redirect stdout to it
    print_data( "test/test_single_input.csv" ) ;
    fflush( stdout ) ; // forces the buffer to output right now
    freopen( "CON", "w", stdout ) ; // redirect stdout to console

    FILE *read_file = fopen( temp_filename, "r" ) ;
    char buffer[ 100 ] ;
    fgets( buffer, sizeof(buffer), read_file ) ;
    fclose( read_file ) ;

    remove( temp_filename ) ;

    TASSERT( 
        strcmp( buffer, "test input\n" ) == 0,
        "Expected to print 'test_input'"
    ) ;

    return 1 ;
}

int _2_test_print_headers_returns_headers( void ) {
    FILE *original_stdout = stdout ;
    char temp_filename[] = "test_output.tmp" ;
    
    freopen( temp_filename, "w", stdout ) ; // write a file with temp_filename, and redirect stdout to it
    print_headers( "test/test_two_headers.csv" ) ;
    fflush( stdout ) ; // forces the buffer to output right now
    freopen( "CON", "w", stdout ) ; // redirect stdout to console

    FILE *read_file = fopen( temp_filename, "r" ) ;
    char buffer[ 100 ] ;
    fgets( buffer, sizeof(buffer), read_file ) ;
    fclose( read_file ) ;

    remove( temp_filename ) ;

    TASSERT( 
        strcmp( buffer, "p1; p2; " ) == 0,
        "Expected to print 'p1; p2; '"
    ) ;

    return 1 ;
}

int _3_test_print_headers_returns_more_headers( void ) {
    FILE *original_stdout = stdout ;
    char temp_filename[] = "test_output.tmp" ;
    
    freopen( temp_filename, "w", stdout ) ; // write a file with temp_filename, and redirect stdout to it
    print_headers( "test/test_three_headers.csv" ) ;
    fflush( stdout ) ; // forces the buffer to output right now
    freopen( "CON", "w", stdout ) ; // redirect stdout to console

    FILE *read_file = fopen( temp_filename, "r" ) ;
    char buffer[ 100 ] ;
    fgets( buffer, sizeof(buffer), read_file ) ;
    fclose( read_file ) ;

    remove( temp_filename ) ;

    TASSERT( 
        strcmp( buffer, "p1; player2; result; " ) == 0,
        "Expected to print 'p1; player2; result; '"
    ) ;

    return 1 ;
}

int _4_test_print_file_prints_all_lines( void ) {
    FILE *original_stdout = stdout ;
    char temp_filename[] = "test_output.tmp" ;
    
    freopen( temp_filename, "w", stdout ) ; // write a file with temp_filename, and redirect stdout to it
    print_data( "test/test_three_headers.csv" ) ;
    fflush( stdout ) ; // forces the buffer to output right now
    freopen( "CON", "w", stdout ) ; // redirect stdout to console

    FILE *read_file = fopen( temp_filename, "r" ) ;
    char buffer[ 100 ] ;
    char full_output[ 256 ] ;

    while( fgets( buffer, sizeof(buffer), read_file ) != NULL ) {
        strcat( full_output, buffer ) ;
    }
    
    fclose( read_file ) ;

    remove( temp_filename ) ;

    char expected_output[] = "p1,player2,result\n11,5,p1\n" ;
    TASSERT( 
        strcmp( full_output, expected_output ) == 0,
        "Expected to print 'p1,player2,result\n11,5,p1\n'"
    ) ;

    return 1 ;
}

int _5_test_load_data_dict_keys_contains_header( void ) {
    struct dict _load_data( char *path ) ;
    struct dict data = _load_data( "test/test_three_headers.csv" ) ;
    TASSERT(
        dict_has_key( &data, "p1" ),
        "Expected 'p1' as data dict key"
    ) ;
    return 1 ;
}

int _6_test_load_data_dict_values_contains_data( void ) {
    struct dict _load_data( char *path ) ;
    struct dict data = _load_data( "test/test_three_headers.csv" ) ;
    TASSERT(
        *(int*)dict_get( &data, "p1" ) == 11,
        "Expected 'p1' data to be 11"
    ) ;
    return 1 ;
}


test_function tests[] = {
    _1_test_print_prints_file_contents,
    _2_test_print_headers_returns_headers,
    _3_test_print_headers_returns_more_headers,
    _4_test_print_file_prints_all_lines,
    _5_test_load_data_dict_keys_contains_header,
    _6_test_load_data_dict_values_contains_data
} ;
int test_count = sizeof( tests ) / sizeof( tests[0] ) ;

int main( void ) {
    run_tests( tests, test_count ) ;
    // print_data( "test/test_three_headers.csv" ) ;
    return 0 ;
}
