#include "dict.h"
#include "sarr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _dict_find_key_index( struct dict *dict, char *input_key ) ;

void dict_init( struct dict *base_dict ) {
    sarr_init( &base_dict->keys, 1 ) ;
    sarr_init( &base_dict->values, 1 ) ;
}

void dict_add( struct dict *dict, char *key, void *value, size_t key_size, size_t value_size ) {
    sarr_append( &dict->keys, key, key_size ) ;
    sarr_append( &dict->values, value, value_size ) ;
}

void* dict_get( struct dict *dict, char *input_key ) {
    int index = _dict_find_key_index( dict, input_key ) ;

    if ( index == -1 ) {
        fprintf(stderr, "dict: No matching key found\n") ;
        exit( 1 ) ;
    }

    return dict->values.contents[ index ] ;
}

int dict_has_key( struct dict *dict, char *input_key ) {
    return _dict_find_key_index( dict, input_key ) != -1 ;
}

void dict_free( struct dict *dict ) {
    sarr_free( &dict->keys ) ;
    sarr_free( &dict->values ) ;
}

int _dict_find_key_index( struct dict *dict, char *input_key ) {
    for ( int i = 0 ; i < dict->keys.len ; i++ ) {
        char *key = (char*)dict->keys.contents[i] ;
        int key_comparison = strcmp( key, input_key ) ;
        if ( key_comparison == 0 ) {
            return i ; // index of found key
        }
    }

    return -1 ; // key not found
}
