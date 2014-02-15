#include <stdio.h>
#include <math.h>
#include "virtual_machine.h"
#include "memory_manager.h"
#include "interpreter.h"

/* TODO:
 * implement more system calls
 * optimize execute_code! that code is horrible
 * floating point operation
 */

#define CHECK_ANY_ERROR \
    if (*perror != E_NONE) {\
        return;\
    }

#define CHECK_READ_PERMISSION(m, a) \
    permission = get_permission(m, a, perror);\
    CHECK_ANY_ERROR\
    if ((*permission & P_READ) == 0) {\
        *perror = E_READ_FORBIDDEN;\
        return;\
    }

#define CHECK_WRITE_PERMISSION(m, a) \
    permission = get_permission(m, a, perror);\
    CHECK_ANY_ERROR\
    if ((*permission & P_WRITE) == 0) {\
        *perror = E_WRITE_FORBIDDEN;\
        return;\
    }

#define CHECK_EXECUTE_PERMISSION(m, a) \
    permission = get_permission(m, a, perror);\
    CHECK_ANY_ERROR\
    if ((*permission & P_EXECUTE) == 0) {\
        *perror = E_EXECUTE_FORBIDDEN;\
        return;\
    }

extern registry_file_t* registry_file;
extern char flags;

void syscall(uint64_t a, uint64_t b, uint64_t c, herror_t* perror);

static inline qword_t read_reg(hword_t reg)
{
    if ((reg & 0xf) == 0xe) {
        return registry_file->regs[reg >> 4];
    }
    else if ((reg & 0xc) == 0xc) {
        return (registry_file->regs[reg >> 4] >> (8 * sizeof(dword_t) * (reg & 1))) & 0xffffffff;
    }
    else if ((reg & 0x8) == 0x8) {
        return (registry_file->regs[reg >> 4] >> (8 * sizeof(sword_t) * (reg & 3))) & 0xffff;
    }
    else {
        return (registry_file->regs[reg >> 4] >> (8 * sizeof(hword_t) * (reg & 7))) & 0xff;
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

void execute_code(module_t* module, herror_t* perror)
{
    hdata_t code;
    index_t pos;
    hpermission_t* permission;
    qword_t qtemp;
    dword_t dtemp;
    sword_t stemp;
    hword_t htemp;
    dword_t array;
    dword_t command;
    hword_t op;
    hword_t a;
    hword_t b;
    hword_t c;

    code = module->code;
    pos = module->entry;
    while ((flags & 0x8) == 0) {
        command = read_dword(code, pos, perror);
        CHECK_ANY_ERROR
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
            qtemp = read_reg(b);
            if (qtemp == 0) {
                *perror = E_DIVBYZERO;
                return;
            }
            write_reg(c, read_reg(a) / qtemp);
        }
        else if (op == 0x08) {
            qtemp = read_reg(b);
            if (qtemp == 0) {
                *perror = E_DIVBYZERO;
                return;
            }
            write_reg(c, read_reg(a) % qtemp);
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

        /* code must be verified */
        else if (op == 0x28) {
            /*write_reg(b, create_array(read_reg(a), perror));*/
            array = create_array(read_reg(a), perror);
            ((dword_t*)(&(registry_file->regs[b >> 4])))[b & 1] = array;
            CHECK_ANY_ERROR
            permission = get_permission(module, array, perror);
            CHECK_ANY_ERROR
            *permission = P_READ | P_READ_TRANSIT | P_WRITE | P_WRITE_TRANSIT;
        }
        else if (op == 0x29) {
            /*destroy_array(read_reg(a), perror);*/
            array = registry_file->regs[a >> 4] >> (8 * sizeof(dword_t) * (a & 1));
            CHECK_WRITE_PERMISSION(module, array)
            destroy_array(array, perror);
            CHECK_ANY_ERROR
        }
        else if (op == 0x2a) {
            /*write_reg(b, get_size(read_reg(a), perror));*/
            array = registry_file->regs[a >> 4] >> (8 * sizeof(dword_t) * (a & 1));
            CHECK_READ_PERMISSION(module, array)
            write_reg(b, get_size(array, perror));
            CHECK_ANY_ERROR
        }
        else if (op == 0x2b) {
            /*write_reg(a, code);*/
            ((dword_t*)(&(registry_file->regs[a >> 4])))[a & 1] = code;
        }
        else if (op == 0x2c) {
            array = registry_file->regs[b >> 4] >> (8 * sizeof(dword_t) * (b & 1));
            CHECK_WRITE_PERMISSION(module, array)
            if ((a & 0xf) == 0xe) {
                qtemp = registry_file->regs[a >> 4];
                write_qword(array, read_reg(c), qtemp, perror);
            }
            else if ((a & 0xc) == 0xc) {
                dtemp = (registry_file->regs[a >> 4] >> (8 * sizeof(dword_t) * (a & 1))) & 0xffffffff;
                write_dword(array, read_reg(c), dtemp, perror);
            }
            else if ((a & 0x8) == 0x8) {
                stemp = (registry_file->regs[a >> 4] >> (8 * sizeof(sword_t) * (a & 3))) & 0xffff;
                write_sword(array, read_reg(c), stemp, perror);
            }
            else {
                htemp = (registry_file->regs[a >> 4] >> (8 * sizeof(hword_t) * (a & 7))) & 0xff;
                write_hword(array, read_reg(c), htemp, perror);
            }
            CHECK_ANY_ERROR
        }
        else if (op == 0x2d) {
            array = registry_file->regs[a >> 4] >> (8 * sizeof(dword_t) * (a & 1));
            CHECK_READ_PERMISSION(module, array)
            if ((c & 0xf) == 0xe) {
                qtemp = read_hword(array, read_reg(b), perror);
                registry_file->regs[c >> 4] = qtemp;
            }
            else if ((c & 0xc) == 0xc) {
                dtemp = read_dword(array, read_reg(b), perror);
                ((dword_t*)(&(registry_file->regs[c >> 4])))[c & 1] = dtemp;
            }
            else if ((c & 0x8) == 0x8) {
                stemp = read_sword(array, read_reg(b), perror);
                ((sword_t*)(&(registry_file->regs[c >> 4])))[c & 3] = stemp;
            }
            else {
                htemp = read_hword(array, read_reg(b), perror);
                ((hword_t*)(&(registry_file->regs[c >> 4])))[c & 7] = htemp;
            }
            CHECK_ANY_ERROR
        }
        else if (op == 0x2e) {
            syscall(a, read_reg(0x4e), read_reg(0x5e), perror);
            CHECK_ANY_ERROR
        }
        else if (op == 0x2f) {
            /**/
        }

        if ((op & 0x38) == 0x30) {
            if ((op == 0x30) || ((flags & 0x7) & (op & 0x7))) {
                code = registry_file->regs[a >> 4] >> (8 * sizeof(dword_t) * (a & 1));
                CHECK_EXECUTE_PERMISSION(module, code)
                pos = read_reg(b);
                continue;
            }
        }

        pos += sizeof(command);
    }
}

void syscall(uint64_t a, uint64_t b, uint64_t c, herror_t* perror)
{
    /* TODO: 
     * implement more system calls 
     * get rid of unsafe
     */
    hword_t* ptr;

    if (a == 0) {
        ptr = get_pointer_unsafe(b, perror);
        ptr += c;
        printf("%s\n", ptr);
    }
}
