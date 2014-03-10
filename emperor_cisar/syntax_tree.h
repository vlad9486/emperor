/* 
 * File:   syntax_tree.h
 * Author: vlad
 *
 * Created on 10 березня 2014, 11:46
 */

#ifndef SYNTAX_TREE_H
#define	SYNTAX_TREE_H

#include "dictionary.h"

typedef enum token_type_t {
    module_, using_, end_, func_, do_, var_,
    const_, if_, while_, type_, enum_, struct_, line_
};

typedef struct syntax_node_t {
    struct dict_entry_t* childs;
    enum token_type_t type;
    char* name;
    char* condition;
};

struct syntax_node_t* make_tree(char* data, esize_t size, herror_t* perror);
void delete_tree(struct syntax_node_t* root);

#endif	/* SYNTAX_TREE_H */

