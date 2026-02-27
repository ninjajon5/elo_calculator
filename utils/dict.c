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

void dict_free_with_nested_sarrs( struct dict *dict ) {
    for( int i = 0 ; i < dict->values.len ; i++ ) {
        struct sarr *nested_sarr = (struct sarr*)dict->values.contents[i] ;
        sarr_free( nested_sarr ) ;
        free( nested_sarr ) ; // _load_headers_into_keys uses malloc()
    }
    // can't call dict_free because we've done a special case of _sarr_free_each_element above, and dict_free would therefore double-free
    sarr_free( &dict->keys ) ;  
    free( dict->values.contents ) ;
    dict->values.contents = NULL ;
    dict->values.capacity = 0 ;
    dict->values.len = 0 ;
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
