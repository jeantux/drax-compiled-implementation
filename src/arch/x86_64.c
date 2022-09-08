
/* Drax Lang - 2022 */
#include <string.h>

#include "x86_64.h"
#include "../dlowcode.h"
#include "../dgen.h"
#include "../ddefs.h"

int dx_x86_64_data_section() {
  return df_asm_gen(GAS_DATA FL);
}

int dx_x86_64_text_section() {
  return df_asm_gen(GAS_TEXT FL);
}

int dx_x86_64_start() {
  return df_asm_gen(GAS_GLOBAL " " SD_GNU_ENT FL SD_GNU_ENT ":" FL);
}

int dx_x86_64_exit() {
  return df_asm_gen(
    SDCALL_EXIT   FL \
    SDCODE_RETURN FL \
    SDSYSCALL
  );
}

static int dx_PUTS(dline_pair* e) {
  df_asm_gen("mov $%s, %%esi\n",e->command);
  df_asm_gen("call dsys_out\n");

  return 0;
}

static int dx_const(dline_pair* e) {
  return df_asm_gen(GAS_ASCII SD_GNU_STR FL, e->command);
}

static int dx_label(dline_pair* e) {
  return df_asm_gen("%s: ", e->command);
}

int get_asm_code(dline_pair* v) {
  switch (v->op) {
    case DOP_MRK_ID:
    case DOP_LABEL: return dx_label(v);
    case DOP_MOV: return df_asm_gen("mov ");
    case DOP_EXIT: return dx_x86_64_exit(v);
    case DOP_CONST: return dx_const(v);
    case DOP_PUTS: return dx_PUTS(v);
  
    default: return 0;
  }

  return 0;
}
