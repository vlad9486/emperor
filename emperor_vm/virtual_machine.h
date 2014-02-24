#ifndef VIRTUAL_MACHINE_H
#define	VIRTUAL_MACHINE_H

#include "common.h"

typedef struct registry_file_tag {
    uint64_t regs[0x10];
    uint8_t type[0x10];
} registry_file_t;

typedef hword_t hpermission_t;

#define P_READ              (1 << 0)
#define P_READ_TRANSIT      (1 << 1)
#define P_WRITE             (1 << 2)
#define P_WRITE_TRANSIT     (1 << 3)
#define P_EXECUTE           (1 << 2)
#define P_EXECUTE_TRANSIT   (1 << 3)

typedef struct permissions_tag {
    hpermission_t* descr;
    struct permissions_tag* next;
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
hpermission_t* get_permission(module_t* module, hdata_t array, herror_t* perror);

#endif	/* VIRTUAL_MACHINE_H */
