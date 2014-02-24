
/************************************************
 *  the virtual machine for the emperor project
 *  vlad9486, 05.02.2014
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "virtual_machine.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("[ERROR]: too few parameters\n");
        return EXIT_FAILURE;
    }
    initialize_vm(argv[1]);
    loop();
    finalize_vm();

    return EXIT_SUCCESS;
}
