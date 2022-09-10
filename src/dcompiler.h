/* Drax Lang - 2022 */

#ifndef __DCOMPILER
#define __DCOMPILER

#include "dlowcode.h"
#include "ddefs.h"

/* Arith Stack */
#define D_ARITH_STACK_SIZE 1024

#define DCSwitch() switch (gcA->op[gcA->pc++])

#define DCCase(t) case t:

typedef enum d_ast_op {
  DAT_NUMBER, /* Generic number */
  DAT_INT,
  DAT_FLOAT,
  DAT_ADD,
  DAT_SUB,
  DAT_MUL,
  DAT_DIV,
  DAT_CALL,
  DAT_CONST,
  DAT_PUTS,
  DAT_VAR,
} d_ast_op;

typedef struct d_ast {
  d_ast_op* op;
  d_byte_def* val;
  int pc;
  int len;
  int cap;
} d_ast;

typedef struct darith_stack {
  dlcode_register* rgx;
  int count;
} darith_stack;

d_ast* new_d_ast();

int push_d_ast(d_ast* v, d_ast_op op, d_byte_def val);

int __compile__(d_ast* sda, dlcode_state* lcs, const char* outn);

#endif
