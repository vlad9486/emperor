#ifndef MEMORY_MANAGER_H
#define	MEMORY_MANAGER_H

#include "common.h"

#define BIT_PER_LEVEL 8
#define LEVELS_NUMBER 4

/* binding an array to the file */
hdata_t bind_file(const char* fn, herror_t* perror);
void sync_data(hdata_t data, herror_t* perror);
/* create and destroy an array */
hdata_t create_array(esize_t size, herror_t* perror);
void destroy_array(hdata_t data, herror_t* perror);
/* access to the array */
void write_word(hdata_t data, index_t index, lword_t word, herror_t* perror);
lword_t read_word(hdata_t data, index_t index, herror_t* perror);
/* get the size of the array */
index_t get_size(hdata_t data, herror_t* perror);
/* get the pointer to the array, unsafe */
void* get_pointer_unsafe(hdata_t data, herror_t* perror);

#endif	/* MEMORY_MANAGER_H */
