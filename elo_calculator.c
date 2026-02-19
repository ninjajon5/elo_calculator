#include <stdio.h>

void elo_print( char *path ) {
    FILE *input_file ;
    char buffer[ 100 ] ;

    input_file = fopen( path, "r" ) ;
    fgets( buffer, sizeof( buffer ), input_file ) ;
    printf( "%s", buffer ) ;

    fclose( input_file ) ;
}

// int main( void ) {
//     elo_print( "test/test_input.csv" ) ;
// }
