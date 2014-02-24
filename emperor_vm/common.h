#ifndef COMMON_H
#define	COMMON_H

#include <stdint.h>

/*#define E_NONE              0x00000000
#define E_OUTOFMEMORY       0x00000001
#define E_INVALIDPTR        0x00000002
#define E_ACCESSFORBIDDEN   0x00000003
#define E_DIVBYZERO         0x00000004
#define E_READ_FORBIDDEN    0x00000005
#define E_WRITE_FORBIDDEN   0x00000006
#define E_EXECUTE_FORBIDDEN 0x00000007*/


#define E__NONE                 0x00000000

#define E_MM_OUTOFMEMORY        0x00000001
#define E_MM_INVALIDPTR         0x00000002
#define E_MM_OUTOFDOMAIN        0x00000003
#define E_MM_DISKOPERATION      0x00000004

#define E_I_CODEACCESS          0x00000010
#define E_I_DIVBYZERO           0x00000020
#define E_I_READATTEMPT         0x00000030
#define E_I_WRITEATTEMPT        0x00000040
#define E_I_EXECUTEATTEMPT      0x00000050
#define E_I_CREATEARRAY         0x00000060
#define E_I_DESTROYARRAY        0x00000070
#define E_I_ACCESSTABLE         0x00000080
#define E_I_ACCESSARRAY         0x00000090
#define E_I_SYSTEM              0x000000A0

#define FALSE 0
#define TRUE 1

typedef uint32_t hdata_t;
typedef uint32_t herror_t;
typedef uint64_t esize_t;
typedef uint8_t hword_t;
typedef uint16_t sword_t;
typedef uint32_t dword_t;
typedef uint64_t qword_t;
typedef uint64_t index_t;
typedef char bool_t;

#endif	/* COMMON_H */
