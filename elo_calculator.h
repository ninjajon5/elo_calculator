#ifndef ELO_CALCULATOR_H
#define ELO_CALCULATOR_H

#include "utils/dict.h"

struct elo_calculator {
    int starting_elo ;
    struct dict data ;
    struct dict elos ;
} ;

void elo_init( struct elo_calculator *elo ) ;
void elo_load_data( struct elo_calculator *elo, char *path ) ;
void elo_calculate( struct elo_calculator *elo, float starting_elo, float diff_factor, float k ) ;
void elo_free( struct elo_calculator *elo ) ;

#endif
