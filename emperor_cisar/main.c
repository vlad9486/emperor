
/************************************************
 *  the high level language translator
 *  for the emperor project
 *  vlad9486, 05.02.2014
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "syntax_tree.h"

void translate(char* data, esize_t size, FILE* output)
{
    herror_t error;
    struct syntax_node_t* tree;

    tree = make_tree(data, size, &error);
    delete_tree(tree);
}

int main(int argc, char** argv)
{
    FILE* input;
    FILE* output;
    char* data;
    esize_t size;

    if (argc < 3) {
        printf("[ERROR]: too few parameters\n");
        return EXIT_FAILURE;
    }

    input = fopen(argv[1], "rt");
    fseek(input, 0, SEEK_END);
    size = ftell(input);
    rewind(input);
    data = malloc(size);
    fread(data, size, 1, input);
    fclose(input);

    output = fopen(argv[2], "wb");
    translate(data, size, output);
    free(data);
    fclose(output);

    return EXIT_SUCCESS;
}
