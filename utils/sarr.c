#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sarr.h"

void _sarr_free_each_element( struct sarr *sarr ) ;
void _sarr_validate_initial_capacity( size_t initial_capacity ) ;
int _sarr_alloc_initial_capacity( struct sarr *base_sarr, size_t initial_capacity ) ;
int _sarr_append_within_capacity( struct sarr *sarr, void *input, size_t item_size ) ;
int _sarr_append_beyond_capacity( struct sarr *sarr, void *input, size_t item_size ) ;
int _sarr_realloc_capacity( struct sarr *sarr, int new_capacity ) ;

int sarr_init( struct sarr *base_sarr, size_t initial_capacity ) {
    if (  _sarr_alloc_initial_capacity( base_sarr, initial_capacity ) == 1 ) { 
        fprintf(stderr, "sarr: Failed to allocate initial capacity\n") ;
        exit( 1 ) ;
    }

    base_sarr->len = 0 ;
    base_sarr->capacity = initial_capacity ;

    return 0 ;
}

int sarr_append( struct sarr *sarr, void *input, size_t input_size ) {
    if( sarr->len < sarr->capacity ) {
        if ( _sarr_append_within_capacity( sarr, input, input_size ) == 1 ) {
            fprintf(stderr, "sarr: Failed to allocate input\n") ;
            exit( 1 ) ;
        }
    } else {
        if ( _sarr_append_beyond_capacity( sarr, input, input_size ) == 1 ) {
            fprintf(stderr, "sarr: Failed to reallocate capacity\n") ;
            exit( 1 ) ;
        }
    }

    return 0 ;
}

void sarr_free( struct sarr *sarr ) {
    _sarr_free_each_element( sarr ) ;

    free( sarr->contents ) ;
    sarr->contents = NULL ;
    sarr->capacity = 0 ;
    sarr->len = 0 ;
}

void _sarr_free_each_element( struct sarr *sarr ) {
    for ( int i = 0 ; i < sarr->len ; i++ ) {
        free( sarr->contents[i] ) ;
        sarr->contents[i] = NULL ;
    }
}

int _sarr_alloc_initial_capacity( struct sarr *base_sarr, size_t initial_capacity ) {
    _sarr_validate_initial_capacity( initial_capacity ) ;
    base_sarr->contents = malloc( sizeof *base_sarr->contents * initial_capacity ) ;
    if ( base_sarr->contents == NULL ) { return 1 ; } // malloc failed
    return 0 ;
}

void _sarr_validate_initial_capacity( size_t initial_capacity ) {
    if ( initial_capacity <= 0 ) {
        fprintf(stderr, "sarr: Initial capacity must be at least 1\n") ;
        exit( 1 ) ;
    }
}

int _sarr_append_within_capacity( struct sarr *sarr, void *input, size_t item_size ) {
    void *copy = malloc( item_size ) ;
    if ( copy == NULL ) { return 1 ; } // malloc failed

    memcpy( copy, input, item_size ) ;  // copy item_size number of bytes from 'input' to 'copy'
    // by doing this copy into malloc'd memory, we are using heap memory not stack memory
    // this means the memory will persist until free() - it would otherwise be cleared when the scope is exited

    sarr->contents[ sarr->len ] = copy ;
    sarr->len++ ;

    return 0 ;
}

int _sarr_append_beyond_capacity( struct sarr *sarr, void *input, size_t item_size ) {
    int new_capacity = sarr->capacity * 2 ;
    sarr->capacity = new_capacity ;

    if ( _sarr_realloc_capacity( sarr, new_capacity ) == 1 ) {
        return 1 ;
    } // realloc failed

    return _sarr_append_within_capacity( sarr, input, item_size ) ;
}

int _sarr_realloc_capacity( struct sarr *sarr, int new_capacity ) {
    void *new_contents = realloc(
        sarr->contents, // pointer to previously allocated memory
        sizeof *sarr->contents * new_capacity // size of new memory = double capacity
    ) ; // returns pointer to some new memory

    if ( new_contents == NULL ) {
        fprintf(stderr, "sarr_append: error reallocating memory\n") ;
        return 1 ; // realloc failed
    }

    sarr->contents = new_contents ;

    return 0 ;
}
