#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "memory_manager.h"
#include "virtual_machine.h"

/* TODO:
 * threads
 */

herror_t error;
hdata_t main_module;

registry_file_t* registry_file;
char flags;

void print_error(const char* msg, herror_t* perror)
{
    if (*perror == E_NONE) {
        return;
    }
    if (*perror == E_OUTOFMEMORY) {
        printf("[ERROR]: \"out of memory\" %s\n", msg);
    }
    if (*perror == E_INVALIDPTR) {
        printf("[ERROR]: \"invalid pointer\" %s\n", msg);
    }
    if (*perror == E_ACCESSFORBIDDEN) {
        printf("[ERROR]: \"access is forbidden\" %s\n", msg);
    }
}

void initialize_vm(const char* fn)
{
    registry_file = malloc(sizeof(registry_file_t));
    main_module = load_data(fn, &error);
    print_error("when loading module", &error);
}

void finalize_vm()
{
    free_array(main_module, &error);
    print_error("when deleting main_module", &error);
    free(registry_file);
}

void loop()
{
    flags = 0;
    execute_code(main_module, 0, &error);
    print_error("when execute", &error);
}
