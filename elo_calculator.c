#include <stdio.h>
#include <string.h>
#include <math.h>
#include "elo_calculator.h"
#include "utils/dict.h"
#include "utils/sarr.h"
#include "utils/IO.h"

int _elo_get_number_of_data_rows( struct elo_calculator *elo ) ;
struct elo_config _elo_config_init( float starting_elo, float diff_factor, float k ) ;
void _elo_update_elos( struct elo_calculator *elo, struct elo_config *config, int row_number ) ;
void _elo_get_row_data( struct elo_calculator *elo, struct elo_data_row *data_row, int row_number ) ;
void _elo_get_player_names_from_row( struct elo_calculator *elo, char *player_names[2], int row_number ) ;
void _elo_get_winner_from_row( struct elo_calculator *elo, char **winner, int row_number ) ;
void _elo_add_player_names_to_elos( struct elo_calculator *elo, struct elo_config *config, char *player_names[2] ) ;
void _elo_update_elos_from_data_row( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *row ) ;
void _elo_assign_results( float *p1_result, float *p2_result, struct elo_data_row *row ) ;
void _elo_assign_expected_results( float *p1_expected_result, float *p2_expected_result, float p1_elo, float p2_elo, struct elo_config *config ) ;

void elo_init( struct elo_calculator *elo ) {
    dict_init( &elo->data ) ;
    dict_init( &elo->elos ) ;
}

void elo_load_data( struct elo_calculator *elo, char *path ) {
    elo->data = load_data( path ) ;
}

void elo_calculate_from_data( struct elo_calculator *elo, float starting_elo, float diff_factor, float k ) {
    struct elo_config config = _elo_config_init( starting_elo, diff_factor, k ) ;
    int number_of_data_rows = _elo_get_number_of_data_rows( elo ) ;
    for( int i = 0 ; i < number_of_data_rows ; i++ ) {
        _elo_update_elos( elo, &config, i ) ;
    }
}

void elo_free( struct elo_calculator *elo ) {
    dict_free_with_nested_sarrs( &elo->data ) ;
    dict_free( &elo->elos ) ;
}

struct elo_config _elo_config_init( float starting_elo, float diff_factor, float k ) {
    struct elo_config config = {
        .starting_elo = starting_elo,
        .diff_factor = diff_factor,
        .k = k
    } ;
    return config ;
}

void _elo_update_elos( struct elo_calculator *elo, struct elo_config *config, int row_number ) {
    struct elo_data_row data_row ;
    _elo_get_row_data( elo, &data_row, row_number ) ;
    _elo_update_elos_from_data_row( elo, config, &data_row ) ;
}

void _elo_update_elos_from_data_row( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *row ) {    
    _elo_add_player_names_to_elos( elo, config, row->player_names ) ;

    float p1_result ;
    float p2_result ;
    _elo_assign_results( &p1_result, &p2_result, row ) ;

    float *p1_elo = (float*)dict_get( &elo->elos, row->player_names[0] ) ;
    float *p2_elo = (float*)dict_get( &elo->elos, row->player_names[1] ) ;

    float p1_expected_result ;
    float p2_expected_result ;
    _elo_assign_expected_results( &p1_expected_result, &p2_expected_result, *p1_elo, *p2_elo, config ) ;

    float p1_elo_change = config->k * ( p1_result - p1_expected_result ) ;
    float p2_elo_change = config->k * ( p2_result - p2_expected_result ) ;

    *p1_elo += p1_elo_change ;
    *p2_elo += p2_elo_change ;
}

void _elo_assign_results( float *p1_result, float *p2_result, struct elo_data_row *row ) {
    if( strcmp( row->player_names[0], row->winner ) == 0 ) {
        *p1_result += 1.0f ;
    } else {
        *p2_result += 1.0f ;
    }
}

void _elo_assign_expected_results( float *p1_expected_result, float *p2_expected_result, float p1_elo, float p2_elo, struct elo_config *config ) {
    float exponent = ( ( p2_elo - p1_elo ) / config->diff_factor ) ;
    float denominator = powf( 10.0f, exponent ) ;

    *p1_expected_result = ( 1.0f / ( 1.0f + denominator ) ) ;
    *p2_expected_result = 1.0f - *p1_expected_result ;
}

void _elo_get_row_data( struct elo_calculator *elo, struct elo_data_row *data_row, int row_number ) {
    data_row->row_number = row_number ;
    _elo_get_player_names_from_row( elo, data_row->player_names, row_number ) ;
    _elo_get_winner_from_row( elo, &data_row->winner, row_number ) ;
}

void _elo_get_player_names_from_row( struct elo_calculator *elo, char *player_names[2], int row_number ) {
    char *player_headers[2] = { "p1", "p2" } ;

    for( int i = 0 ; i < 2 ; i ++ ) {
        char *player_header = player_headers[i] ;
        struct sarr player_sarr = *(struct sarr*)dict_get( &elo->data, player_header ) ;
        player_names[i] = (char*)player_sarr.contents[row_number] ;
    }
}

void _elo_get_winner_from_row( struct elo_calculator *elo, char **winner, int row_number ) {
    struct sarr winner_sarr = *(struct sarr*)dict_get( &elo->data, "winner" ) ;
    *winner = (char*)winner_sarr.contents[row_number] ;
}

void _elo_add_player_names_to_elos( struct elo_calculator *elo, struct elo_config *config, char *player_names[2] ) {
    for( int i = 0 ; i < 2 ; i ++ ) {
        if( dict_has_key( &elo->elos, player_names[i] ) == 0 ) {
            dict_add( 
                &elo->elos, 
                player_names[i], &config->starting_elo,
                strlen( player_names[i] ) + 1, sizeof( config->starting_elo )
            ) ;
        }
    }
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
