#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "memory_manager.h"
#include "virtual_machine.h"

/* TODO:
 * threads
 */

#define BIT_PER_PAGE 16
#define ARRAYS_PER_PAGE (1 << BIT_PER_PAGE)

herror_t error;
module_t* main_module;

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
    hdata_t main_code;
    hpermission_t* permission;

    registry_file = malloc(sizeof(registry_file_t));
    memset(registry_file, 0, sizeof(*registry_file));
    main_code = bind_file(fn, &error);
    print_error("when loading main_code", &error);
    main_module = create_module(main_code, 0, &error);
    print_error("when creating main_module", &error);
    permission = get_permission(main_module, main_code, &error);
    print_error("when ask access to permission table", &error);
    *permission |= P_READ | P_READ_TRANSIT | 
        P_WRITE | P_WRITE_TRANSIT | P_EXECUTE | P_EXECUTE_TRANSIT;
}

void finalize_vm()
{
    destroy_module(main_module, &error);
    print_error("when deleting main_module", &error);
    free(registry_file);
}

void loop()
{
    flags = 0;
    execute_code(main_module, &error);
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
        if (temp->descr != NULL) {
            free(temp->descr);
        }
        free(temp);
    }
    free(module->permissions.descr);
    destroy_array(module->code, perror);
    if (*perror != E_NONE) {
        return;
    }
    free(module);
}

hpermission_t* get_permission(module_t* module, hdata_t array, herror_t* perror)
{
    dword_t head;
    dword_t tail;
    permissions_t* page;

    head = array & (ARRAYS_PER_PAGE-1);
    tail = array >> BIT_PER_PAGE;

    page = &(module->permissions);
    while (tail != 0) {
        if (page->next == NULL) {
            page->next = malloc(sizeof(*page));
            if (page->next == NULL) {
                *perror = E_OUTOFMEMORY;
                return NULL;
            }
            page->next->descr = malloc(ARRAYS_PER_PAGE*sizeof(*(module->permissions.descr)));
            if (module->permissions.descr == NULL) {
                *perror = E_OUTOFMEMORY;
                return NULL;
            }
        }
        page = page->next;
        tail--;
    }

    return &(page->descr[head]);
}
