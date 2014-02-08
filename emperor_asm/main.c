
/************************************************
    the assembler for the emperor project
    vlad9486, 18.01.2014
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "common.h"
#include "asm.h"

const char* separators = " \t";
const char* alphabet = "abcdefghijklmnopqrstuvwxyz_0123456789";
const char quote = '\"';
const char comment = ';';
const char endl = '\n';

int pos_(const char* str, char c)
{
    size_t i;

    i = 0;
    while (str[i] != 0) {
        if (str[i] == c) {
            return i;
        }
        i++;
    }
    return -1;
}

char* read_line(size_t* pos, char* data, size_t size)
{
    char* line;
    size_t i;
    size_t j;
    char buffer[1024];
    bool_t separator;
    bool_t alphabetical;
    bool_t whitespace_zone;
    bool_t comment_zone;

    separator = FALSE;
    alphabetical = FALSE;
    whitespace_zone = TRUE;
    comment_zone = FALSE;
    i = 0;
    while (data[*pos] != endl) {
        if (*pos >= size) {
            return NULL;
        }
        if (!comment_zone) {
            comment_zone = (data[*pos] == comment);
            if (comment_zone) {
                (*pos)++;
                continue;
            }
            separator = (pos_(separators, data[*pos]) != -1);
            alphabetical = (pos_(alphabet, data[*pos]) != -1);
            if (!(separator || alphabetical)) {
                (*pos)++;
                continue;
            }
            if (whitespace_zone) {
                whitespace_zone = separator;
                if (separator) {
                    (*pos)++;
                }
                else {
                    buffer[i] = separators[0];
                    i++;
                }
                continue;
           }
           buffer[i] = data[*pos];
           i++;
        }
        (*pos)++;
    }
    (*pos)++;

    if (i == 0) {
        if (*pos >= size) {
            return NULL;
        }
        return read_line(pos, data, size);
    }
    line = malloc(i+1);
    j = 0;
    while (j < i) {
        line[j] = buffer[j];
        j++;
    }
    line[j] = 0;

    return line;
}

void assembly(size_t size, char* data, FILE* output)
{
    size_t pos;
    char* line;
    int res;

    pos = 0;
    while (pos < size) {
        line = read_line(&pos, data, size);
        if (line == NULL) {
            break;
        }
        res = process_line(line, output);
        if (res == -1) {
            printf("syntax error in: %s\n", line);
        }
        free(line);
    }
}

int main(int argc, char* argv[])
{
    FILE* input;
    FILE* output;
    size_t size;
    char* data;

    if (argc < 3) {
        printf("too few parameters\n");
    }
    input = fopen(argv[1], "rb");
    fseek(input, 0, SEEK_END);
    size = ftell(input);
    rewind(input);
    data = malloc(size);
    fread(data, size, 1, input);
    fclose(input);

    output = fopen(argv[2], "wb");
    assembly(size, data, output);
    free(data);
    fclose(output);

    return EXIT_SUCCESS;
}
