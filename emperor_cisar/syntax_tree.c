#include <stdlib.h>
#include "syntax_tree.h"

#define HAS_NAME        (1 << 0)
#define HAS_CONDITION   (1 << 1)
#define END_TERMINATED  (1 << 2)

const char* node_str[] = {
    "module", "using", "func", "do", "var",
    "const", "type", "if", "while", "enum", "struct"
};

char node_flags[] = {
    HAS_NAME | END_TERMINATED,
    0,
    HAS_NAME | END_TERMINATED,
    0,
    0,
    0,
    0,
    HAS_CONDITION | END_TERMINATED,
    HAS_CONDITION | END_TERMINATED,
    HAS_NAME | END_TERMINATED,
    HAS_NAME | END_TERMINATED
};

const char* white_space = " \t\n";
const char* terminator = "end";

bool_t find_char(char c, const char* line)
{
    esize_t i;

    i = 0;
    while (line[i] != 0) {
        if (line[i] == c) {
            return TRUE;
        }
        i++;
    }

    return FALSE;
}

bool_t compare_part(const char* a, const char* str, esize_t* pos)
{
    esize_t i;
    bool_t condition;

    i = 0;
    while (a[i] != 0) {
        if (a[i] != str[*pos + i]) {
            return FALSE;
        }
        i++;
    }

    condition = (str[*pos + i] == 0) || find_char(str[*pos + i], white_space);
    if (!condition) {
        return condition;
    }

    (*pos) += i;
    return TRUE;
}

void read_white(char* data, esize_t size, esize_t* pos, esize_t* line, herror_t* perror)
{
    bool_t condition;

    condition = find_char(data[*pos], white_space);
    if (!condition) {
        *perror = 1;
        return;
    }
    while (condition) {
        condition = find_char(data[*pos], white_space);
        if (data[*pos] == '\n') {
            (*line)++;
        }
        (*pos)++;
    }
}

struct syntax_node_t* read_node(char* data, esize_t size, esize_t* pos, esize_t* line, herror_t* perror)
{
    struct syntax_node_t* root;
    int i;
    int n;

    root = malloc(sizeof(*root));
    root->childs = NULL;
    root->name = NULL;
    root->condition = NULL;

    read_white(data, size, pos, line, perror);
    perror = 0; /* ignore error here */

    i = 0;
    n = sizeof(node_str)/sizeof(node_str[0]);
    while (i < n) {
        if (compare_part(node_str[i], data, pos)) {
            break;
        }
        i++;
    }
    if (i == n) {
        if (compare_part(terminator, data, pos)) {
        }
        else {
            perror = 1;
        }
        return root;
    }
    root->type = i;

    if (node_flags[i] && HAS_NAME) {
        /* read name */
    }

    if (node_flags[i] && HAS_CONDITION) {
        /* read condition */
    }

    return root;
}

struct syntax_node_t* make_tree(char* data, esize_t size, herror_t* perror)
{
    esize_t pos;
    esize_t line;
    
    pos = 0;
    line = 0;
    return read_node(data, size, &pos, &line, perror);
}

void delete_tree(struct syntax_node_t* root)
{
    if (root->childs) {
        delete_dict(root->childs, delete_tree);
    }
    free(root->name);
    free(root->condition);
    free(root);
}
