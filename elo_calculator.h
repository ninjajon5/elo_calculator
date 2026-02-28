#ifndef ELO_CALCULATOR_H
#define ELO_CALCULATOR_H

#include "utils/dict.h"

struct elo_calculator {
    struct dict data ;
    struct dict elos ;
} ;

struct elo_config {
    float starting_elo ;
    float diff_factor ;
    float k ;
} ;

struct elo_data_row {
    int row_number ;
    char *player_names[2] ;
    char *winner ;
    float player_results [2] ;
    float player_elos[2] ;
    float player_elo_changes[2] ;
    float player_expected_results[2] ;
} ;

void elo_init( struct elo_calculator *elo ) ;
void elo_load_data( struct elo_calculator *elo, char *path ) ;
void elo_calculate_from_data( struct elo_calculator *elo, float starting_elo, float diff_factor, float k ) ;
void elo_free( struct elo_calculator *elo ) ;

#endif
