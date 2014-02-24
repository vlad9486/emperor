#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "common.h"
#include "virtual_machine.h"

void execute_code(module_t* module, herror_t* perror);

#endif	/* INTERPRETER_H */
