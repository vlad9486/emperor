#ifndef VIRTUAL_MACHINE_H
#define	VIRTUAL_MACHINE_H

#include "common.h"

typedef struct registry_file_tag {
    uint64_t regs[0x10];
} registry_file_t;

void initialize_vm(const char* fn);
void finalize_vm();
void loop();

#endif	/* VIRTUAL_MACHINE_H */
