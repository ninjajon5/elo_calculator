#include <stdio.h>
#include <string.h>
#include "IO.h"
#include "sarr.h"
#include "dict.h"

void _write_data_rows( struct dict *data, FILE *file, int number_of_rows, void (*_write_row)( struct dict*, FILE*, int ) ) ;
void _write_headers( struct dict *data, FILE *file ) ;
void _write_float_dict_row( struct dict *data, FILE *file, int row_number ) ;
void _write_data_dict_row( struct dict *data, FILE *file, int row_number ) ;
void _write_data_point( struct dict *data, FILE *file, char *data_point, int column_number ) ;
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

void write_float_dict( struct dict *data, char *path ) {
    FILE *file = fopen( path, "w" ) ;
    if( file == NULL ) {
        fprintf( stderr, "IO: failed to write file" ) ;
        exit( 1 ) ;
    }

    int number_of_rows = data->keys.len ;
    _write_data_rows( data, file, number_of_rows, _write_float_dict_row ) ;

    fclose( file ) ;
}

void write_data_dict( struct dict *data, char *path ) {
    FILE *file = fopen( path, "w" ) ;
    if( file == NULL ) {
        fprintf( stderr, "IO: failed to write file" ) ;
        exit( 1 ) ;
    }

    int number_of_rows = get_number_of_data_rows( data ) ;
    _write_headers( data, file ) ;
    _write_data_rows( data, file, number_of_rows, _write_data_dict_row ) ;

    fclose( file ) ;
}

int get_number_of_data_rows( struct dict *data ) {
    struct sarr column_1 = *(struct sarr*)data->values.contents[0] ;
    struct sarr column_2 = *(struct sarr*)data->values.contents[1] ;

    int rows_in_column_1 = column_1.len ;
    int rows_in_column_2 = column_2.len ;

    if ( rows_in_column_1 != rows_in_column_2 ) {
        fprintf(stderr, "IO: data dict columns have different row counts\n") ;
        exit( 1 ) ;
    }

    return rows_in_column_1 ;
}

void _write_data_rows( struct dict *data, FILE *file, int number_of_rows, void (*_write_row)( struct dict*, FILE*, int ) ) {
    dict_validate( data ) ;
    for( int row_number = 0 ; row_number < number_of_rows ; row_number++ ) {
        _write_row( data, file, row_number ) ;
    }
}

void _write_headers( struct dict *data, FILE *file ) {
    for( int column_number = 0 ; column_number < data->keys.len ; column_number++ ) {
        char *value = data->keys.contents[ column_number ] ;
        _write_data_point( data, file, value, column_number ) ;
    }
}

void _write_float_dict_row( struct dict *data, FILE *file, int row_number ) {
    char *header = data->keys.contents[ row_number ] ;
    float *value = data->values.contents[ row_number ] ;
    char buffer[ 64 ] ;
    snprintf( 
        buffer, sizeof( buffer ),
        "%s,%.2f\n", header, *value
    ) ;
    
    fputs( buffer, file ) ;
}

void _write_data_dict_row( struct dict *data, FILE *file, int row_number ) {
    for( int column_number = 0 ; column_number < data->keys.len ; column_number++ ) {
        struct sarr *column = data->values.contents[ column_number ] ;
        char *value = column->contents[ row_number ] ;
        _write_data_point( data, file, value, column_number ) ;
    }
}

void _write_data_point( struct dict *data, FILE *file, char *data_point, int column_number ) {
    fputs( data_point, file ) ;
    if( column_number == ( data->keys.len - 1 ) ) {
        fputs( "\n", file ) ; // final value only gets newline instead of comma
    } else {
        fputs( ",", file ) ;
    }
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

        free( empty_data ) ; // has been memcpy'd by sarr_append(), so can be freed
    }
}

void _append_datapoints_into_values( struct dict *data, struct sarr *line_data ) {
    if( line_data-> len != data->keys.len ) {
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
