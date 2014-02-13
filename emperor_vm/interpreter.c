#include <stdio.h>
#include <math.h>
#include "virtual_machine.h"
#include "memory_manager.h"
#include "interpreter.h"

/* TODO:
 * instructions cache 
 * implement more system calls
 * optimize execute_code! that code is horrible
 * handling errors
 * floating point operation
 */

extern hdata_t main_module;
extern registry_file_t* registry_file;
extern char flags;

void syscall(uint64_t a, uint64_t b, uint64_t c, herror_t* perror);

static inline qword_t read_reg(hword_t reg)
{
    if ((reg & 0xf) == 0xe) {
        return registry_file->regs[reg >> 4];
    }
    else if ((reg & 0xc) == 0xc) {
        return registry_file->regs[reg >> 4] >> (8 * sizeof(dword_t) * (reg & 1));
    }
    else if ((reg & 0x8) == 0x8) {
        return registry_file->regs[reg >> 4] >> (8 * sizeof(sword_t) * (reg & 3));
    }
    else {
        return registry_file->regs[reg >> 4] >> (8 * sizeof(hword_t) * (reg & 7));
    }
}

static inline void write_reg(hword_t reg, qword_t word)
{
    dword_t* dtemp;
    sword_t* stemp;
    hword_t* htemp;

    if ((reg & 0xf) == 0xe) {
        registry_file->regs[reg >> 4] = word;
    }
    else if ((reg & 0xc) == 0xc) {
        dtemp = (dword_t*)(&(registry_file->regs[reg >> 4]));
        dtemp[reg & 1] = (dword_t)word;
    }
    else if ((reg & 0x8) == 0x8) {
        stemp = (sword_t*)(&(registry_file->regs[reg >> 4]));
        stemp[reg & 3] = (sword_t)word;
    }
    else {
        htemp = (hword_t*)(&(registry_file->regs[reg >> 4]));
        htemp[reg & 7] = (hword_t)word;
    }
}

void execute_code(hdata_t code, index_t pos, herror_t* perror)
{
    dword_t command;
    hword_t op;
    hword_t a;
    hword_t b;
    hword_t c;

    while ((flags & 0x8) == 0) {
        command = read_dword(code, pos, perror);
        op = command >> 24;
        a = command >> 16;
        b = command >> 8;
        c = command >> 0;
        if (op == 0x00) {
            /* nop */
        }
        else if (op == 0x01) {
            write_reg(b, read_reg(a));
        }
        else if (op == 0x02) {
            write_reg(b, -read_reg(a));
        }
        else if (op == 0x03) {
            flags &= ~0x1;
            flags |= (read_reg(a) == read_reg(b));
            flags &= ~0x2;
            flags |= (read_reg(a) < read_reg(b)) << 1;
            flags &= ~0x4;
            flags |= (read_reg(a) > read_reg(b)) << 2;
        }
        else if (op == 0x04) {
            write_reg(c, read_reg(a) + read_reg(b));
        }
        else if (op == 0x05) {
            write_reg(c, read_reg(a) - read_reg(b));
        }
        else if (op == 0x06) {
            write_reg(c, read_reg(a) * read_reg(b));
        }
        else if (op == 0x07) {
            write_reg(c, read_reg(a) / read_reg(b));
        }
        else if (op == 0x08) {
            write_reg(c, read_reg(a) % read_reg(b));
        }
        else if (op == 0x09) {
            write_reg(c, read_reg(a) & read_reg(b));
        }
        else if (op == 0x0a) {
            write_reg(c, read_reg(a) | read_reg(b));
        }
        else if (op == 0x0b) {
            write_reg(c, read_reg(a) ^ read_reg(b));
        }
        else if (op == 0x0c) {
            write_reg(b, ~read_reg(a));
        }
        else if (op == 0x0d) {
            write_reg(c, ~(read_reg(a) & read_reg(b)));
        }
        else if (op == 0x0e) {
            write_reg(c, ~(read_reg(a) | read_reg(b)));
        }
        else if (op == 0x0f) {
            write_reg(c, ~(read_reg(a) ^ read_reg(b)));
        }
        else if (op == 0x10) {
            write_reg(b, read_reg(a)+1);
        }
        else if (op == 0x11) {
            write_reg(b, read_reg(a)-1);
        }
        else if (op == 0x12) {
            write_reg(c, read_reg(a) << b);
        }
        else if (op == 0x13) {
            write_reg(c, read_reg(a) >> b);
        }
        else if (op == 0x18) {
            write_reg(c, a * 0x100 + b);
        }
        else if (op == 0x19) {
            write_reg(c, pos + a * 0x100 + b);
        }

        else if (op == 0x20) {
            flags &= ~0x1;
        }
        else if (op == 0x21) {
            flags |= 0x1;
        }
        else if (op == 0x22) {
            flags &= ~0x2;
        }
        else if (op == 0x23) {
            flags |= 0x2;
        }
        else if (op == 0x24) {
            flags &= ~0x4;
        }
        else if (op == 0x25) {
            flags |= 0x4;
        }
        else if (op == 0x26) {
            flags &= ~0x8;
        }
        else if (op == 0x27) {
            flags |= 0x8;
        }

        else if (op == 0x28) {
            write_reg(b, create_array(read_reg(a), perror));
        }
        else if (op == 0x29) {
            destroy_array(read_reg(a), perror);
        }
        else if (op == 0x2a) {
            write_reg(b, get_size(read_reg(a), perror));
        }
        else if (op == 0x2b) {
            write_reg(a, code);
        }
        else if (op == 0x2c) {
            /**/
        }
        else if (op == 0x2d) {
            /**/
        }
        else if (op == 0x2e) {
            syscall(a, read_reg(0x40), read_reg(0x50), perror);
        }
        else if (op == 0x2f) {
            /**/
        }

        if ((op & 0x38) == 0x30) {
            if ((op == 0x30) || ((flags & 0x7) & (op & 0x7))) {
                code = read_reg(a);
                pos = read_reg(b);
                continue;
            }
        }

        pos += sizeof(command);
    }
}

void syscall(uint64_t a, uint64_t b, uint64_t c, herror_t* perror)
{
    hword_t* ptr;
    if (a == 0) {
        ptr = get_pointer_unsafe(b, perror);
        ptr += c;
        printf("%s\n", ptr);
    }
    /* TODO: 
     * implement more system calls 
     * rewrite this syscall to get rid of unsafe
     */
}
