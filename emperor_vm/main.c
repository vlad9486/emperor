
/************************************************
    the virtual machine for the emperor project
    vlad9486, 05.02.2014
 ************************************************/

#include <stdlib.h>
#include "virtual_machine.h"

int main(int argc, char* argv[]) {

    initialize_vm();
    loop();
    finalize_vm();

    return EXIT_SUCCESS;
}
