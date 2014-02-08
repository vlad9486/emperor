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
 */

extern hdata_t main_module;
extern registry_file_t* registry_file;
extern char flags;

void syscall(uint64_t a, herror_t* perror);

void execute_code(hdata_t code, index_t pos, herror_t* perror)
{
    index_t size;
    word_t command;
    bool_t read;
    unsigned char op;
    unsigned char a;
    unsigned char b;
    unsigned char c;
    index_t temp;

    read = FALSE;
    size = get_size(code, perror);
    while ((flags & 0x8) == 0) {
        command = read_word(code, pos, perror); /* TODO: cache it */
        op = command >> 24;
        a = command >> 16;
        b = command >> 8;
        c = command;

        if (op == 0x00) {
            registry_file->int_[b] = registry_file->int_[a];
        }
        if (op == 0x01) {
            registry_file->int_[b] = registry_file->long_[a];
        }
        if (op == 0x02) {
            registry_file->int_[b] = trunc(registry_file->float_[a]);
        }
        if (op == 0x03) {
            registry_file->int_[b] = trunc(registry_file->double_[a]);
        }
        if (op == 0x04) {
            registry_file->long_[b] = registry_file->int_[a];
        }
        if (op == 0x05) {
            registry_file->long_[b] = registry_file->long_[a];
        }
        if (op == 0x06) {
            registry_file->long_[b] = trunc(registry_file->float_[a]);
        }
        if (op == 0x07) {
            registry_file->long_[b] = trunc(registry_file->double_[a]);
        }
        if (op == 0x08) {
            registry_file->float_[b] = registry_file->int_[a];
        }
        if (op == 0x09) {
            registry_file->float_[b] = registry_file->long_[a];
        }
        if (op == 0x0a) {
            registry_file->float_[b] = registry_file->float_[a];
        }
        if (op == 0x0b) {
            registry_file->float_[b] = registry_file->double_[a];
        }
        if (op == 0x0c) {
            registry_file->double_[b] = registry_file->int_[a];
        }
        if (op == 0x0d) {
            registry_file->double_[b] = registry_file->long_[a];
        }
        if (op == 0x0e) {
            registry_file->double_[b] = registry_file->float_[a];
        }
        if (op == 0x0f) {
            registry_file->double_[b] = registry_file->double_[a];
        }

        if (op == 0x10) {
            registry_file->int_[c] = registry_file->int_[a]+registry_file->int_[b];
        }
        if (op == 0x11) {
            registry_file->long_[c] = registry_file->long_[a]+registry_file->long_[b];
        }
        if (op == 0x12) {
            registry_file->float_[c] = registry_file->float_[a]+registry_file->float_[b];
        }
        if (op == 0x13) {
            registry_file->double_[c] = registry_file->double_[a]+registry_file->double_[b];
        }
        if (op == 0x14) {
            registry_file->int_[c] = registry_file->int_[a]-registry_file->int_[b];
        }
        if (op == 0x15) {
            registry_file->long_[c] = registry_file->long_[a]-registry_file->long_[b];
        }
        if (op == 0x16) {
            registry_file->float_[c] = registry_file->float_[a]-registry_file->float_[b];
        }
        if (op == 0x17) {
            registry_file->double_[c] = registry_file->double_[a]-registry_file->double_[b];
        }
        if (op == 0x18) {
            registry_file->int_[c] = registry_file->int_[a]*registry_file->int_[b];
        }
        if (op == 0x19) {
            registry_file->long_[c] = registry_file->long_[a]*registry_file->long_[b];
        }
        if (op == 0x1a) {
            registry_file->float_[c] = registry_file->float_[a]*registry_file->float_[b];
        }
        if (op == 0x1b) {
            registry_file->double_[c] = registry_file->double_[a]*registry_file->double_[b];
        }
        if (op == 0x1c) {
            registry_file->int_[c] = registry_file->int_[a]/registry_file->int_[b];
        }
        if (op == 0x1d) {
            registry_file->long_[c] = registry_file->long_[a]/registry_file->long_[b];
        }
        if (op == 0x1e) {
            registry_file->float_[c] = registry_file->float_[a]/registry_file->float_[b];
        }
        if (op == 0x1f) {
            registry_file->double_[c] = registry_file->double_[a]/registry_file->double_[b];
        }

        if (op == 0x20) {
            registry_file->int_[b] = -registry_file->int_[a];
        }
        if (op == 0x21) {
            registry_file->long_[b] = -registry_file->long_[a];
        }
        if (op == 0x22) {
            registry_file->float_[b] = -registry_file->float_[a];
        }
        if (op == 0x23) {
            registry_file->double_[b] = -registry_file->double_[a];
        }
        if (op == 0x24) {
            flags &= 0xe;
            flags |= (registry_file->int_[b] == -registry_file->int_[a]);
            flags &= 0xd;
            flags |= (registry_file->int_[b] < -registry_file->int_[a]);
            flags &= 0xb;
            flags |= (registry_file->int_[b] > -registry_file->int_[a]);
        }
        if (op == 0x25) {
            flags &= 0xe;
            flags |= (registry_file->long_[b] == -registry_file->long_[a]);
            flags &= 0xd;
            flags |= (registry_file->long_[b] < -registry_file->long_[a]);
            flags &= 0xb;
            flags |= (registry_file->long_[b] > -registry_file->long_[a]);
        }
        if (op == 0x26) {
            flags &= 0xe;
            flags |= (registry_file->float_[b] == -registry_file->float_[a]);
            flags &= 0xd;
            flags |= (registry_file->float_[b] < -registry_file->float_[a]);
            flags &= 0xb;
            flags |= (registry_file->float_[b] > -registry_file->float_[a]);
        }
        if (op == 0x27) {
            flags &= 0xe;
            flags |= (registry_file->double_[b] == -registry_file->double_[a]);
            flags &= 0xd;
            flags |= (registry_file->double_[b] < -registry_file->double_[a]);
            flags &= 0xb;
            flags |= (registry_file->double_[b] > -registry_file->double_[a]);
        }
        if (op == 0x28) {
            registry_file->int_[b] = registry_file->int_[a]+1;
        }
        if (op == 0x29) {
            registry_file->long_[b] = registry_file->long_[a]+1;
        }
        if (op == 0x2a) {
            registry_file->int_[b] = registry_file->int_[a]-1;
        }
        if (op == 0x2b) {
            registry_file->long_[b] = registry_file->long_[a]-1;
        }
        if (op == 0x2c) {
            registry_file->int_[c] = registry_file->int_[a] << b;
        }
        if (op == 0x2d) {
            registry_file->long_[c] = registry_file->long_[a] << b;
        }
        if (op == 0x2e) {
            registry_file->int_[c] = registry_file->int_[a] >> b;
        }
        if (op == 0x2f) {
            registry_file->long_[c] = registry_file->long_[a] >> b;
        }

        if (op == 0x30) {
            registry_file->int_[b] = ~registry_file->int_[a];
        }
        if (op == 0x31) {
            registry_file->long_[b] = ~registry_file->long_[a];
        }
        if (op == 0x32) {
            registry_file->int_[c] = ~(registry_file->int_[a] & registry_file->int_[b]);
        }
        if (op == 0x33) {
            registry_file->long_[c] = ~(registry_file->long_[a] & registry_file->long_[b]);
        }
        if (op == 0x34) {
            registry_file->int_[c] = ~(registry_file->int_[a] | registry_file->int_[b]);
        }
        if (op == 0x35) {
            registry_file->long_[c] = ~(registry_file->long_[a] | registry_file->long_[b]);
        }
        if (op == 0x36) {
            registry_file->int_[c] = ~(registry_file->int_[a] ^ registry_file->int_[b]);
        }
        if (op == 0x37) {
            registry_file->long_[c] = ~(registry_file->long_[a] ^ registry_file->long_[b]);
        }
        if (op == 0x38) {
            registry_file->int_[c] = registry_file->int_[a] % registry_file->int_[b];
        }
        if (op == 0x39) {
            registry_file->long_[c] = registry_file->long_[a] % registry_file->long_[b];
        }
        if (op == 0x3a) {
            registry_file->int_[c] = registry_file->int_[a] & registry_file->int_[b];
        }
        if (op == 0x3b) {
            registry_file->long_[c] = registry_file->long_[a] & registry_file->long_[b];
        }
        if (op == 0x3c) {
            registry_file->int_[c] = registry_file->int_[a] | registry_file->int_[b];
        }
        if (op == 0x3d) {
            registry_file->long_[c] = registry_file->long_[a] | registry_file->long_[b];
        }
        if (op == 0x3e) {
            registry_file->int_[c] = registry_file->int_[a] ^ registry_file->int_[b];
        }
        if (op == 0x3f) {
            registry_file->long_[c] = registry_file->long_[a] ^ registry_file->long_[b];
        }

        if (op == 0x40) {
            flags &= 0xe;
        }
        if (op == 0x41) {
            flags |= 0x1;
        }
        if (op == 0x42) {
            flags &= 0xd;
        }
        if (op == 0x43) {
            flags |= 0x2;
        }
        if (op == 0x44) {
            flags &= 0xb;
        }
        if (op == 0x45) {
            flags |= 0x4;
        }
        if (op == 0x46) {
            flags &= 0x7;
        }
        if (op == 0x47) {
            flags |= 0x8;
        }
        if (op == 0x48) {
            registry_file->int_[b] = create_array(registry_file->long_[a], perror);
        }
        if (op == 0x49) {
            free_array(registry_file->int_[a], perror);
        }
        if (op == 0x4a) {
            registry_file->long_[b] = get_size(registry_file->int_[a], perror);
        }
        if (op == 0x4b) {
            registry_file->int_[c] = pos+a*0x100+b;
        }
        if (op == 0x4c) {
            registry_file->int_[a] = code;
        }
        if (op == 0x4d) {
            syscall(registry_file->long_[a], perror);
        }
        if (op == 0x4e);
        if (op == 0x4f);

        if (op == 0x50) {
            write_word(registry_file->int_[c], registry_file->long_[b],
                    registry_file->int_[a], perror);
        }
        if (op == 0x51) {
            write_word(registry_file->int_[c], registry_file->long_[b],
                    registry_file->long_[a] / (uint64_t)0x100000000, perror);
            write_word(registry_file->int_[c], registry_file->long_[b]+1,
                    registry_file->long_[a], perror);
        }
        if (op == 0x52) {
            write_word(registry_file->int_[c], registry_file->long_[b],
                    registry_file->float_[a], perror);
        }
        if (op == 0x53) {
            write_word(registry_file->int_[c], registry_file->long_[b],
                    (uint64_t)(registry_file->long_[a]) / (uint64_t)0x100000000, perror);
            write_word(registry_file->int_[c], registry_file->long_[b]+1,
                    (uint64_t)(registry_file->long_[a]), perror);
        }
        if (op == 0x54) {
            registry_file->int_[c] = read_word(registry_file->int_[a],
                    registry_file->long_[b], perror);
        }
        if (op == 0x55) {
            registry_file->long_[c] = read_word(registry_file->int_[a],
                    registry_file->long_[b], perror);
            registry_file->long_[c] *= (uint64_t)0x100000000;
            registry_file->long_[c] += read_word(registry_file->int_[a],
                    registry_file->long_[b]+1, perror);
        }
        if (op == 0x56) {
            registry_file->float_[c] = read_word(registry_file->int_[a],
                    registry_file->long_[b], perror);
        }
        if (op == 0x57) {
            temp = read_word(registry_file->int_[a],
                    registry_file->long_[b], perror);
            temp *= (uint64_t)0x100000000;
            temp += read_word(registry_file->int_[a],
                    registry_file->long_[b]+1, perror);
            registry_file->double_[c] = (double)temp;
        }
        if ((op & 0x58) == 0x58) {
            if ((op == 0x58) || (flags & (op & 0x7))) {
                code = registry_file->int_[a];
                pos = registry_file->long_[b];
            }
        }

        if ((op & 0x58) != 0x58) {
            pos++;
        }
    }
}

void syscall(uint64_t a, herror_t* perror)
{
    word_t* ptr;
    if (a == 0) {
        ptr = get_pointer(registry_file->int_[4], perror);
        ptr += registry_file->long_[5];
        printf("%s\n", ptr);
    }
    /* TODO: implement more system calls */
}
