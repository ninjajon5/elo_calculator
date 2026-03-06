#ifndef IO_H
#define IO_H

#include "dict.h"

void print_data( char *path ) ;
void print_headers( char *path ) ;
struct dict load_data( char *path ) ;
void write_data_dict( struct dict *data, char *path ) ;
int get_number_of_data_rows( struct dict *data ) ;

#endif
