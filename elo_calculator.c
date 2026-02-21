#include <stdio.h>
#include <string.h>
#include "utils/dict.h"
#include "utils/sarr.h"

struct sarr get_headers( char *path ) ;
struct sarr _divide_csv_into_headers( FILE *csv ) ;

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
    struct sarr headers = get_headers( path ) ;
    for( int i = 0 ; i < headers.len ; i++ ) {
        printf( "%s; ", (char*)headers.contents[i] ) ;
    }
    sarr_free( &headers ) ;
}

struct sarr get_headers( char *path ) {
    FILE *data_file = fopen( path, "r" ) ;
    struct sarr headers = _divide_csv_into_headers( data_file ) ;
    fclose( data_file ) ;  
    return headers ;
}

struct sarr _divide_csv_into_headers( FILE *csv ) {
    char line[ 32 ] ;
    fgets( line, sizeof( line ), csv ) ; // store first line in line[]

    struct sarr headers ;
    sarr_init( &headers, 8 ) ; // initialise sarr to store header strings

    char *character = line ; // pointer to first character in line[]
    int header_index = 0 ; // tracker for index in line[] of start of header strings

    while( *character != '\0' ) {
        if( *character == ',' || *character == '\n' ) {
            *character = '\0' ; // replace ',' or '\n' with '\0'
            char *header = &line[ header_index ] ; // header is pointer to first character in this sections
            sarr_append( &headers, header, strlen( header ) + 1 ) ; // add header string to sarr
            header_index = ( character - line ) + 1 ; // move to one after the newly-created '\0'
        }
        character++ ;
    }
 
    return headers ;
}

// int main( void ) {
//     print_headers( "test/test_win_loss.csv" ) ;
// }
