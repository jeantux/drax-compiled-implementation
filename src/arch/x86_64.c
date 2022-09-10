
/* Drax Lang - 2022 */
#include <string.h>
#include <stdlib.h>

#include "x86_64.h"
#include "../dlowcode.h"
#include "../dgen.h"
#include "../ddefs.h"

static const char* dxasm_reg_table(dlcode_register t) {
  switch (t) {
    case DRG_RX0: return DMC("%%eax");
    case DRG_RX1: return DMC("%%ebx");
    case DRG_RX2: return DMC("%%ecx");
    case DRG_RX3: return DMC("%%edx");
    case DRG_RX4: return DMC("%%esi");
    case DRG_RX5: return DMC("%%edi");
    case DRG_RX6: return DMC("%%esp");
    case DRG_RX7: return DMC("%%ebp");

    default: return "";
  }
}

static const char* dxasm_cmd_table(dlcode_op t) {
  switch (t) {
    case DOP_ADD:  return DMC("add");
    case DOP_SUB:  return DMC("sub");
    case DOP_MUL:  return DMC("mul");
    case DOP_DIV:  return DMC("div");
    case DOP_MOV:  return DMC("mov");
    case DOP_PUSH: return DMC("push");
    case DOP_POP:  return DMC("pop");
    case DOP_CALL: return DMC("call");

    default: return "";
  }
}

int dx_x86_64_data_section() {
  return df_asm_gen(GAS_DATA FL);
}

int dx_x86_64_text_section() {
  return df_asm_gen(GAS_TEXT FL);
}

int dx_x86_64_start() {
  return df_asm_gen(
    GAS_GLOBAL SPACE SD_GNU_ENT FL \
    SD_GNU_ENT ":" FL
  );
}

int dx_x86_64_exit() {
  return df_asm_gen(
    SDCALL_EXIT   FL \
    SDCODE_RETURN FL \
    SDSYSCALL
  );
}

static int dx_x86_64_puts(dline_cmd* e) {
  df_asm_gen("mov $%s, %%esi\n", CAST_STRING(e->value));
  df_asm_gen("call dsys_out\n");

  return 0;
}

static int dx_x86_64_const(dline_cmd* e) {
  return df_asm_gen(GAS_ASCII SD_GNU_STR FL, CAST_STRING(e->value));
}

static int dx_label(dline_cmd* e) {
  return df_asm_gen("%s: ", CAST_STRING(e->value));
}

static int dx_x86_64_mov(dline_cmd* e) {
  // mov fixed value
  char* var;
  asprintf(&var, "%lu", e->value);

  df_asm_gen(dxasm_cmd_table(e->op));
  df_asm_gen(var);
  df_asm_gen(",");
  df_asm_gen(dxasm_reg_table(e->rg));

  df_asm_gen(FL);

  return 0;
}

static int dx_x86_64_arith(dline_cmd* e) {
  // mov fixed value
  char* var;
  asprintf(&var, "%lu", e->value);

  df_asm_gen(dxasm_cmd_table(e->op));
  df_asm_gen(var);
  df_asm_gen(",");
  df_asm_gen(dxasm_reg_table(e->rg));

  df_asm_gen(FL);

  return 0;
}

int get_asm_code(dline_cmd* v) {
  switch (v->op) {
    case DOP_MRK_ID:
    case DOP_LABEL: return dx_label(v);
    case DOP_MOV: return dx_x86_64_mov(v);
    case DOP_ADD:
    case DOP_SUB:
    case DOP_MUL:
    case DOP_DIV: return dx_x86_64_arith(v);
    case DOP_EXIT: return dx_x86_64_exit(v);
    case DOP_CONST: return dx_x86_64_const(v);
    case DOP_PUTS: return dx_x86_64_puts(v);
  
    default: return 0;
  }

  return 0;
}
