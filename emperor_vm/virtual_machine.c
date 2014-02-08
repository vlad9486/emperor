#include <stdlib.h>
#include "interpreter.h"
#include "memory_manager.h"
#include "virtual_machine.h"

herror_t error;
hdata_t main_module;
hdata_t system_module;

registry_file_t* registry_file;

void initialize_vm()
{
    registry_file = malloc(sizeof(registry_file_t));
    system_module = create_array(0, &error);
    main_module = load_data("../emperor_asm/program.b", &error);
}

void finalize_vm()
{
    free_array(main_module, &error);
    free_array(system_module, &error);
    free(registry_file);
}

void loop()
{
    execute_code(main_module, &error);
}
