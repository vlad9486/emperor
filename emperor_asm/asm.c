#include <stdio.h>
#include "asm.h"
#include "common.h"

typedef struct op_tag {
    const char* name;
    signed char code;
    /*0 - end; 1 - read register code; 2 - read 16 bit; 3 - read to end;
    5-7 - write 1 bit with 1st-3th tail; 8-a - write 2 bit with 1st-3th tail;*/
    unsigned int program;
} op_t;

typedef struct reg_tag {
    unsigned char number;
    unsigned char type; /*0 - int; 1 - long; 2 - float; 3 - double; 4 - none*/
    unsigned char size;
} reg_t;

const op_t table[] = {
    {   "mov", 0x00, 0x9181},
    {   "add", 0x10, 0x8111},
    {   "sub", 0x14, 0x8111},
    {   "mul", 0x18, 0x8111},
    {   "div", 0x1c, 0x8111},
    {   "neg", 0x20, 0x0811},
    {   "cmp", 0x24, 0x0811},
    {   "inc", 0x28, 0x0511},
    {   "dec", 0x2a, 0x0511},
    {   "shl", 0x2c, 0x5111},
    {   "shr", 0x2e, 0x5111},
    {   "not", 0x30, 0x0511},
    {  "nand", 0x32, 0x5111},
    {   "nor", 0x34, 0x5111},
    {  "nxor", 0x36, 0x5111},
    {   "mod", 0x38, 0x5111},
    {   "and", 0x3a, 0x5111},
    {    "or", 0x3c, 0x5111},
    {   "xor", 0x3e, 0x5111},
    {   "cle", 0x40, 0x0000},
    {   "ste", 0x41, 0x0000},
    {   "cll", 0x42, 0x0000},
    {   "stl", 0x43, 0x0000},
    {   "clg", 0x44, 0x0000},
    {   "stg", 0x45, 0x0000},
    {   "clk", 0x46, 0x0000},
    {   "stk", 0x47, 0x0000},
    {   "new", 0x48, 0x0011},
    {"delete", 0x49, 0x0001},
    {  "size", 0x4a, 0x0011},
    {   "lit", 0x4b, 0x0012},
    {  "self", 0x4c, 0x0001},
    {"system", 0x4d, 0x0001},
    {  "hole", 0x4e, 0x0000},
    { "store", 0x50, 0x8111},
    {  "load", 0x54, 0xa111},
    {  "goto", 0x58, 0x0011},
    { "egoto", 0x59, 0x0011},
    { "lgoto", 0x5a, 0x0011},
    {"nggoto", 0x5b, 0x0011},
    { "ggoto", 0x5c, 0x0011},
    {"nlgoto", 0x5d, 0x0011},
    {"negoto", 0x5e, 0x0011},
    {   "nop", 0x5f, 0x0011},
    {  "data", 0xff, 0x0003},
};

const char* tails[] = {
    "_i", "_l", "_f", "_d", "_n",
};

bool_t compare_part(const char* a, const char* b)
{
    int i;

    i = 0;
    while (a[i] != 0) {
        if (a[i] != b[i]) {
            return FALSE;
        }
        i++;
    }

    return TRUE;
}

int length(const char* s)
{
    int i;

    i = 0;
    while (s[i]) {
        i++;
    }
    return i;
}

const op_t* find_op(char* str)
{
    int i;
    int entries;
    bool_t condition;

    i = 0;
    entries = sizeof(table)/sizeof(table[0]);
    while (i < entries) {
        condition = compare_part(table[i].name, str);
        if (condition) {
            return &table[i];
        }
        i++;
    }

    return NULL;
}

int read_hex(char* line, int n)
{
    char c;

    c = (((*line) >= 'a') ? (*line + 10 - 'a') : (*line - '0'));

    return ((n > 0) && (0 != *line)) ? ((1 << (4 * (n - 1))) * c + read_hex(line + 1, n - 1)) : 0;
}

reg_t read_reg(char* str)
{
    reg_t reg;
    int entries;
    bool_t condition;

    reg.number = (unsigned char)read_hex(str, 2);
    reg.type = 0;
    reg.size = 2;

    entries = sizeof(tails)/sizeof(tails[0]);
    while (reg.type < entries) {
        condition = compare_part(tails[reg.type], str+reg.size);
        if (condition) {
            break;
        }
        (reg.type)++;
    }

    reg.size += 2;

    return reg;
}

unsigned short rev2(unsigned short x)
{
    return ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8);
}

unsigned int rev4(unsigned int x)
{
    return ((x & 0x000000ff) << 24) | ((x & 0x0000ff00) <<  8) | 
           ((x & 0x00ff0000) >>  8) | ((x & 0xff000000) >> 24);
}

void execute_program(char* str, int* code, int program)
{
    int pos;
    int suffix_pos;
    int reg_index;
    reg_t regs[3];
    unsigned short lit;

    reg_index = 0;
    pos = 8;
    suffix_pos = 0;
    while (program) {
        if ((program & 0xf) == 1) {
            regs[reg_index] = read_reg(str);
            str += regs[reg_index].size;
            str++;
            (*code) |= (regs[reg_index].number << pos);
            pos += 8;
            reg_index++;
        }
        if ((program & 0xf) == 2) {
            lit = read_hex(str, 4);
            lit = rev2(lit);
            str += 5;
            (*code) |= (lit << pos);
            pos += 16;
        }
        if ((program & 0xf) == 3) {
            *code = rev4(read_hex(str, 8));
            str += 8;/*unn*/
        }
        if (((program & 0xf) >= 5) && ((program & 0xf) <= 7)) {
            *code |= ((regs[(program & 0xf) - 5].type) & ((1 << 1) - 1)) << suffix_pos;
            suffix_pos++;
        }
        if (((program & 0xf) >= 8) && ((program & 0xf) <= 10)) {
            *code |= ((regs[(program & 0xf) - 8].type) & ((1 << 2) - 1)) << suffix_pos;
            suffix_pos += 2;
        }
        program >>= 4;
    }
}

int process_line(char* line, FILE* output)
{
    const op_t* op;
    int code;

    line++;
    op = find_op(line);
    code = 0;
    line += length(op->name);
    line++;
    code |= op->code;
    execute_program(line, &code, op->program);
    fwrite(&code, 4, 1, output);

    return 0;
}
