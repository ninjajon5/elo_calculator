#include <stdio.h>
#include <string.h>
#include <math.h>
#include "elo_calculator.h"
#include "utils/dict.h"
#include "utils/sarr.h"
#include "utils/IO.h"

int _elo_get_number_of_data_rows( struct elo_calculator *elo ) ;
struct elo_config elo_config_default( void ) ;
void _elo_update_elos( struct elo_calculator *elo, struct elo_config *config, int row_number ) ;
void _elo_get_row_data( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *data_row, int row_number ) ;
void _elo_sort( struct elo_calculator *elo, int number_of_data_rows, int *ordered_row_indexes ) ;
void _elo_get_date_sorted_row_indexes( struct elo_calculator *elo, int number_of_data_rows, int *ordered_row_indexes ) ;
int _elo_compare_DDMMYYYY_dates( char *date, char *base_date ) ;
int _elo_compare_dates_by_offset( char *date, char *base_date, int offset, int end ) ;
void _elo_add_player_names_and_within_boost_threshold_to_row( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *row, int row_number ) ;
void _elo_add_player_names_to_row( struct elo_calculator *elo, struct elo_data_row *row, int row_number, char *player_headers[2], char *player_names[2], struct sarr player_sarrs[2] ) ;
void _elo_add_within_boost_threshold_to_row( struct elo_config *config, struct elo_data_row *row, char *player_names[2], struct sarr player_sarrs[2], int row_number ) ;
bool _elo_check_if_within_boost_threshold( struct elo_config *config, char *player_name, struct sarr *player_sarr, int row_number ) ;
void _elo_add_winner_and_straight_sets_to_row( struct elo_calculator *elo, struct elo_data_row *row, int row_number ) ;
char* _elo_get_winner_from_winner_column( struct elo_calculator *elo, int row_number ) ;
char* _elo_get_winner_from_match_counts( struct elo_data_row *row ) ;
bool _elo_get_straight_sets_from_match_counts( struct elo_data_row *row ) ;
void _elo_calculate_match_counts( struct elo_calculator *elo, int row_number, int *p1_match_count, int *p2_match_count ) ;
int _elo_calculate_player_score( struct elo_calculator *elo, int row_number, int player_number, int game_number ) ;
void _elo_add_player_names_to_elos( struct elo_calculator *elo, struct elo_config *config, char *player_names[2] ) ;
void _elo_update_elos_from_data_row( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *row ) ;
void _elo_assign_results( struct elo_data_row *row ) ;
void _elo_assign_expected_results( struct elo_config *config, struct elo_data_row *row ) ;
void _elo_calculate_elo_change( struct elo_config *config, struct elo_data_row *row ) ;
float _elo_calculate_k( struct elo_config *config, struct elo_data_row *row ) ;
float _elo_update_k_values( float *k, struct elo_config *config, struct elo_data_row *row ) ;

void elo_init( struct elo_calculator *elo ) {
    dict_init( &elo->data ) ;
    dict_init( &elo->elos ) ;
}

struct elo_config elo_config_default( void ) {
    struct elo_config config = { 0 } ;
    return config ;
}

void elo_load_data( struct elo_calculator *elo, char *path ) {
    elo->data = load_data( path ) ;
}

void elo_calculate_from_data( struct elo_calculator *elo, struct elo_config *config ) {
    int number_of_data_rows = _elo_get_number_of_data_rows( elo ) ;
    int ordered_row_indexes[ 2048 ] ;
    _elo_sort( elo, number_of_data_rows, ordered_row_indexes ) ;
    for( int i = 0 ; i < number_of_data_rows ; i++ ) {
        _elo_update_elos( elo, config, ordered_row_indexes[i] ) ;
    }
}

void elo_write_elos( struct elo_calculator *elo, char *path ) {
    write_float_dict( &elo->elos, path ) ;
}

void elo_free( struct elo_calculator *elo ) {
    dict_free_with_nested_sarrs( &elo->data ) ;
    dict_free( &elo->elos ) ;
}

void _elo_update_elos( struct elo_calculator *elo, struct elo_config *config, int row_number ) {
    struct elo_data_row data_row ;
    _elo_get_row_data( elo, config, &data_row, row_number ) ;
    _elo_update_elos_from_data_row( elo, config, &data_row ) ;
}

void _elo_get_row_data( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *data_row, int row_number ) {
    data_row->row_number = row_number ;
    _elo_add_player_names_and_within_boost_threshold_to_row( elo, config, data_row, row_number ) ;
    _elo_add_winner_and_straight_sets_to_row( elo, data_row, row_number ) ;
}

void _elo_sort( struct elo_calculator *elo, int number_of_data_rows, int *ordered_row_indexes ) {
    if( dict_has_key( &elo->data, "date" ) ) {
        _elo_get_date_sorted_row_indexes( elo, number_of_data_rows, ordered_row_indexes ) ;
    } else {
        for( int i = 0 ; i < number_of_data_rows ; i++ ) {
            ordered_row_indexes[i] = i ;
        }
    }
}

