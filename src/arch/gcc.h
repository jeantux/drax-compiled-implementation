/* Drax Lang - 2022 */

#ifndef __DGCC_MODE
#define __DGCC_MODE
#ifdef __DRAX_BACKEND_GCC

#include "../dlowcode.h"

#define FL            "\n"
#define SPACE         " "

#define SD_GNU_STR       " \"%s\\n\""
#define DNUM_SIMPLE_REPR " %lu"
#define DNUM_ASM_REPR    " $%lu"
#define SD_GNU_ENT       "main"

/* Syscalls */
#define SDSYSCALL     "int  $0x80"FL
#define SDCALL_EXIT   "mov $1, %%rax"
#define SDCALL_STDOUT "mov $1, %%rcx"
#define SDCODE_RETURN "xor %%rbx, %%rbx"
#define SDCALL_WRITE  "mov $4, %%rax"

#define GAS_ASCII     ".asciz"
#define GAS_LONG      ".long"

#define GAS_BSS       ".bss"
#define GAS_DATA      ".data"
#define GAS_TEXT      ".text"
#define GAS_GLOBAL    ".global"

#define GCCCOMPILER "gcc "
#define GCC_AS_ARGS " -no-pie -fno-pie "

#define DMC(v)        v SPACE

int dx_init_bss_section();

int dx_init_data_section();

int dx_init_text_section();

int dx_init_exit();

char* get_ln_cmd(const char* name);

int get_asm_code(dline_cmd* v) ;

#endif
#endif
