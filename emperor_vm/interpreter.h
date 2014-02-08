#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "common.h"

void execute_code(hdata_t code, index_t pos, herror_t* perror);

#endif	/* INTERPRETER_H */
