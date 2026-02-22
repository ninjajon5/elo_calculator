#ifndef ELO_CALCULATOR_H
#define ELO_CALCULATOR_H

#include "utils/dict.h"

struct elo_calculator {
    struct dict data ;
    struct dict elos ;
} ;

void print_data( char *path ) ;
void print_headers( char *path ) ;

#endif
