/* Drax Lang - 2022 */

#ifndef __DARCH_x86_64
#define __DARCH_x86_64

#include "../dlowcode.h"

#define FL            "\n"
#define SPACE         " "

#define SD_GNU_STR    " \"%s\\n\""
#define DNUM_ASM_REPR "$%lu"
#define SD_GNU_ENT    "_start"

#define SDCALL_WRITE  "mov $1, %%eax"
#define SDCALL_STDOUT "mov $1, %%edi"
#define SDCALL_EXIT   "mov $1, %%eax"
#define SDCODE_RETURN "xor %%edi, %%edi"
#define SDSYSCALL     "int  $0x80"FL

#define GAS_ASCII     ".asciz"
#define GAS_DATA      ".data"
#define GAS_TEXT      ".text"
#define GAS_GLOBAL    ".global"

#define DMC(v)        v SPACE

int dx_x86_64_data_section();

int dx_x86_64_text_section();

int dx_x86_64_exit();

int get_asm_code(dline_cmd* v) ;

#endif