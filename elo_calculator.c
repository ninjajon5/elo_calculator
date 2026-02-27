#include <stdio.h>
#include <string.h>
#include <math.h>
#include "elo_calculator.h"
#include "utils/dict.h"
#include "utils/sarr.h"

int _elo_get_number_of_data_rows( struct elo_calculator *elo ) ;
struct dict _load_data( char *path ) ;
void _load_data_lines( struct dict *data, FILE *data_file ) ;
void _load_data_line( struct dict *data, char *line, int *linecount ) ;
void _load_headers_into_keys( struct dict *data, struct sarr *line_data ) ;
void _append_datapoints_into_values( struct dict *data, struct sarr *line_data ) ;
struct sarr _get_headers( char *path ) ;
struct sarr _divide_csv_line_into_strings( char *line ) ;

void elo_init( struct elo_calculator *elo ) {
    dict_init( &elo->data ) ;
    dict_init( &elo->elos ) ;
}

void elo_load_data( struct elo_calculator *elo, char *path ) {
    elo->data = _load_data( path ) ;
}

void elo_calculate( struct elo_calculator *elo, float starting_elo ) {
    int number_of_data_rows = _elo_get_number_of_data_rows( elo ) ;
    for( int i = 0 ; i < number_of_data_rows ; i++ ) {
        struct sarr p1_sarr = *(struct sarr*)dict_get( &elo->data, "p1" ) ;
        char *p1_name = (char*)p1_sarr.contents[i] ;

        if( dict_has_key( &elo->elos, p1_name ) == 0 ) {
            dict_add( 
                &elo->elos, 
                p1_name, &starting_elo,
                strlen( p1_name ) + 1, sizeof( starting_elo )
            ) ;
        }

        struct sarr p2_sarr = *(struct sarr*)dict_get( &elo->data, "p2" ) ;
        char *p2_name = (char*)p2_sarr.contents[i] ;

        if( dict_has_key( &elo->elos, p2_name ) == 0 ) {
            dict_add( 
                &elo->elos, 
                p2_name, &starting_elo,
                strlen( p2_name ) + 1, sizeof( starting_elo )
            ) ;
        }

        float p1_result = 0.0f ;
        float p2_result = 0.0f ;

        struct sarr winner_sarr = *(struct sarr*)dict_get( &elo->data, "winner" ) ;
        char *winner = (char*)winner_sarr.contents[i] ;

        if( strcmp( p1_name, winner ) == 0 ) {
            p1_result += 1.0f ;
        } else {
            p2_result += 1.0f ;
        }

        float *p1_elo = (float*)dict_get( &elo->elos, p1_name ) ;
        float *p2_elo = (float*)dict_get( &elo->elos, p2_name ) ;

        float diff_factor = 400.0f ;
        float k = 32.0f ;

        float exponent = ( ( *p2_elo - *p1_elo ) / diff_factor ) ;
        float denominator = powf( 10.0f, exponent ) ;

        float p1_expected_result = ( 1.0f / ( 1.0f + denominator ) ) ;
        float p2_expected_result = 1.0f - p1_expected_result ;

        float p1_elo_change = k * ( p1_result - p1_expected_result ) ;
        float p2_elo_change = k * ( p2_result - p2_expected_result ) ;

        *p1_elo += p1_elo_change ;
        *p2_elo += p2_elo_change ;
    }
}

void elo_free( struct elo_calculator *elo ) {
    free_dict_with_nested_sarrs( &elo->data ) ;
    dict_free( &elo->elos ) ;
}

void print_data( char *path ) {
    FILE *input_file ;
    char buffer[ 256 ] ;

    input_file = fopen( path, "r" ) ;

    while( fgets( buffer, sizeof( buffer ), input_file ) != NULL ) {
        printf( "%s", buffer ) ;
    }

    fclose( input_file ) ;
}

void print_headers( char *path ) {
    struct sarr headers = _get_headers( path ) ;
    for( int i = 0 ; i < headers.len ; i++ ) {
        printf( "%s; ", (char*)headers.contents[i] ) ;
    }
    sarr_free( &headers ) ;
}

