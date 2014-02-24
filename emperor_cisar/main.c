
/************************************************
 *  the high level language translator
 *  for the emperor project
 *  vlad9486, 05.02.2014
 ************************************************/

#include <stdio.h>
#include <stdlib.h>

void translate(FILE* input, FILE* output)
{
    
}

int main(int argc, char** argv)
{
    FILE* input;
    FILE* output;

    if (argc < 3) {
        printf("[ERROR]: too few parameters\n");
        return EXIT_FAILURE;
    }

    input = fopen(argv[1], "rt");
    output = fopen(argv[2], "wb");
    translate(input, output);
    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}
