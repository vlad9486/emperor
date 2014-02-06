
/************************************************
    the virtual machine for the emperor project
    vlad9486, 05.02.2014
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "interpreter.h"

herror_t error;
hdata_t main_module;

int main(int argc, char* argv[]) {

    main_module = load_data("../emperor_asm/program.b", &error);
    execute_code(main_module);

    return EXIT_SUCCESS;
}
