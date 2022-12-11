#ifndef __DPARSER
#define __DPARSER

#include <stdbool.h>
#include "dlex.h"
#include "ddefs.h"
#include "dlowcode.h"
#include "dcompiler.h"

#define callback_table(n) void n(bool b);

#define make_op_line(i, l, m, r) [i] = {l, m, r}

typedef struct d_local_registers {
  char** vars;
  int length;
  int capacity;
} d_local_registers;

typedef struct parser_state {
  d_token current;
  d_token prev;
  d_local_registers* locals;
  bool has_error;
  bool panic_mode;
} parser_state;

typedef enum priorities {
  iNONE = 0,
  iASSIGNMENT,
  iOR, 
  iAND,
  iEQUALITY,
  iDIFF,
  iTERM,
  iFACTOR,
  iUNARY,
  iCALL,
  iPRIMARY
} priorities;

typedef void (*parser_callback) (bool v);

typedef struct {
  parser_callback prefix;
  parser_callback infix;
  priorities priorities;
} operation_line;

typedef enum scope_type {
  SCP_FUN,
  SCP_ROOT
} scope_type;

typedef struct parser_builder {
  scope_type type;
} parser_builder;

typedef struct parser_definition_table {
  int count;
  int cap;
  char** names;
} parser_definition_table;

int __parser__(d_ast* sda, const char* source);

void mark_compiler_roots();

callback_table(process_grouping);
callback_table(process_list);
callback_table(process_call);
callback_table(process_unary);
callback_table(process_binary);
callback_table(process_variable);
callback_table(process_string);
callback_table(process_number);
callback_table(process_and);
callback_table(process_or);
callback_table(process_native_type);
callback_table(process_id);

void dfatal(d_token* token, const char* message);

#endif
