#include "../utils/test.h"
#include "../elo_calculator.h"
#include "../utils/dict.h"
#include "../utils/sarr.h"
#include "../utils/IO.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

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
    struct dict load_data( char *path ) ;
    struct dict data = load_data( "test_three_headers.csv" ) ;
    TASSERT(
        dict_has_key( &data, "p1" ),
        "Expected 'p1' as data dict key"
    ) ;

    dict_free_with_nested_sarrs( &data ) ;
    return 1 ;
}

int _6_test_load_data_dict_values_contains_data( void ) {
    struct dict load_data( char *path ) ;
    struct dict data = load_data( "test_three_headers.csv" ) ;

    struct sarr *p1_data = (struct sarr*)dict_get( &data, "p1" ) ;
    char *p1_datapoint_1 = (char*)p1_data->contents[0] ;

    TASSERT(
        strcmp( p1_datapoint_1, "11" ) == 0,
        "Expected 'p1' second datapoint to be '11'"
    ) ;

    dict_free_with_nested_sarrs( &data ) ;
    return 1 ;
}

int _7_test_load_data_dict_values_contain_sarr_of_datapoints( void ) {
    struct dict load_data( char *path ) ;
    struct dict data = load_data( "test_two_data_rows.csv" ) ;

    struct sarr *p1_data = (struct sarr*)dict_get( &data, "p1" ) ;
    char *p1_datapoint_2 = (char*)p1_data->contents[1] ;

    TASSERT(
        strcmp( p1_datapoint_2, "5" ) == 0,
        "Expected 'p1' second datapoint to be '5'"
    ) ;

    dict_free_with_nested_sarrs( &data ) ;
    return 1 ;
}

int _8_test_elo_init_returns_starting_elos( void ) {
    struct elo_calculator test_elo ;
    elo_init( &test_elo ) ;

    elo_load_data( &test_elo, "test_named_players.csv" ) ;
    elo_calculate_from_data( &test_elo, 1000.0f, 400.0f, 32.0f ) ;

    float name_A_elo = *(float*)dict_get( &test_elo.elos, "Name A" ) ;

    TASSERT(
        name_A_elo == 1016.0f,
        "Expected Name A elo to be 1016"
    ) ;

    elo_free( &test_elo ) ;
    return 1 ;
}

int _9_test_elo_init_returns_correct_elos( void ) {
    struct elo_calculator test_elo ;
    elo_init( &test_elo ) ;

    elo_load_data( &test_elo, "test_multiple_matches.csv" ) ;
    elo_calculate_from_data( &test_elo, 1000.0f, 400.0f, 32.0f ) ;

    float name_A_elo = *(float*)dict_get( &test_elo.elos, "Name A" ) ;
    float name_B_elo = *(float*)dict_get( &test_elo.elos, "Name B" ) ;
    float name_C_elo = *(float*)dict_get( &test_elo.elos, "Name C" ) ;
    float name_D_elo = *(float*)dict_get( &test_elo.elos, "Name D" ) ;

    TASSERT(
        roundf( name_A_elo ) == 1031.0f, 
        "Expected Name A elo to round to 1031"
    ) ;

    TASSERT(
        roundf( name_B_elo ) == 984.0f,
        "Expected Name B elo to round to 984"
    ) ;

    TASSERT(
        roundf( name_C_elo ) == 969.0f,
        "Expected Name C elo to round to 969"
    ) ;

    TASSERT(
        roundf( name_D_elo ) == 1016.0f,
        "Expected Name D elo to round to 1016"
    ) ;

    elo_free( &test_elo ) ;
    return 1 ;
}

int _10_test_winner_calculated_from_match_scores( void ) {
    void _elo_get_player_names_from_row( struct elo_calculator *elo, char *player_names[2], int row_number ) ;
    void _elo_get_winner_from_row( struct elo_calculator *elo, struct elo_data_row *row, int row_number ) ;
    
    struct elo_calculator test_elo ;
    elo_init( &test_elo ) ;

    elo_load_data( &test_elo, "all_headers_one_row.csv" ) ;

    struct elo_data_row test_data_row ;
    _elo_get_player_names_from_row( &test_elo, test_data_row.player_names, 0 ) ;
    _elo_get_winner_from_row( &test_elo, &test_data_row, 0 ) ;

    TASSERT(
        strcmp( test_data_row.winner, "Name A" ) == 0,
        "Expected winner to be Name A"
    ) ;

    elo_free( &test_elo ) ;
    return 1 ;
}

int _11_multiple_matches_with_all_headers_returns_correct_elos( void ) {
    struct elo_calculator test_elo ;
    elo_init( &test_elo ) ;

    elo_load_data( &test_elo, "all_headers_three_rows.csv" ) ;
    elo_calculate_from_data( &test_elo, 1000.0f, 400.0f, 32.0f ) ;

    float name_A_elo = *(float*)dict_get( &test_elo.elos, "Name A" ) ;
    float name_B_elo = *(float*)dict_get( &test_elo.elos, "Name B" ) ;
    float name_C_elo = *(float*)dict_get( &test_elo.elos, "Name C" ) ;
    float name_D_elo = *(float*)dict_get( &test_elo.elos, "Name D" ) ;

    TASSERT(
        roundf( name_A_elo ) == 1031.0f, 
        "Expected Name A elo to round to 1031"
    ) ;

    TASSERT(
        roundf( name_B_elo ) == 984.0f,
        "Expected Name B elo to round to 984"
    ) ;

    TASSERT(
        roundf( name_C_elo ) == 969.0f,
        "Expected Name C elo to round to 969"
    ) ;

    TASSERT(
        roundf( name_D_elo ) == 1016.0f,
        "Expected Name D elo to round to 1016"
    ) ;

    elo_free( &test_elo ) ;
    return 1 ;
}

int _12_straight_set_matches_return_boosted_elos( void ) {
    struct elo_calculator test_elo ;
    elo_init( &test_elo ) ;

    struct elo_config test_config = {
        .starting_elo = 1000.0f,
        .diff_factor = 400.0f,
        .k = 32.0f,
        .boost_threshold = 1.0f,
        .k_scaling = 1.25f
    } ;

    elo_load_data( &test_elo, "boosting.csv" ) ;
    elo_calculate_from_data( &test_elo, &test_config ) ;

    float name_C_elo = *(float*)dict_get( &test_elo.elos, "Name C" ) ;
    float name_D_elo = *(float*)dict_get( &test_elo.elos, "Name D" ) ;

    TASSERT(
        roundf( name_C_elo ) == 980.0f,
        "Expected Name C elo to round to 980"
    ) ;

    TASSERT(
        roundf( name_D_elo ) == 1020.0f,
        "Expected Name D elo to round to 1020"
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
    _8_test_elo_init_returns_starting_elos,
    _9_test_elo_init_returns_correct_elos,
    _10_test_winner_calculated_from_match_scores,
    _11_multiple_matches_with_all_headers_returns_correct_elos,
    _12_straight_set_matches_return_boosted_elos
} ;
int test_count = sizeof( tests ) / sizeof( tests[0] ) ;

int main( void ) {
    run_tests( tests, test_count ) ;
    // print_data( "test/test_three_headers.csv" ) ;
    return 0 ;
}
