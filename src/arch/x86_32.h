/* Drax Lang - 2022 */

#ifndef __DARCH_x86_32
#define __DARCH_x86_32

#include "../dlowcode.h"

#define FL            "\n"
#define SPACE         " "

#define SD_GNU_STR    " \"%s\\n\""
#define DNUM_ASM_REPR "$%lu"
#define SD_GNU_ENT    "_start"

/* Syscalls */
#define SDSYSCALL     "int  $0x80"FL
#define SDCALL_EXIT   "mov $1, %%eax"
#define SDCALL_STDOUT "mov $1, %%ecx"
#define SDCODE_RETURN "xor %%ebx, %%ebx"
#define SDCALL_WRITE  "mov $4, %%eax"

#define GAS_ASCII     ".asciz"
#define GAS_DATA      ".data"
#define GAS_TEXT      ".text"
#define GAS_GLOBAL    ".global"

#define DMC(v)        v SPACE

int dx_x86_32_data_section();

int dx_x86_32_text_section();

int dx_x86_32_exit();

int get_asm_code(dline_cmd* v) ;

#endif
