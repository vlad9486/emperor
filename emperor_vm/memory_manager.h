#ifndef MEMORY_MANAGER_H
#define	MEMORY_MANAGER_H

#include "common.h"

#define BIT_PER_LEVEL 8
#define NODES_NUMBER (1 << BIT_PER_LEVEL)
#define LEVELS_NUMBER 4

hdata_t load_data(const char* fn, herror_t* perror);
void save_data(const char* fn, hdata_t data, herror_t* perror);
hdata_t create_array(esize_t size, herror_t* perror);
void free_array(hdata_t data, herror_t* perror);
void write_word(hdata_t data, index_t index, word_t word, herror_t* perror);
word_t read_word(hdata_t data, index_t index, herror_t* perror);

#endif	/* MEMORY_MANAGER_H */
