#include <stdlib.h>
#include "dictionary.h"

struct dict_entry_t* create_dict()
{
    struct dict_entry_t* dict;

    dict = malloc(sizeof(*dict));
    dict->data = 0;
    dict->next = dict;
    dict->prev = dict;

    return dict;
}

void** add_entry_to_dict(struct dict_entry_t* dict, void* data, compare_func_t* compare)
{
    struct dict_entry_t* temp;
    bool_t condition;

    temp = dict->next;
    while (temp != dict) {
        condition = compare(temp->data, data);
        if (condition) {
            return 0;
        }
        temp = temp->next;
    }
    temp = malloc(sizeof(*temp));
    temp->next = dict;
    temp->prev = dict->prev;
    dict->prev->next = temp;
    dict->prev = temp;

    return &temp->data;
}

void* access_to_dict(struct dict_entry_t* dict, predicate_func_t* predicate)
{
    struct dict_entry_t* temp;
    bool_t condition;

    temp = dict->next;
    while (temp != dict) {
        condition = predicate(temp->data);
        if (condition) {
            return temp->data;
        }
        temp = temp->next;
    }

    return 0;
}

void* access_by_index_to_dict(struct dict_entry_t* dict, int n)
{
    struct dict_entry_t* temp;
    int i;

    i = 0;
    temp = dict->next;
    while (temp != dict) {
        if (i == n) {
            return temp->data;
        }
        temp = temp->next;
        i++;
    }

    return 0;
}

int find_in_dict(struct dict_entry_t* dict, void* data, predicate_func_t* predicate)
{
    struct dict_entry_t* temp;
    bool_t condition;
    int i;

    i = 0;
    temp = dict->next;
    while (temp != dict) {
        condition = predicate(temp->data);
        if (condition) {
            return i;
        }
        temp = temp->next;
    }

    return -1;
}

void delete_dict(struct dict_entry_t* dict)
{
    struct dict_entry_t* temp;
    struct dict_entry_t* ptr;

    temp = dict->next;
    while (temp != dict) {
        ptr = temp;
        temp = temp->next;
        free(ptr);
    }
    free(temp);
}
