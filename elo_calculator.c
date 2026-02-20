#include <stdio.h>
#include "utils/dict.h"

void print_data( char *path ) {
    FILE *input_file ;
    char buffer[ 100 ] ;

    input_file = fopen( path, "r" ) ;
    fgets( buffer, sizeof( buffer ), input_file ) ;
    printf( "%s", buffer ) ;

    fclose( input_file ) ;
}

// struct dict calculate_elos( char *path ) {
//     struct dict data ;
//     struct dict elos ;
//     dict_init( &data ) ;
//     dict_init( &elos ) ;

//     FILE *data_file = fopen( path, "r" ) ;
//     char line[ 1024 ] ;
//     int linecount = 0 ;

//     while( fgets( line, sizeof line, data_file ) != NULL ) {
//         if( linecount == 0 ) {
//             // assign columns as database dict keys
//             printf( "line length: %d", strlen( line ) ) ;
//             for( int i = 0 ; i < strlen( line ) ; i++ ) {
//                 char *current_entry ;

//             }
//         } else {
//             // assign values to database dict
//         }
//     }

//     fclose( data_file ) ;
//     dict_free( &data ) ;

//     return elos ;
// }

// int main( void ) {
//     elo_print( "test/test_input.csv" ) ;
// }
