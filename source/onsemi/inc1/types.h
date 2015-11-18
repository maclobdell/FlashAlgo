#ifndef _types_h
#define _types_h

#include <stdint.h>

typedef  unsigned       char   u8_t;
typedef    signed       char   i8_t;
typedef  unsigned      short  u16_t;
typedef    signed      short  i16_t;
typedef  unsigned       long  u32_t;
typedef    signed       long  i32_t;
typedef  unsigned  long long  u64_t;
typedef    signed  long long  i64_t;
typedef unsigned char boolean;

#define ASSERT(test)		((void)(test))

#define True		(1)
#define False		(0)
#define Null		(0)


#endif