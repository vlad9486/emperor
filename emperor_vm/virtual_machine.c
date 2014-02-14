#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "memory_manager.h"
#include "virtual_machine.h"

/* TODO:
 * threads
 */

#define ARRAYS_PER_PAGE 0x400

typedef struct permissions_tag {
    hdata_t* arrays;
    sword_t* descr;
    struct permissions_tag* next;
    sword_t number;
    sword_t deepness;
} permissions_t;

typedef struct module_tag {
    hdata_t code;
    index_t entry;
    permissions_t permissions;
} module_t;

herror_t error;
hdata_t main_module;

registry_file_t* registry_file;
char flags;

const char* error_descr[] = {
    "\"no error\"",
    "\"out of memory\"",
    "\"invalid pointer\"",
    "\"access is forbidden\"",
    "\"division by zero\"",
};

void print_error(const char* msg, herror_t* perror)
{
    if (*perror == E_NONE) {
        return;
    }
    else {
        printf("[ERROR]: %s %s\n", error_descr[*perror], msg);
    }
}

void initialize_vm(const char* fn)
{
    registry_file = malloc(sizeof(registry_file_t));
    memset(registry_file, 0, sizeof(*registry_file));
    main_module = bind_file(fn, &error);
    print_error("when loading module", &error);
}

void finalize_vm()
{
    destroy_array(main_module, &error);
    print_error("when deleting main_module", &error);
    free(registry_file);
}

void loop()
{
    flags = 0;
    execute_code(main_module, 0, &error);
    print_error("when execute", &error);
}

module_t* create_module(hdata_t code, index_t entry)
{
    module_t* module;

    module = malloc(sizeof(*module));
    module->code = code;
    module->entry = entry;
    module->permissions.arrays = malloc(ARRAYS_PER_PAGE*sizeof(*(module->permissions.arrays)));
    module->permissions.descr = malloc(ARRAYS_PER_PAGE*sizeof(*(module->permissions.descr)));
    module->permissions.next = NULL;

    return module;
}
