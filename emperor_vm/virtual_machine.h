#ifndef VIRTUAL_MACHINE_H
#define	VIRTUAL_MACHINE_H

#include "common.h"

typedef struct registry_file_tag {
    uint64_t regs[0x10];
    uint8_t type[0x10];
} registry_file_t;

typedef sword_t hpermission_t;

typedef struct permissions_tag {
    hpermission_t* descr;
    struct permissions_tag* next;
    sword_t number;
    sword_t deepness;
} permissions_t;

typedef struct module_tag {
    hdata_t code;
    index_t entry;
    permissions_t permissions;
} module_t;

void initialize_vm(const char* fn);
void finalize_vm();
void loop();
module_t* create_module(hdata_t code, index_t entry, herror_t* perror);
void destroy_module(module_t* module, herror_t* perror);

#endif	/* VIRTUAL_MACHINE_H */
