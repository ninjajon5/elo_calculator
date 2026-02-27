#include <stdio.h>
#include <string.h>
#include <math.h>
#include "elo_calculator.h"
#include "utils/dict.h"
#include "utils/sarr.h"
#include "utils/IO.h"

int _elo_get_number_of_data_rows( struct elo_calculator *elo ) ;

void elo_init( struct elo_calculator *elo ) {
    dict_init( &elo->data ) ;
    dict_init( &elo->elos ) ;
}

void elo_load_data( struct elo_calculator *elo, char *path ) {
    elo->data = load_data( path ) ;
}

void elo_calculate_from_data( struct elo_calculator *elo, float starting_elo, float diff_factor, float k ) {
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
    dict_free_with_nested_sarrs( &elo->data ) ;
    dict_free( &elo->elos ) ;
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
