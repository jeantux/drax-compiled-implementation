/* Drax Lang - 2022 */
 
#ifndef __DDEFS
#define __DDEFS

#include <stdint.h>

/* Definitions */

typedef unsigned long d_byte_def;

#define UNUSED(x) (void)(x)

#define UNUSED2(a, b) UNUSED(a); UNUSED(b)

#define UNUSED3(a, b, c) UNUSED2(a, b); UNUSED(c)

#define CAST_DRAX_BYTE(v) (d_byte_def) v

#define CAST_STRING(v)  (char*) v

#define CAST_INT(v)  (int) v

#define CAST_FLOAT(v)  (double) v

#define ASMDFD    "icode"

#define ASMFN1    ASMDFD".s"

#define ASMFO0    ASMDFD".o"

#define ASMLIBS   "bin/libdrax.so" /* Development */

#endif