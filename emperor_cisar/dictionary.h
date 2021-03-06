#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

#include "common.h"

typedef struct dict_entry_t {
    void* data;
    struct dict_entry_t* next;
    struct dict_entry_t* prev;
};

typedef bool_t (compare_func_t)(void*, void*);
typedef bool_t (predicate_func_t)(void*);
typedef void (destroy_func_t)(void*);

struct dict_entry_t* create_dict();
void** add_entry_to_dict(struct dict_entry_t* dict, void* data, compare_func_t* compare);
void* access_to_dict(struct dict_entry_t* dict, predicate_func_t* predicate);
void* access_by_index_to_dict(struct dict_entry_t* dict, int n);
int find_in_dict(struct dict_entry_t* dict, void* data, predicate_func_t* predicate);
void delete_dict(struct dict_entry_t* dict, destroy_func_t* destroy_func);

#endif // DICTIONARY_H_INCLUDED
