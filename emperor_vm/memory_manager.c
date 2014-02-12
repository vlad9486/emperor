#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory_manager.h"

/* TODO:
 * cache hdata_t <-> ptr_t conformity
 * memory mapped file
 * synchronizing with file
 */

#define NODES_NUMBER (1 << BIT_PER_LEVEL)

typedef struct leaf_tag {
    esize_t size;
    lword_t data; /*users data, probably more than one word, i hope :)*/
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

bool_t try_to_create_node(hdata_t* data, table_t** table, leaf_t* ptr, 
    char level, herror_t* perror)
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
                (*table)->nodes[i] = (table_t*)ptr;
                (*table)->header++;
                (*data) |= i;
                return TRUE;
            }
        }
        else {
            condition = try_to_create_node(data, &((*table)->nodes[i]), ptr,
                level-1, perror);
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

hdata_t alloc_handle(leaf_t* ptr, herror_t* perror)
{
    hdata_t res;
    bool_t condition;

    res = 0;
    condition = try_to_create_node(&res, &links_4, ptr, LEVELS_NUMBER-1, perror);
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

leaf_t* free_handle(hdata_t data, herror_t* perror)
{
    char i;
    uint16_t index;
    table_t*** temp;
    leaf_t* res;

    temp = malloc((LEVELS_NUMBER+1)*sizeof(*temp));
    temp[LEVELS_NUMBER] = &links_4;
    i = LEVELS_NUMBER;
    while (i != 0) {
        if (*(temp[i]) == 0) {
            *perror = E_INVALIDPTR;
            return 0;
        }
        index = (data >> ((i-1)*BIT_PER_LEVEL)) & (NODES_NUMBER-1);
        temp[i-1] = &((*(temp[i]))->nodes[index]);
        i--;
    }

    res = (leaf_t*)(*(temp[0]));
    i = 0;
    while (i < LEVELS_NUMBER) {
        (*temp[i+1])->header--;
        if ((*temp[i+1])->header == 0) {
            free(*(temp[i+1]));
            *(temp[i+1]) = 0;
        }
        i++;
    }
    *(temp[0]) = 0;
    free(temp);
                 
    return res;
}

leaf_t* resolve_conformity(hdata_t data, herror_t* perror)
{
    char i;
    uint16_t index;
    table_t* table;
    leaf_t* res;

    table = links_4;
    i = LEVELS_NUMBER;
    while (i != 0) {
        if (table == 0) {
            *perror = E_INVALIDPTR;
            return 0;
        }
        index = (data >> ((i-1)*BIT_PER_LEVEL)) & (NODES_NUMBER-1);
        table = table->nodes[index];
        i--;
    }
    res = (leaf_t*)table;

    /* TODO: CACHE USAGE */

    return res;
}

word_t rev4(word_t x)
{
    return ((x & 0x000000ff) << 24) | ((x & 0x0000ff00) <<  8) | 
           ((x & 0x00ff0000) >>  8) | ((x & 0xff000000) >> 24);
}

hdata_t bind_file(const char* fn, herror_t* perror)
{
    hdata_t data;
    esize_t size;
    FILE* file;
    char k;
    index_t index;
    word_t* ptr;

    file = fopen(fn, "rb");
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    k = size % sizeof(word_t);
    size /= sizeof(word_t);
    data = create_array(size*sizeof(lword_t)/sizeof(word_t)+(k != 0), perror);
    if (*perror != E_NONE) {
        return 0;
    }
    ptr = get_pointer_unsafe(data, perror);
    if (*perror != E_NONE) {
        return 0;
    }
    fread(ptr, (size+(k != 0))*sizeof(word_t), 1, file);
    index = 0;
    while (index < (size+(k != 0))) {
        ptr[index] = rev4(ptr[index]);
        index++;
    }
    fclose(file);

    /* TODO: MEMORY MAPPING */

    return data;
}

void sync_data(hdata_t data, herror_t* perror)
{
    /* TODO */
}

hdata_t create_array(esize_t size, herror_t* perror)
{
    leaf_t* ptr;
    hdata_t res;

    ptr = malloc(size*sizeof(lword_t)+sizeof(esize_t));
    if (ptr == 0) {
        *perror = E_OUTOFMEMORY;
        return 0;
    }
    ptr->size = size;
    memset(&(ptr->data), 0, size*sizeof(lword_t));

    res = alloc_handle(ptr, perror);
    if (*perror != E_NONE) {
        return 0;
    }
    printf("[LOG]: created %d, 0x%x\n", res, ptr);

    return res;
}

void destroy_array(hdata_t data, herror_t* perror)
{
    leaf_t* ptr;

    ptr = free_handle(data, perror);
    if (*perror != E_NONE) {
        return;
    }
    printf("[LOG]: deleted %d, 0x%x\n", data, ptr);

    if (ptr->data != 0) {
        free(ptr->data);
    }
    free(ptr);
}

void write_word(hdata_t data, index_t index, lword_t word, herror_t* perror)
{
    leaf_t* ptr;
    lword_t* array;

    ptr = resolve_conformity(data, perror);
    if (*perror != E_NONE) {
        return;
    }

    if (index >= ptr->size) {
        *perror = E_ACCESSFORBIDDEN;
        return;
    }

    array = &(ptr->data);
    array[index] = word;
}

lword_t read_word(hdata_t data, index_t index, herror_t* perror)
{
    leaf_t* ptr;
    lword_t* array;

    ptr = resolve_conformity(data, perror);
    if (*perror != E_NONE) {
        return 0;
    }

    if (index >= ptr->size) {
        *perror = E_ACCESSFORBIDDEN;
        return 0;
    }

    array = &(ptr->data);
    return array[index];
}

index_t get_size(hdata_t data, herror_t* perror)
{
    leaf_t* ptr;

    ptr = resolve_conformity(data, perror);
    if (*perror != E_NONE) {
        return 0;
    }

    return ptr->size;
}

void* get_pointer_unsafe(hdata_t data, herror_t* perror)
{
    leaf_t* ptr;

    ptr = resolve_conformity(data, perror);
    if (*perror != E_NONE) {
        return 0;
    }

    return &(ptr->data);
}
