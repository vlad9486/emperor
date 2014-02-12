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
    while ((flags & 0x8) == 0) {

    }
}

void syscall(uint64_t a, uint64_t b, uint64_t c, herror_t* perror)
{
    word_t* ptr;
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
