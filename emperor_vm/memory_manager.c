#include "memory_manager.h"

/* on the leaf are pointers,
 * 0 means forbidden (non allocated) hdata_t,
 * if the greater bit is 1 there is a mapped file
 */
void**** links_4;

hdata_t load_data(const char* fn, herror_t* perror)
{
    /**/
}

void save_data(const char* fn, hdata_t data, herror_t* perror)
{
    /**/
}

hdata_t create_array(size_t size, herror_t* perror)
{
    /**/
}

void free_array(hdata_t data, herror_t* perror)
{
    /**/
}

void write_word(hdata_t data, index_t index, word_t word, herror_t* perror)
{
    /**/
}

word_t read_word(hdata_t data, index_t index, herror_t* perror)
{
    /**/
}
