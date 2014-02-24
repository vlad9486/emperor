#ifndef MEMORY_MANAGER_H
#define	MEMORY_MANAGER_H

#include "common.h"

#define BIT_PER_LEVEL 8
#define LEVELS_NUMBER 4
#define SIZE_KOEF sizeof(hword_t)

/* binding an array to the file */
hdata_t bind_file(const char* fn, herror_t* perror);
/* create and destroy an array */
hdata_t create_array(esize_t size, herror_t* perror);
void destroy_array(hdata_t data, herror_t* perror);
/* access to the array */
void write_qword(hdata_t data, index_t index, qword_t word, herror_t* perror);
qword_t read_qword(hdata_t data, index_t index, herror_t* perror);
void write_dword(hdata_t data, index_t index, dword_t word, herror_t* perror);
dword_t read_dword(hdata_t data, index_t index, herror_t* perror);
void write_sword(hdata_t data, index_t index, sword_t word, herror_t* perror);
sword_t read_sword(hdata_t data, index_t index, herror_t* perror);
void write_hword(hdata_t data, index_t index, hword_t word, herror_t* perror);
hword_t read_hword(hdata_t data, index_t index, herror_t* perror);
/* get the size of the array */
index_t get_size(hdata_t data, herror_t* perror);
/* get the pointer to the array, unsafe */
void* get_pointer_unsafe(hdata_t data, herror_t* perror);

#endif	/* MEMORY_MANAGER_H */
