/* Drax Lang - 2022 */
 
#ifndef __DDEFS
#define __DDEFS

#include <stdint.h>

/* Definitions */

typedef unsigned long d_byte_def;

#define UNUSED(x) (void)(x)

#define UNUSED2(a, b) UNUSED(a); UNUSED(b)

#define UNUSED3(a, b, c) UNUSED2(a, b); UNUSED(c)

#define CAST_STRING(v)  (char*) v

#define ASMFN1    "icode.s"

#define ASMFO0    "icode.o"

#define ASMLD     "ld"

#define ASMLIBS   "bin/libdrax.so" /* Development */

#define DOF_DATA  ".ddata"

#define DOF_TEXT  ".dtext"

#define DOF_START ".dstart"

#endif