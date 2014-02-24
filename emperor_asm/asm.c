#include <stdio.h>
#include "asm.h"
#include "common.h"

typedef struct op_tag {
    const char* name;
    unsigned char code;
    unsigned char prog;
} op_t;

const op_t table[] = {
    {   "nop", 0x00, 0x00},
    {   "mov", 0x01, 0x0a},
    {   "neg", 0x02, 0x0a},
    {   "cmp", 0x03, 0x0a},
    {   "add", 0x04, 0x2a},
    {   "sub", 0x05, 0x2a},
    {   "mul", 0x06, 0x2a},
    {   "div", 0x07, 0x2a},
    {   "mod", 0x08, 0x2a},
    {   "and", 0x09, 0x2a},
    {    "or", 0x0a, 0x2a},
    {   "xor", 0x0b, 0x2a},
    {   "not", 0x0c, 0x0a},
    {  "nand", 0x0d, 0x2a},
    {   "nor", 0x0e, 0x2a},
    {  "nxor", 0x0f, 0x2a},
    
    {   "inc", 0x10, 0x0a},
    {   "dec", 0x11, 0x0a},
    {   "shl", 0x12, 0x2e},
    {   "shr", 0x13, 0x2e},
    {  "hole", 0x14, 0x00},
    {  "hole", 0x15, 0x00},
    {  "hole", 0x16, 0x00},
    {  "hole", 0x17, 0x00},
    {   "lit", 0x18, 0x0b},
    {   "lip", 0x19, 0x0b},
    {  "hole", 0x1a, 0x00},
    {  "hole", 0x1b, 0x00},
    {  "hole", 0x1c, 0x00},
    {  "hole", 0x1d, 0x00},
    {  "hole", 0x1e, 0x00},
    {  "hole", 0x1f, 0x00},

    {   "cle", 0x20, 0x00},
    {   "ste", 0x21, 0x00},
    {   "cll", 0x22, 0x00},
    {   "stl", 0x23, 0x00},
    {   "clg", 0x24, 0x00},
    {   "stg", 0x25, 0x00},
    {   "clk", 0x26, 0x00},
    {   "stk", 0x27, 0x00},

    {   "new", 0x28, 0x0a},
    {"delete", 0x29, 0x02},
    {  "size", 0x2a, 0x0a},
    {  "self", 0x2b, 0x02},
    { "store", 0x2c, 0x2a},
    {  "load", 0x2d, 0x2a},
    {"system", 0x2e, 0x03},
    {    "il", 0x2f, 0x00},

    {  "goto", 0x30, 0x0a},
    { "egoto", 0x31, 0x0a},
    { "lgoto", 0x32, 0x0a},
    {"nggoto", 0x33, 0x0a},
    { "ggoto", 0x34, 0x0a},
    {"nlgoto", 0x35, 0x0a},
    {"negoto", 0x36, 0x0a},
    { "kgoto", 0x37, 0x0a},

    {  "data", 0xff, 0xff},
};

bool_t compare_part(const char* a, const char* str, size_t* pos)
{
    int i;
    bool_t condition;

    i = 0;
    while (a[i] != 0) {
        if (a[i] != str[*pos + i]) {
            return FALSE;
        }
        i++;
    }

    condition = (str[*pos + i] == 0) || (str[*pos + i] == ' ')
            || (str[*pos + i] == '\n');
    if (!condition) {
        return condition;
    }

    (*pos) += i;
    return TRUE;
}

int find_op(char* str, const op_t** op, size_t* pos)
{
    int i;
    int entries;
    bool_t condition;

    i = 0;
    entries = sizeof(table)/sizeof(table[0]);
    while (i < entries) {
        condition = compare_part(table[i].name, str, pos);
        if (condition) {
            (*op) = &table[i];
            return 0;
        }
        i++;
    }

    return -1;
}

static inline unsigned short rev2(unsigned short x)
{
    return ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8);
}

static inline unsigned int rev4(unsigned int x)
{
    return ((x & 0x000000ff) << 24) | ((x & 0x0000ff00) <<  8) | 
           ((x & 0x00ff0000) >>  8) | ((x & 0xff000000) >> 24);
}

static inline unsigned char c2i(char c)
{
    return (c >= 'a') ? (c - 'a' + 10) : (c - '0');
}

static inline unsigned char length(char* line, size_t* pos)
{
    unsigned char r;

    r = 0;
    while ((line[*pos + r] != ' ') && (line[*pos + r] != 0)) {
        r++;
    }
    (*pos) += r;

    return r;
}

int read_reg(char* line, unsigned char* res, size_t* pos)
{
    unsigned char temp;
    unsigned char l;

    if (line[*pos] != 'r') {
        return -1;
    }
    (*pos)++;

    if (line[*pos] == 0) {
        return -1;
    }
    (*res) = c2i(line[*pos]) << 4;
    (*pos)++;

    if ((line[*pos] == 0) || (line[*pos] == ' ')) {
        (*res) |= 0xe;
        return 0;
    }
    else if (line[*pos] == '_') {
        (*pos)++;
        l = length(line, pos);
        if (l == 0) {
            return -1;
        }
        temp = 0xe << l;
        while (l) {
            temp |= (line[*pos - l] == '1') << (l - 1);
            l--;
        }
        temp &= 0xf;
        (*res) |= temp;
    }
    else {
        return -1;
    }

    return 0;
}

int read_hex(char* line, unsigned int* res, size_t* pos)
{
    unsigned char l;

    (*res) = 0;
    l = length(line, pos);
    while (l) {
        (*res) <<= 4;
        (*res) |= c2i(line[*pos - l]);
        l--;
    }
    
    return 0;
}

int execute_program(char* line, unsigned int* code, unsigned char program, size_t* pos)
{
    unsigned char reg;
    unsigned char p;
    int res;
    size_t temp;
    unsigned int temp_code;

    if (program == 0xff) {
        temp = *pos;
        res = read_hex(line, code, pos);
        if (res) {
            return res;
        }
        if ((*pos - temp) != 8) {
            return -1;
        }
        (*code) = rev4(*code);
        return 0;
    }

    p = 8;
    while (program & 2) {
        if (p == 32) {
            return -1;
        }
        if (program & 1) {
            temp = *pos;
            res = read_hex(line, &temp_code, pos);
            if (res) {
                return res;
            }
            if ((*pos - temp) == 2) {
                (*code) |= temp_code << p;
                p += 8;
            }
            else if ((*pos - temp) == 4) {
                temp_code = rev2(temp_code);
                (*code) |= temp_code << p;
                p += 16;
            }
            else {
                return -1;
            }
            (*pos)++;
        }
        else {
            res = read_reg(line, &reg, pos);
            if (res) {
                return res;
            }
            (*code) |= reg << p;
            p += 8;
            (*pos)++;
        }
        program >>= 2;
    }
    return 0;
}

int process_line(char* line, FILE* output)
{
    size_t pos;
    int res;
    const op_t* op;
    unsigned int code;
    unsigned int bad;

    bad = 0xffffffff;

    pos = 1;
    res = find_op(line, &op, &pos);
    if (res) {
        fwrite(&bad, 4, 1, output);
        return res;
    }
    pos++;
    code = op->code;
    res = execute_program(line, &code, op->prog, &pos);
    if (res) {
        fwrite(&bad, 4, 1, output);
        return res;
    }

    fwrite(&code, 4, 1, output);
    return 0;
}
