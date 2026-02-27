#include "../utils/test.h"
#include "../elo_calculator.h"
#include "../utils/dict.h"
#include "../utils/sarr.h"
#include <stdio.h>
#include <string.h>



int _1_test_print_prints_file_contents( void ) {
    FILE *original_stdout = stdout ;
    char temp_filename[] = "test_output.tmp" ;
    
    freopen( temp_filename, "w", stdout ) ; // write a file with temp_filename, and redirect stdout to it
    print_data( "test_single_input.csv" ) ;
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
    print_headers( "test_two_headers.csv" ) ;
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
    print_headers( "test_three_headers.csv" ) ;
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
    print_data( "test_three_headers.csv" ) ;
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
    struct dict data = _load_data( "test_three_headers.csv" ) ;
    TASSERT(
        dict_has_key( &data, "p1" ),
        "Expected 'p1' as data dict key"
    ) ;

    free_dict_with_nested_sarrs( &data ) ;
    return 1 ;
}

int _6_test_load_data_dict_values_contains_data( void ) {
    struct dict _load_data( char *path ) ;
    struct dict data = _load_data( "test_three_headers.csv" ) ;

    struct sarr *p1_data = (struct sarr*)dict_get( &data, "p1" ) ;
    char *p1_datapoint_1 = (char*)p1_data->contents[0] ;

    TASSERT(
        strcmp( p1_datapoint_1, "11" ) == 0,
        "Expected 'p1' second datapoint to be '11'"
    ) ;

    free_dict_with_nested_sarrs( &data ) ;
    return 1 ;
}

int _7_test_load_data_dict_values_contain_sarr_of_datapoints( void ) {
    struct dict _load_data( char *path ) ;
    struct dict data = _load_data( "test_two_data_rows.csv" ) ;

    struct sarr *p1_data = (struct sarr*)dict_get( &data, "p1" ) ;
    char *p1_datapoint_2 = (char*)p1_data->contents[1] ;

    TASSERT(
        strcmp( p1_datapoint_2, "5" ) == 0,
        "Expected 'p1' second datapoint to be '5'"
    ) ;

    free_dict_with_nested_sarrs( &data ) ;
    return 1 ;
}

int _8_test_elo_init_returns_starting_elos( void ) {
    struct elo_calculator test_elo ;
    elo_init( &test_elo ) ;

    elo_load_data( &test_elo, "test_named_players.csv" ) ;
    elo_calculate( &test_elo, 1000.0f ) ;

    float name_A_elo = *(float*)dict_get( &test_elo.elos, "Name A" ) ;

    TASSERT(
        name_A_elo >= 1000.0f,
        "Expected Name A elo to exceed starting elo"
    ) ;

    elo_free( &test_elo ) ;
    return 1 ;
}


test_function tests[] = {
    _1_test_print_prints_file_contents,
    _2_test_print_headers_returns_headers,
    _3_test_print_headers_returns_more_headers,
    _4_test_print_file_prints_all_lines,
    _5_test_load_data_dict_keys_contains_header,
    _6_test_load_data_dict_values_contains_data,
    _7_test_load_data_dict_values_contain_sarr_of_datapoints,
    _8_test_elo_init_returns_starting_elos
} ;
int test_count = sizeof( tests ) / sizeof( tests[0] ) ;

int main( void ) {
    run_tests( tests, test_count ) ;
    // print_data( "test/test_three_headers.csv" ) ;
    return 0 ;
}
