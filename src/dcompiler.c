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

#define NEXT_CMD gcA->pc++;

#define GET_PREV_VAL(_idx)  gcA->val[gcA->pc - (1 + _idx)]

#define GET_STRING_VAL()    (CAST_STRING(GET_PREV_VAL(0)))
#define GET_INT_VAL()       (CAST_INT(GET_PREV_VAL(0)))

#define INC_CONST_COUNT dg_const_def++;

int dg_const_def = 0; /* Last const defintions */
dlcode_state* glcs;
d_ast* gcA;
dregx_stack* garith_stack;
d_fn_state* fn_state;

dlines_cmd* curr_global_state;
d_const_table* gconst_table;


/* Stack Helpers */

#define AST_SIZE 1024

/* AST Helpers */

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

/* Const table helper */

#define CONST_TABLE_FACTOR 50

static void new_const_table() {
  gconst_table = (d_const_table*) malloc(sizeof(d_const_table));
  gconst_table->count = 0;
  gconst_table->cap = CONST_TABLE_FACTOR;
  gconst_table->idxs = malloc(gconst_table->cap * sizeof(int));
  gconst_table->names = malloc(gconst_table->cap * sizeof(char*));
}

static void push_const_table(char* name, int idx) {
  gconst_table->count++;

  if (gconst_table->count >= gconst_table->cap) {
    gconst_table->cap = gconst_table->cap + CONST_TABLE_FACTOR;
    gconst_table->idxs = realloc(gconst_table->idxs, gconst_table->cap * sizeof(int));
    gconst_table->names = realloc(gconst_table->names, gconst_table->cap * sizeof(char*));
  }
  
  gconst_table->idxs[gconst_table->count -1] = idx;
  gconst_table->names[gconst_table->count -1] = name;
}

static int find_const_table(char* name) {
  for (int i = 0; i < gconst_table->count; i++) {
    if (strcmp(gconst_table->names[i], name) == 0) {
      return gconst_table->idxs[i];
    }
  }

  return -1;
}

/* General Helpers */
static char* get_var_name(int idx) {
  char* var;
  asprintf(&var, "DS_%i", idx);
  return var;
}

/* Compiler Functions */

static void fatal_compiler(const char* error) {
  // monut stack trace
  printf("Compilation error: %s\n", error);
  exit(0);
}

/**
 * Define const on section data
 */
static int dc_const_string_data(dlines_cmd* v, char* name, char* value) {
  DPUSH_VALUE(v, DOP_MRK_ID, DRG_NONE, name);
  DPUSH_VALUE(v, DOP_STRING,  DRG_NONE, value);
  return 0;
}
/**
 * Define number on section data
 */
static int dc_const_int_data(dlines_cmd* v, char* name, int value) {
  DPUSH_VALUE(v, DOP_MRK_ID, DRG_NONE, name);
  DPUSH_VALUE(v, DOP_INT,    DRG_NONE, value);
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
  dc_const_string_data(glcs->data_section, (char*) var, content);
  dc_puts_instruction(curr_global_state, (char*) var);
  return 0;
}

static int dc_puts_idx(int idx) {
  char* var = get_var_name(idx);
  dc_puts_instruction(curr_global_state, (char*) var);
  return 0;
}

static int dc_const_string() {
  INC_CONST_COUNT;
  char* var = get_var_name(dg_const_def);
  dc_const_string_data(glcs->data_section, var, GET_STRING_VAL());
  return dg_const_def;
}

static int dc_const_number() {
  INC_CONST_COUNT;
  char* var = get_var_name(dg_const_def);
  dc_const_int_data(glcs->data_section, var, GET_INT_VAL());
  return dg_const_def;
}

static void dc_var() {
  char* name = GET_STRING_VAL();
  
  int idx = 0;
  DCSwitch() {
      DCCase(DAT_CONST) {
        idx = dc_const_string();
        break;
      }
      DCCase(DAT_NUMBER) {
        idx = dc_const_number();
        break;
      }
      DCCase(DAT_ID) {
        break;
      }
      default: {
        // throuw compilation error
        break;
      }
  }
  push_const_table(name, idx);
}

static void dc_puts() {
  DCSwitch() {
      DCCase(DAT_CONST) {
        INC_CONST_COUNT;
        char* var = get_var_name(dg_const_def);
        dc_puts_str(var, GET_STRING_VAL());
        break;
      }
      DCCase(DAT_ID) {
        int idx = find_const_table(GET_STRING_VAL());

        if (idx == -1) {
          char* var;
          asprintf(&var, "identifier '%s' not found.", GET_STRING_VAL());
          fatal_compiler(var);
        }

        dc_puts_idx(idx);
        break;
      }
      default: {
        // throuw compilation error
        break;
      }
  }

}

/* Arith Stack Helpers */
static void reg_stack_push(dlcode_register crgx) {
  garith_stack->count++;
  garith_stack->last = crgx;
  garith_stack->rgx[garith_stack->count-1] = crgx;
}

