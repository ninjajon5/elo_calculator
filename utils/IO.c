#include <stdio.h>
#include <string.h>
#include "sarr.h"
#include "dict.h"

void _load_data_lines( struct dict *data, FILE *data_file ) ;
void _load_data_line( struct dict *data, char *line, int *linecount ) ;
void _load_headers_into_keys( struct dict *data, struct sarr *line_data ) ;
void _append_datapoints_into_values( struct dict *data, struct sarr *line_data ) ;
struct sarr _get_headers( char *path ) ;
struct sarr _divide_csv_line_into_strings( char *line ) ;

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

struct dict load_data( char *path ) {
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
