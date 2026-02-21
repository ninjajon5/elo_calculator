#include <stdio.h>
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

    data_file = fopen( path, "r" ) ;
    fgets( line, sizeof( line ), data_file ) ;

    char *character = &line[0] ;
    while( *character != '\0' ) {
        if( *character == ',' || *character == '\n' ) {
            *character = '\0' ;
        }
        character++ ;
    }

    char *header_1 = &line[0] ;
    char *header_2 = &line[3] ;
    printf( "%s; %s", header_1, header_2 ) ;

    fclose( data_file ) ;
}

// int main( void ) {
//     print_headers( "test/test_win_loss.csv" ) ;
// }
