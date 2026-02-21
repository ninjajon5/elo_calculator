#include <stdio.h>
#include <string.h>
#include "utils/dict.h"
#include "utils/sarr.h"

void print_data( char *path ) {
    FILE *input_file ;
    char buffer[ 100 ] ;

    input_file = fopen( path, "r" ) ;
    fgets( buffer, sizeof( buffer ), input_file ) ;
    printf( "%s", buffer ) ;

    fclose( input_file ) ;
}

void print_headers( char *path ) {
    FILE *data_file ;
    char line[ 32 ] ;
    struct sarr headers ;
    sarr_init( &headers, 8 ) ;

    data_file = fopen( path, "r" ) ;
    fgets( line, sizeof( line ), data_file ) ;

    char *character = &line[0] ;
    int header_index = 0 ;
    int current_index = 0;

    while( *character != '\0' ) {
        if( *character == ',' || *character == '\n' ) {
            *character = '\0' ;
            char *header = &line[ header_index ] ;
            sarr_append( &headers, header, strlen( header ) + 1 ) ;
            header_index = current_index + 1 ;
        }
        current_index++ ;
        character++ ;
    }

    for( int i = 0 ; i < headers.len ; i++ ) {
        printf( "%s; ", (char*)headers.contents[i] ) ;
    }

    fclose( data_file ) ;
}

// int main( void ) {
//     print_headers( "test/test_win_loss.csv" ) ;
// }
