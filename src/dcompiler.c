/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dcompiler.h"
#include "dlowcode.h"
#include "dgen.h"
#include "ddefs.h"
#include "arch/backend.h"

#define DCSwitch() switch (gcA->op[gcA->pc++])

#define DCCase(t) case t:

#define DPUSH_VALUE(l, t, r, v)   push_line_op(l, new_line_cmd(t, r, DRG_NONE, (d_byte_def) v));

#define DPUSH_RGX(l, t, r0, r1)   push_line_op(l, new_line_cmd(t, r0, r1, 0));

#define GET_PREV_VAL(_idx)  gcA->val[gcA->pc - (1 + _idx)]

#define GET_STRING_VAL()    (CAST_STRING(GET_PREV_VAL(0)))

int dg_const_def = 0;
dlcode_state* glcs;
d_ast* gcA;
darith_stack* garith_stack;

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
  DPUSH_VALUE(v, DOP_MRK_ID, DRG_NONE, name);
  DPUSH_VALUE(v, DOP_CONST,  DRG_NONE, value);
  return 0;
}
/**
 * Print String definition
 * 
 * Using Data section
 */
static int dc_puts_instruction(dlines_cmd* v, char* var) {
  DPUSH_VALUE(v, DOP_PUTS, DRG_NONE, var);
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

/* Arith Stack Helpers */
static void arith_push(dlcode_register crgx) {
  garith_stack->count++;
  garith_stack->rgx[garith_stack->count-1] = crgx;
}

static dlcode_register arith_pop() {
  if (garith_stack->count <= 0) {
    return DRG_NONE;
  }

  garith_stack->count--;
  return garith_stack->rgx[garith_stack->count];
}

/* Main Atith. process */
static void dc_arith_op(dlcode_op t_op) {
  dlcode_register rgx = arith_pop();

  if (DRG_NONE == rgx) {
    // register must free
    DPUSH_VALUE(glcs->start_global, DOP_POP, DRG_RX0, NULL);
    DPUSH_VALUE(glcs->start_global, DOP_POP, DRG_RX1, NULL);
    DPUSH_RGX(glcs->start_global,   t_op,    DRG_RX0, DRG_RX1);
    arith_push(DRG_RX0);
    return;
  }

  DPUSH_VALUE(glcs->start_global, DOP_POP, DRG_RX0, NULL);
  DPUSH_RGX(glcs->start_global,   t_op,    DRG_RX0, DRG_RX1);
}

static void debugger_ast() {
  gcA->pc = 0;
  while (gcA->pc < gcA->len) {
    DCSwitch() {
      DCCase(DAT_ADD) {
        printf("[add]\n");
        break;
      }
      DCCase(DAT_SUB) {
        printf("[sub]\n");
        break;
      }
      DCCase(DAT_MUL) {
        printf("[mul]\n");
        break;
      }
      DCCase(DAT_DIV) {
        printf("[div]\n");
        break;
      }
      DCCase(DAT_CALL) {
        printf("[call]\n");
        break;
      }
      DCCase(DAT_CONST) {
        printf("[const]\n");
        break;
      }
      DCCase(DAT_PUTS) {
        printf("[puts]\n");
        break;
      }
      DCCase(DAT_VAR) {
        printf("[var]\n");
        break;
      }
      DCCase(DAT_NUMBER) {
        printf("NUM => [%lu]\n", GET_PREV_VAL(0));
      }
      default:
        break;
    }
  }
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
      DCCase(DAT_NUMBER) {
        DPUSH_VALUE(glcs->start_global, DOP_MOV,  DRG_RX0, GET_PREV_VAL(0));
        DPUSH_VALUE(glcs->start_global, DOP_PUSH, DRG_RX0, NULL);
        break;
      }
      
      default:
        break;
    }
  }

  return 0;
}

/* Compiler Call */
int __compile__(d_ast* sda, dlcode_state* lcs, const char* outn) {
  glcs = lcs;
  gcA = sda;

  garith_stack = (darith_stack*) malloc(sizeof(darith_stack));
  garith_stack->count = 0;
  garith_stack->rgx = (dlcode_register*) malloc(sizeof(dlcode_register) * D_ARITH_STACK_SIZE);

  debugger_ast();
  compiler_process();
  dx_code_generation(glcs, outn);
  return 0;
}