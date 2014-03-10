#include <stdlib.h>
#include "syntax_tree.h"

#define HAS_NAME        (1 << 0)
#define HAS_CONDITION   (1 << 1)

const char* node_str[] = {
    "module", "using", "end", "func", "do", "var",
    "const", "if", "while", "type", "enum", "struct"
};

char node_flags[] = {
    HAS_NAME, 0, 0, HAS_NAME, 0, 0,
    0, HAS_CONDITION, HAS_CONDITION, 0, HAS_NAME, HAS_NAME
};

const char* white_space = " \t\n";

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

void read_node(char* data, esize_t size, esize_t* pos, esize_t* line, herror_t* perror)
{
    
}

struct syntax_node_t* make_tree(char* data, esize_t size, herror_t* perror)
{
    /**/
}

void delete_tree(struct syntax_node_t* root)
{
    /**/
}