static dlcode_register reg_stack_pop() {
  if (garith_stack->count <= 0) {
    return DRG_NONE;
  }

  garith_stack->count--;
  return garith_stack->rgx[garith_stack->count];
}

/* Main Atith. process */
static void dc_arith_op(dlcode_op t_op) {

  DPUSH_VALUE(curr_global_state, DOP_POP, t_op == DOP_DIV ? DRG_RX1 : DRG_RX0, NULL);
  DPUSH_VALUE(curr_global_state, DOP_POP, t_op == DOP_DIV ? DRG_RX0 : DRG_RX1, NULL);

  if (t_op == DOP_DIV) {
    DPUSH_RGX(curr_global_state, t_op, DRG_RX1, DRG_NONE);
  } else {
    DPUSH_RGX(curr_global_state, t_op, DRG_RX0, DRG_RX1);
  }

  DPUSH_RGX(curr_global_state, DOP_PUSH, t_op == DOP_DIV ? DRG_RX0 : DRG_RX1, DRG_NONE);
  /* register with result */
  dlcode_register rgx = reg_stack_pop();

  if (DRG_NONE == rgx) {
    reg_stack_push(DRG_RX1);
  }
}

static void dc_function() {
  char* fname = GET_STRING_VAL();

  if (strcmp(fname, "main") == 0) {
    fn_state->main_defined = true;
    fn_state->curr_is_main = true;
    fname = (char*) "_start";
    DPUSH_VALUE(glcs->start_global, DOP_GLOBAL, DRG_NONE, fname);
    DPUSH_VALUE(glcs->start_global, DOP_LABEL,  DRG_NONE, fname);
    return;
  }

  curr_global_state = glcs->funcs_defs;
  DPUSH_VALUE(curr_global_state, DOP_LABEL, DRG_NONE, fname);
  fn_state->curr_is_main = false;
}

#ifdef __DRAX_INSPECT
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
        DCCase(DAT_RETURN) {
          printf("[return]\n");
          break;
        }
        DCCase(DAT_FUN) {
          printf("FUNCTION => [%s]\n", (char*) gcA->val[gcA->pc - 1]);
          break;
        }
        DCCase(DAT_VAR) {
          printf("[var]\n");
          break;
        }
        DCCase(DAT_NUMBER) {
          printf("NUM => [%lu]\n", GET_PREV_VAL(0));
          break;
        }
        DCCase(DAT_ID) {
          printf("[ID]\n");
          break;
        }
        default:
          break;
      }
    }
  }
#endif
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
        // dc_const_string();
        break;
      }
      DCCase(DAT_PUTS) {
        dc_puts();
        break;
      }
      DCCase(DAT_RETURN) {        
        if (fn_state->curr_is_main) {
          DPUSH_VALUE(curr_global_state, DOP_POP,  DRG_RX1, NULL);
          DPUSH_VALUE(curr_global_state, DOP_EXIT, DRG_RX1, NULL);
        } else {
          DPUSH_VALUE(curr_global_state, DOP_RETURN, DRG_NONE, NULL);
        }

        fn_state->curr_is_main = fn_state->main_defined;
        curr_global_state = glcs->start_global;
        break;
      }
      DCCase(DAT_FUN) {
        dc_function();
        break;
      }
      DCCase(DAT_VAR) {
        dc_var();
        break;
      }
      DCCase(DAT_NUMBER) {
        DPUSH_VALUE(curr_global_state, DOP_MOV,  DRG_RX0, GET_PREV_VAL(0));
        DPUSH_VALUE(curr_global_state, DOP_PUSH, DRG_RX0, NULL);
        break;
      }
      
      default:
        break;
    }
  }

  return 0;
}

static int init_bss() {
  d_byte_pair_def* dpair = (d_byte_pair_def*) malloc(sizeof(d_byte_pair_def));
  char* key = (char*) "buffer";
  char* size = (char*) "1024";

  dpair->left  = CAST_DRAX_BYTE(key);
  dpair->right = CAST_DRAX_BYTE(size);

  DPUSH_VALUE(glcs->bss_section, DOP_COMM, DRG_NONE, CAST_DRAX_BYTE(dpair));
  return 0;
}

/* Compiler Call */
int __compile__(d_ast* sda, dlcode_state* lcs, const char* outn) {
  glcs = lcs;
  gcA = sda;

  new_const_table();

  garith_stack = (dregx_stack*) malloc(sizeof(dregx_stack));
  garith_stack->count = 0;
  garith_stack->rgx = (dlcode_register*) malloc(sizeof(dlcode_register) * D_ARITH_STACK_SIZE);

  fn_state = (d_fn_state*) malloc(sizeof(fn_state));
  fn_state->main_defined = false;
  fn_state->curr_is_main = false;

  curr_global_state = glcs->start_global;

  #ifdef __DRAX_INSPECT
    debugger_ast();
  #endif

  init_bss();
  compiler_process();

  if (!fn_state->main_defined) {
    printf("Error:\n  main is not defined!\n");
    exit(1);
  }

  dx_code_generation(glcs, outn);

  return 0;
}