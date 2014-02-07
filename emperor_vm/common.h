#ifndef COMMON_H
#define	COMMON_H

#include <stdint.h>

#define E_NONE          0x00000000
#define E_OUTOFMEMORY   0x00000001
#define E_INVALIDPTR    0x00000002

#define FALSE 0
#define TRUE 1

typedef uint32_t hdata_t;
typedef uint32_t herror_t;
typedef uint32_t esize_t;
typedef uint32_t word_t;
typedef uint64_t index_t;
typedef void* ptr_t;
typedef char bool_t;

#endif	/* COMMON_H */
