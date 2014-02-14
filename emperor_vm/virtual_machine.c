#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "memory_manager.h"
#include "virtual_machine.h"

/* TODO:
 * threads
 */

#define ARRAYS_PER_PAGE 0x10000

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

module_t* create_module(hdata_t code, index_t entry, herror_t* perror)
{
    module_t* module;

    module = malloc(sizeof(*module));
    if (module == NULL) {
        *perror = E_OUTOFMEMORY;
        return NULL;
    }
    module->code = code;
    module->entry = entry;
    module->permissions.descr = malloc(ARRAYS_PER_PAGE*sizeof(*(module->permissions.descr)));
    if (module->permissions.descr == NULL) {
        *perror = E_OUTOFMEMORY;
        return NULL;
    }
    module->permissions.next = NULL;

    return module;
}

void destroy_module(module_t* module, herror_t* perror)
{
    permissions_t* permissions;
    permissions_t* temp;

    permissions = module->permissions.next;
    while (permissions) {
        temp = permissions;
        permissions = permissions->next;
        free(temp->descr);
        free(temp);
    }
    free(module->permissions.descr);
    destroy_array(module->code, perror);
    if (*perror != E_NONE) {
        return;
    }
    free(module);
}

sword_t get_permission(module_t* module, hdata_t array, herror_t* perror)
{
    
}