void _elo_get_date_sorted_row_indexes( struct elo_calculator *elo, int number_of_data_rows, int *ordered_row_indexes ) {
    struct sarr *dates = dict_get( &elo->data, "date" ) ;
    ordered_row_indexes[0] = 0 ;
    for( int i = 1 ; i < number_of_data_rows ; i++ ) {
        char *row_date = dates->contents[ i ] ;
        int target_index = i - 1 ;
        while( target_index >= 0 ) {
            char *target_row_date = dates->contents[ ordered_row_indexes[ target_index ] ] ;
            if( _elo_compare_DDMMYYYY_dates( row_date, target_row_date ) >= 0 ) {
                break ;
            } else {
                ordered_row_indexes[ target_index + 1 ] = ordered_row_indexes[ target_index ] ;
                target_index -= 1 ;
            }
        }
        ordered_row_indexes[ target_index + 1 ] = i ;
    }
}

int _elo_compare_DDMMYYYY_dates( char *date, char *base_date ) {
    // DD/MM/YYYY
    // 0123456789
    int year_offset = 6 ;
    int year_end = 9 ;
    int year_comparison = _elo_compare_dates_by_offset( date, base_date, year_offset, year_end ) ;
    if( year_comparison > 0 ) {
        return 1 ;
    } else if( year_comparison < 0 ) {
        return -1 ;
    }

    int month_offset = 3 ;
    int month_end = 4 ;
    int month_comparison = _elo_compare_dates_by_offset( date, base_date, month_offset, month_end ) ;
    if( month_comparison > 0 ) {
        return 1 ;
    } else if( month_comparison < 0 ) {
        return -1 ;
    }

    int day_offset = 0 ;
    int day_end = 1 ;
    int day_comparison = _elo_compare_dates_by_offset( date, base_date, day_offset, day_end ) ;
    if( day_comparison > 0 ) {
        return 1 ;
    } else if( day_comparison < 0 ) {
        return -1 ;
    } else {
        return 0 ;
    }
}

int _elo_compare_dates_by_offset( char *input_date, char *input_base_date, int offset, int end ) {
    char date[ 256 ] ;
    char base_date[ 256 ] ;
    strcpy( date, input_date ) ;
    strcpy( base_date, input_base_date ) ;
    
    char *date_section_string = date + offset ;
    *( date + end + 1 ) = '\0' ;
    int date_section = atoi( date_section_string ) ;

    char *base_date_section_string = base_date + offset ;
    int base_date_section = atoi( base_date_section_string ) ;

    if( date_section > base_date_section ) {
        return 1 ;
    } else if ( date_section < base_date_section ) {
        return - 1 ;
    } else {
        return 0 ;
    }
}

void _elo_update_elos_from_data_row( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *row ) {    
    _elo_add_player_names_to_elos( elo, config, row->player_names ) ;
    
    float *p1_elo = (float*)dict_get( &elo->elos, row->player_names[0] ) ;
    float *p2_elo = (float*)dict_get( &elo->elos, row->player_names[1] ) ;
    
    row->player_elos[0] = *p1_elo ;
    row->player_elos[1] = *p2_elo ;

    _elo_assign_results( row ) ;
    _elo_assign_expected_results( config, row ) ;
    _elo_calculate_elo_change( config, row ) ;

    *p1_elo += row->player_elo_changes[0] ;
    *p2_elo += row->player_elo_changes[1] ;
}

void _elo_assign_results( struct elo_data_row *row ) {
    row->player_results[0] = 0.0f ;
    row->player_results[1] = 0.0f ;

    if( strcmp( row->player_names[0], row->winner ) == 0 ) {
        row->player_results[0] += 1.0f ;
    } else {
        row->player_results[1] += 1.0f ;
    }
}

void _elo_assign_expected_results( struct elo_config *config, struct elo_data_row *row ) {
    float exponent = ( ( row->player_elos[1] - row->player_elos[0] ) / config->diff_factor ) ;
    float denominator = powf( 10.0f, exponent ) ;

    row->player_expected_results[0] = ( 1.0f / ( 1.0f + denominator ) ) ;
    row->player_expected_results[1] = 1.0f - row->player_expected_results[0] ;
}

void _elo_calculate_elo_change( struct elo_config *config, struct elo_data_row *row ) {
    float k[2] = { config->k, config->k } ;
    _elo_update_k_values( k, config, row ) ;
    row->player_elo_changes[0] = k[0] * ( row->player_results[0] - row->player_expected_results[0] ) ;
    row->player_elo_changes[1] = k[1] * ( row->player_results[1] - row->player_expected_results[1] ) ;
}

float _elo_update_k_values( float *k, struct elo_config *config, struct elo_data_row *row ) {
    for( int i = 0 ; i < 2 ; i++ ) {
        if( row->straight_sets ) {
            k[i] *= config->k_scaling ;
        }
        if( row->player_within_boost_threshold[i] ) {
            k[i] *= config->k_scaling ;
        }
    }
}

