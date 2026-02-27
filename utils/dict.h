#ifndef DICT_H
#define DICT_H

#include "sarr.h"

/*
dict = python-like dictionary
keys must be strings
values can be any type, though you must keep track - dict_get returns a void pointer
*/

struct dict {
    struct sarr keys ;
    struct sarr values ;
} ;

void dict_init( struct dict *base_dict ) ;
void dict_add( struct dict *dict, char *key, void *value, size_t key_size, size_t value_size ) ;
void* dict_get( struct dict *dict, char *input_key ) ;
int dict_has_key( struct dict *dict, char *input_key ) ;
void dict_free( struct dict *dict ) ;
void dict_free_with_nested_sarrs( struct dict *dict ) ;

#endif
