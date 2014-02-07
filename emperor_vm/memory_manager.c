#include <string.h>
#include <stdlib.h>
#include "memory_manager.h"

/* TODO: fix free_handle, add size field to users data,
 * cache hdata_t <-> ptr_t conformity
 * memory mapped file
 */

/* on the leaf are pointers,
 * 0 means forbidden (non allocated) table,
 * first 16 bytes is header
 */

typedef struct leaf_tag {
    uint64_t size;
    word_t data[1]; /*users data, probably more than one word, i hope :)*/
} leaf_t;

typedef struct table_tag {
    uint64_t size;
    uint64_t header;
    struct table_tag* nodes[NODES_NUMBER];
} table_t;

table_t* create_table(herror_t* perror)
{
    table_t* res;

    res = malloc(sizeof(*res));
    if (res == 0) {
        *perror = E_OUTOFMEMORY;
        return 0;
    }
    memset(res, 0, sizeof(*res));
    res->size = sizeof(*res);
    res->header = 0;
    *perror = E_NONE;

    return res;
}

void free_table(table_t* table, herror_t* perror)
{
    free(table);
    *perror = 0;
}

table_t* links_4 = 0;

bool_t try_to_create_node(hdata_t* data, table_t** table, ptr_t ptr, 
    esize_t size, char level, herror_t* perror)
{
    uint16_t i;
    bool_t condition;

    if (*table == 0) {
        *table = create_table(perror);
        if (*perror != E_NONE) {
            return FALSE;
        }
    }

    if ((*table)->header == NODES_NUMBER) {
        return FALSE;
    }

    i = 0;
    while (i < NODES_NUMBER) {
        if (level == 0) {
            if ((*table)->nodes[i] == 0) {
                (*table)->nodes[i] = ptr;
                /*fixme*/
                (*table)->header++;
                (*data) |= i;
                return TRUE;
            }
        }
        else {
            condition = try_to_create_node(data, &((*table)->nodes[i]), ptr,
                size, level-1, perror);
            if (*perror != E_NONE) {
                return FALSE;
            }
            if (condition) {
                (*table)->header++;
                (*data) |= (i << (level*BIT_PER_LEVEL));
                return TRUE;
            }
        }
        i++;
    }

    return FALSE;
}

hdata_t alloc_handle(ptr_t ptr, esize_t size, herror_t* perror)
{
    hdata_t res;
    bool_t condition;

    res = 0;
    condition = try_to_create_node(&res, &links_4, ptr, size, LEVELS_NUMBER-1, perror);
    if (*perror != E_NONE) {
        return 0;
    }
    if (condition) {
        return res;
    }
    else {
        *perror = E_OUTOFMEMORY;
        return 0;
    }
}

ptr_t free_handle(hdata_t data, herror_t* perror)
{
    char i;
    uint16_t index;
    table_t* table;
    table_t* temp;

    table = links_4;
    i = LEVELS_NUMBER;
    while (i != 0) {
        index = (data >> ((i-1)*BIT_PER_LEVEL)) & ((1 << BIT_PER_LEVEL)-1);
        table->header--;
        temp = table;
        table = temp->nodes[index];
        i--;
    }   /*FIXME*/

    if (temp != 0) {
        temp->nodes[index] = 0;
    }

    return table;
}

hdata_t load_data(const char* fn, herror_t* perror)
{
    /**/
}

void save_data(const char* fn, hdata_t data, herror_t* perror)
{
    /**/
}

hdata_t create_array(esize_t size, herror_t* perror)
{
    ptr_t ptr;
    hdata_t res;

    ptr = malloc(size);
    if (ptr == 0) {
        *perror = E_OUTOFMEMORY;
        return 0;
    }
    memset(ptr, 0, size);

    res = alloc_handle(ptr, size, perror);
    if (*perror != E_NONE) {
        return 0;
    }

    return res;
}

void free_array(hdata_t data, herror_t* perror)
{
    ptr_t ptr;

    ptr = free_handle(data, perror);
    if (*perror != E_NONE) {
        return;
    }

    free(ptr);
}

void write_word(hdata_t data, index_t index, word_t word, herror_t* perror)
{
    /**/
}

word_t read_word(hdata_t data, index_t index, herror_t* perror)
{
    /**/
}
