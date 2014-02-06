#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

struct dict_entry_t {
    void* data;
    struct dict_entry_t* next;
    struct dict_entry_t* prev;
};

typedef int (compare_func_t)(void*, void*);
typedef int (predicate_func_t)(void*);

struct dict_entry_t* create_dict();
void** add_entry_to_dict(struct dict_entry_t* dict, void* data, compare_func_t* compare);
void* access_to_dict(struct dict_entry_t* dict, predicate_func_t* predicate);
void* access_by_index_to_dict(struct dict_entry_t* dict, int n);
int find_in_dict(struct dict_entry_t* dict, void* data, predicate_func_t* predicate);
void delete_dict(struct dict_entry_t* dict);

#endif // DICTIONARY_H_INCLUDED
