/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcompiler.h"
#include "dlowcode.h"
#include "dgen.h"
#include "ddefs.h"
#include "arch/backend.h"

#define DCSwitch() switch (gcA->op[gcA->pc++])

#define DCCase(t) case t:

#define DPUSH(l, t, r, v)   push_line_op(l, new_line_cmd(t, r, (d_byte_def) v));

#define GET_PREV_VAL(_idx)  gcA->val[gcA->pc - (1 + _idx)]

#define GET_STRING_VAL()    (CAST_STRING(GET_PREV_VAL(0)))

int dg_const_def = 0;
dlcode_state* glcs;
d_ast* gcA;

static char* get_ln_cmd(const char* name) {
  const char* fcmd = ASMLD " -s "ASMFO0 " "ASMLIBS " -o ";
  int sz = strlen(fcmd) + strlen(name) + 1;
  char* cmd = (char*) calloc(sizeof(char), sz);

  strcat(cmd, fcmd);
  strcat(cmd, name);
  cmd[sz] = '\0';
  return cmd;
}

/* Stack Helpers */

#define AST_SIZE 1024

d_ast* new_d_ast() {
  d_ast* v = (d_ast*) malloc(sizeof(d_ast));
  v->len = 0;
  v->pc = 0;
  v->cap = AST_SIZE;
  v->op = (d_ast_op*) malloc(sizeof(d_ast_op) * AST_SIZE);
  v->val = (d_byte_def*) malloc(sizeof(d_byte_def) * AST_SIZE);
  return v;
}

int push_d_ast(d_ast* v, d_ast_op op, d_byte_def val) {
  v->len++;
  v->op[v->len -1] = op;
  v->val[v->len -1] = val;
  return 0;
}

/* Compiler Functions */

/**
 * Define const on section data
 */
static int dc_puts_data(dlines_cmd* v, char* name, char* value) {
  DPUSH(v, DOP_MRK_ID, DRG_NONE, name);
  DPUSH(v, DOP_CONST,  DRG_NONE, value);
  return 0;
}
/**
 * Print String definition
 * 
 * Using Data section
 */
static int dc_puts_instruction(dlines_cmd* v, char* var) {
  DPUSH(v, DOP_PUTS, DRG_NONE, var);
  return 0;
}

static int dc_puts_str(const char* var, char* content) {
  dc_puts_data(glcs->data_section, (char*) var, content);
  dc_puts_instruction(glcs->start_global, (char*) var);
  return 0;
}

static void dc_puts() {
  dg_const_def++;
  char* var;
  asprintf(&var, "DV_%i", dg_const_def);
  dc_puts_str(var, GET_STRING_VAL());
}

static void dc_arith_op(dlcode_op t_op) {
  DPUSH(glcs->start_global, DOP_MOV, DRG_RX0, GET_PREV_VAL(2));
  DPUSH(glcs->start_global, t_op,    DRG_RX0, GET_PREV_VAL(1));
}

/**
 * Main Compiler Process
 * 
 */
static int compiler_process() {
  gcA->pc = 0;
  while (gcA->pc < gcA->len) {
    DCSwitch() {
      DCCase(DAT_ADD) {
        dc_arith_op(DOP_ADD);
        break;
      }
      DCCase(DAT_SUB) {
        dc_arith_op(DOP_SUB);
        break;
      }
      DCCase(DAT_MUL) {
        dc_arith_op(DOP_MUL);
        break;
      }
      DCCase(DAT_DIV) {
        dc_arith_op(DOP_DIV);
        break;
      }
      DCCase(DAT_CALL) {
        break;
      }
      DCCase(DAT_CONST) {
        break;
      }
      DCCase(DAT_PUTS) {
        dc_puts();
        break;
      }
      DCCase(DAT_VAR) {
        break;
      }
      
      default:
        break;
    }
  }

  return 0;
}

/* Code Generation */
static int dx_code_generation(const char* outn) {
  if (glcs) {
    dx_init_data_section();
    write_lines_to_buffer(glcs->data_section);

    dx_init_text_section();
    write_lines_to_buffer(glcs->text_section);

    dx_init_start();
    write_lines_to_buffer(glcs->start_global);
    
    dx_init_exit();

    system("as " ASMFN1 " -o "ASMFO0);
    system(get_ln_cmd(outn));
  }

  // system("rm "ASMFN1);
  system("rm "ASMFO0);

  return 0;
}

/* Compiler Call */
int __compile__(d_ast* sda, dlcode_state* lcs, const char* outn) {
  glcs = lcs;
  gcA = sda;

  compiler_process();
  dx_code_generation(outn);
  return 0;
}