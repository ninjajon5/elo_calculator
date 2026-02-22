#include <stdio.h>
#include <string.h>
#include "utils/dict.h"
#include "utils/sarr.h"

void _load_data_lines( struct dict *data, FILE *data_file ) ;
void _load_data_line( struct dict *data, char *line, int *linecount ) ;
struct sarr _get_headers( char *path ) ;
struct sarr _divide_csv_line_into_strings( char *line ) ;

void print_data( char *path ) {
    FILE *input_file ;
    char buffer[ 100 ] ;

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
        data->keys = line_data ;
    } else {
        data->values = line_data ;
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