void _elo_add_player_names_and_within_boost_threshold_to_row( struct elo_calculator *elo, struct elo_config *config, struct elo_data_row *row, int row_number ) {
    char *player_headers[2] = { "player1", "player2" } ;
    char *player_names[2] ;
    struct sarr player_sarrs[2] ;

    _elo_add_player_names_to_row( elo, row, row_number, player_headers, player_names, player_sarrs ) ;
    _elo_add_within_boost_threshold_to_row( config, row, player_names, player_sarrs, row_number ) ;
}

void _elo_add_player_names_to_row( struct elo_calculator *elo, struct elo_data_row *row, int row_number, char *player_headers[2], char *player_names[2], struct sarr player_sarrs[2] ) {
    for( int i = 0 ; i < 2 ; i++ ) {
        char *player_header = player_headers[i] ;
        player_sarrs[i] = *(struct sarr*)dict_get( &elo->data, player_headers[i] ) ;
        player_names[i] = (char*)player_sarrs[i].contents[ row_number ] ;
        row->player_names[i] = player_names[i] ;
    }
}

void _elo_add_within_boost_threshold_to_row( struct elo_config *config, struct elo_data_row *row, char *player_names[2], struct sarr player_sarrs[2], int row_number ) {
    for( int i = 0 ; i < 2 ; i++ ) {
        row->player_within_boost_threshold[i] = _elo_check_if_within_boost_threshold( config, player_names[i], player_sarrs, row_number ) ;
    }
}

bool _elo_check_if_within_boost_threshold( struct elo_config *config, char *player_name, struct sarr *player_sarrs, int row_number ) {
    int player_match_count = 0 ;

    for( int i = 0 ; i <= row_number ; i++ ) {
        char *player1_name = player_sarrs[0].contents[i] ;
        char *player2_name = player_sarrs[1].contents[i] ;
        if( 
            strcmp( player_name, player1_name ) == 0 ||
            strcmp( player_name, player2_name ) == 0
        ) {
            player_match_count += 1 ;
        }
    }

    if( player_match_count > config->boost_threshold ) {
        return false ;
    } else {
    return true ;
    }
}

void _elo_add_winner_and_straight_sets_to_row( struct elo_calculator *elo, struct elo_data_row *row, int row_number ) {
    if( dict_has_key( &elo->data, "winner" ) ) {
        row->winner = _elo_get_winner_from_winner_column( elo, row_number ) ;
        row->straight_sets = false ;
    } else {
        _elo_calculate_match_counts( elo, row_number, &row->player_set_counts[0], &row->player_set_counts[1] ) ;
        row->winner = _elo_get_winner_from_match_counts( row ) ;
        row->straight_sets = _elo_get_straight_sets_from_match_counts( row ) ;
    }
}

char* _elo_get_winner_from_winner_column( struct elo_calculator *elo, int row_number ) {
    struct sarr winner_sarr = *(struct sarr*)dict_get( &elo->data, "winner" ) ;
    return (char*)winner_sarr.contents[ row_number ] ;
}

char* _elo_get_winner_from_match_counts( struct elo_data_row *row ) { 
    if( row->player_set_counts[0] > row->player_set_counts[1] ) {
        return row->player_names[0] ;
    } else {
        return row->player_names[1] ;
    };
}

bool _elo_get_straight_sets_from_match_counts( struct elo_data_row *row ) {
    if( row->player_set_counts[0] == 0 || row->player_set_counts[1] == 0 ) {
        return true ;
    } else {
        return false ;
    }
}

void _elo_calculate_match_counts( struct elo_calculator *elo, int row_number, int *p1_match_count, int *p2_match_count ) {
    *p1_match_count = 0 ;
    *p2_match_count = 0 ;

    for( int game_number = 0 ; game_number < 7 ; game_number++ ) {
        int p1_score = _elo_calculate_player_score( elo, row_number, 1, game_number ) ;
        int p2_score = _elo_calculate_player_score( elo, row_number, 2, game_number ) ;

        if( p1_score > p2_score ) {
            *p1_match_count += 1 ;
        } else if ( p2_score > p1_score ) {
            *p2_match_count += 1 ;
        }
    }
}

int _elo_calculate_player_score( struct elo_calculator *elo, int row_number, int player_number, int game_number ) {
    char p1_header[8] ;
    snprintf( p1_header, sizeof( p1_header ), "G%dP%d", game_number + 1, player_number ) ;
    struct sarr p1_score_sarr = *(struct sarr*)dict_get( &elo->data, p1_header ) ;
    char *p1_score_str = (char*)p1_score_sarr.contents[ row_number ] ;
    return atoi( p1_score_str ) ;
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


// int main( void ) {
//     struct elo_calculator elo_calculator ;
//     elo_init( &elo_calculator ) ;

//     struct elo_config elo_config = {
//         .starting_elo = 1000.0f,
//         .diff_factor = 400.0f,
//         .k = 32.0f,
//         .boost_threshold = 1.0f,
//         .k_scaling = 1.25f
//     } ;

//     elo_load_data( &elo_calculator, "matches_database.csv" ) ;
//     elo_calculate_from_data( &elo_calculator, &elo_config ) ;
//     elo_write_elos( &elo_calculator, "elos.csv" ) ;

//     elo_free( &elo_calculator ) ;
// }
