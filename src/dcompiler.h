/* Drax Lang - 2022 */

#ifndef __DCOMPILER
#define __DCOMPILER

#include <stdbool.h>

#include "dlowcode.h"
#include "ddefs.h"

/* Arith Stack */
#define D_ARITH_STACK_SIZE 1024

#define DCSwitch() switch (gcA->op[gcA->pc++])

#define DCCase(t) case t:

typedef enum d_native_type{
  DNT_NONE,
  DNT_INT,
  DNT_FLOAT,
  DNT_STRING,  
} d_native_type;

typedef enum d_ast_op {
  DAT_INT,
  DAT_FLOAT,
  DAT_STRING,
  DAT_ADD,
  DAT_SUB,
  DAT_MUL,
  DAT_DIV,
  DAT_CALL,
  DAT_PUTS,
  DAT_VAR,
  DAT_RETURN,
  DAT_FUN,
  DAT_ID,
} d_ast_op;

typedef struct d_ast {
  d_ast_op* op;
  d_byte_def* val;
  int pc;
  int len;
  int cap;
} d_ast;

typedef struct d_fn_state {
  bool main_defined;
  bool curr_is_main;
} d_fn_state;

typedef struct dregx_stack {
  dlcode_register last;
  dlcode_register* rgx;
  int count;
} dregx_stack;

typedef struct d_const_data {
  char* name;
  int idx;
} d_const_data;

typedef struct d_const_table {
  char** names;
  int* idxs;
  d_native_type* types;
  d_byte_def* vals;
  int count;
  int cap;
} d_const_table;

d_ast* new_d_ast();

int push_d_ast(d_ast* v, d_ast_op op, d_byte_def val);

int __compile__(d_ast* sda, dlcode_state* lcs, const char* outn);

#endif