void free_dict_with_nested_sarrs( struct dict *dict ) {
    for( int i = 0 ; i < dict->values.len ; i++ ) {
        struct sarr *nested_sarr = (struct sarr*)dict->values.contents[i] ;
        sarr_free( nested_sarr ) ;
        free( nested_sarr ) ; // _load_headers_into_keys uses malloc()
    }
    dict_free( dict ) ;
}

int _elo_get_number_of_data_rows( struct elo_calculator *elo ) {
    struct sarr column_1 = *(struct sarr*)elo->data.values.contents[0] ;
    struct sarr column_2 = *(struct sarr*)elo->data.values.contents[1] ;

    int rows_in_column_1 = column_1.len ;
    int rows_in_column_2 = column_2.len ;

    if ( rows_in_column_1 != rows_in_column_2 ) {
        fprintf(stderr, "elo: data dict columns have different row counts\n") ;
        exit( 1 ) ;
    }

    return rows_in_column_1 ;
}

struct dict _load_data( char *path ) {
    struct dict data ;
    dict_init( &data ) ;

    FILE *data_file = fopen( path, "r" ) ;
    _load_data_lines( &data, data_file ) ;

    fclose( data_file ) ;
    return data ;
}

void _load_data_lines( struct dict *data, FILE *data_file ) {
    int linecount = 0 ;
    char line[ 256 ] ;
    while( fgets( line, sizeof( line ), data_file ) != NULL ) {
        _load_data_line( data, line, &linecount ) ;
    }
}

void _load_data_line( struct dict *data, char *line, int *linecount ) {
    struct sarr line_data = _divide_csv_line_into_strings( line ) ;
    if( (*linecount)++ == 0 ) { // increment linecount after check
        _load_headers_into_keys( data, &line_data ) ;
    } else {
        _append_datapoints_into_values( data, &line_data ) ;
    }
}

void _load_headers_into_keys( struct dict *data, struct sarr *line_data ) {
    for( int i = 0 ; i < line_data->len ; i++ ) {
        char *header = (char*)line_data->contents[i] ;

        struct sarr *empty_data = malloc( sizeof( struct sarr ) ) ; // on heap, so not overwritten by subsequent loops
        sarr_init( empty_data, 16 ) ;

        dict_add( // each header is added as a key paired to an empty sarr
            data,
            header, empty_data,
            strlen( header ) + 1, sizeof( struct sarr )
        ) ;
    }
}

void _append_datapoints_into_values( struct dict *data, struct sarr *line_data ) {
    if( line_data-> len != data->keys.len ) {
        fprintf( stderr, "line data has different element count to data dict keys" ) ;
        exit( 1 ) ;
    }

    for( int i = 0 ; i < data->values.len ; i++ ) { // never go beyond length of data.values
        char *datapoint = (char*)line_data->contents[i] ;
        struct sarr *data_sarr = (struct sarr*)data->values.contents[i] ;
        sarr_append( data_sarr, datapoint, strlen( datapoint ) + 1 ) ;
    }
}

struct sarr _get_headers( char *path ) {
    FILE *data_file = fopen( path, "r" ) ;

    char line[ 256 ] ;
    fgets( line, sizeof( line ), data_file ) ; // store first line in line[]
    struct sarr headers = _divide_csv_line_into_strings( line ) ;

    fclose( data_file ) ;  
    return headers ;
}

struct sarr _divide_csv_line_into_strings( char *line ) {
    struct sarr strings ;
    sarr_init( &strings, 8 ) ; // initialise sarr to store strings

    char *character = line ; // pointer to first character in line[]
    int string_index = 0 ; // tracker for index in line[] of start of strings

    while( *character != '\0' ) {
        if( *character == ',' || *character == '\n' ) {
            *character = '\0' ; // replace ',' or '\n' with '\0'
            char *string = &line[ string_index ] ; // string is pointer to first character in this section
            sarr_append( &strings, string, strlen( string ) + 1 ) ; // add string to sarr
            string_index = ( character - line ) + 1 ; // move to one after the newly-created '\0'
        }
        character++ ;
    }
 
    return strings ;
}

// int main( void ) {
//     print_headers( "test/test_win_loss.csv" ) ;
// }
