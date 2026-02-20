#ifndef SARR_H
#define SARR_H

#include <stdlib.h>

/*
sarr = smart array, an array with variable length
struct sarr name_of_sarr ;
sarr_init( &name_of_sarr, 10 ) ; where 10 is initial number of elements it can hold
sarr_append( &name_of_sarr, &name_of_input, sizeof(TYPE_OF_INPUT) ) ;
*(int*)test_sarr.contents[0] to access e.g. an int from index 0 (cast to int pointer, then dereference)
*/

struct sarr {
    void **contents ;   // array of void pointers
    size_t len ;
    size_t capacity ;
    size_t element_size ;
} ;

int sarr_init( struct sarr *base_sarr, size_t initial_capacity ) ;
int sarr_append( struct sarr *sarr, void *input, size_t input_size ) ;
void sarr_free( struct sarr *sarr ) ;

#endif
