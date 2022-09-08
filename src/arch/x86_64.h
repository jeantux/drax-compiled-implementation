/* Drax Lang - 2022 */

#ifndef __DARCH_x86_64
#define __DARCH_x86_64

#include "../dlowcode.h"

#define FL            "\n"
#define SD_GNU_STR    " \"%s\\n\""
#define SD_GNU_ENT    "_start"

#define SDCALL_WRITE  "mov $1,    %%eax"
#define SDCALL_STDOUT "mov $1,    %%edi"
#define SDCALL_EXIT   "mov $60,   %%eax"
#define SDCODE_RETURN "xor %%edi, %%edi"
#define SDSYSCALL     "syscall"FL

#define GAS_ASCII     ".asciz"
#define GAS_DATA      ".data"
#define GAS_TEXT      ".text"
#define GAS_GLOBAL    ".global"

int dx_x86_64_data_section();

int dx_x86_64_text_section();

int dx_x86_64_start();

int dx_x86_64_exit();

int get_asm_code(dline_pair* v) ;

#endif