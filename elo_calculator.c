#include <stdio.h>
#include <string.h>
#include "utils/dict.h"
#include "utils/sarr.h"

struct sarr get_headers( char *path ) ;

void print_data( char *path ) {
    FILE *input_file ;
    char buffer[ 100 ] ;

    input_file = fopen( path, "r" ) ;
    fgets( buffer, sizeof( buffer ), input_file ) ;
    printf( "%s", buffer ) ;

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
    FILE *data_file ;
    char line[ 32 ] ;
    struct sarr headers ;
    sarr_init( &headers, 8 ) ;

    data_file = fopen( path, "r" ) ;
    fgets( line, sizeof( line ), data_file ) ;

    char *character = line ; // pointer to first character in line[]
    int header_index = 0 ;

    while( *character != '\0' ) {
        if( *character == ',' || *character == '\n' ) {
            *character = '\0' ; // replace ',' or '\n' with '\0'
            char *header = &line[ header_index ] ; // header is pointer to first character in this sections
            sarr_append( &headers, header, strlen( header ) + 1 ) ; // add header string to sarr
            header_index = ( character - line ) + 1 ; // move to one after the newly-created '\0'
        }
        character++ ;
    }

    fclose( data_file ) ;  
    return headers ;
}

// int main( void ) {
//     print_headers( "test/test_win_loss.csv" ) ;
// }
