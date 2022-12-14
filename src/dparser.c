#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dparser.h"
#include "dlex.h"
#include "dcompiler.h"

#define LOCAL_SIZE_FACTOR 30

parser_state parser;
parser_builder* current = NULL;
d_ast* gsda;
parser_definition_table* gp_def_tbl;

static operation_line op_lines[] = {
  make_op_line(DTK_PAR_OPEN,  process_grouping,    process_call,   iCALL),
  make_op_line(DTK_PAR_CLOSE, NULL,                NULL,           iNONE),
  make_op_line(DTK_BKT_OPEN,  process_list,        NULL,           iNONE),
  make_op_line(DTK_BKT_CLOSE, NULL,                NULL,           iNONE),
  make_op_line(DTK_DO,        NULL,                NULL,           iNONE),
  make_op_line(DTK_END,       NULL,                NULL,           iNONE),
  make_op_line(DTK_COMMA,     NULL,                NULL,           iNONE),
  make_op_line(DTK_DOT,       NULL,                NULL,           iNONE),
  make_op_line(DTK_SUB,       process_unary,       process_binary, iTERM),
  make_op_line(DTK_ADD,       NULL,                process_binary, iTERM),
  make_op_line(DTK_DIV,       NULL,                process_binary, iFACTOR),
  make_op_line(DTK_MUL,       NULL,                process_binary, iFACTOR),
  make_op_line(DTK_BNG,       process_unary,       NULL,           iNONE),
  make_op_line(DTK_BNG_EQ,    NULL,                process_binary, iEQUALITY),
  make_op_line(DTK_EQ,        NULL,                NULL,           iNONE),
  make_op_line(DTK_DEQ,       NULL,                process_binary, iEQUALITY),
  make_op_line(DTK_BG,        NULL,                process_binary, iDIFF),
  make_op_line(DTK_BE,        NULL,                process_binary, iDIFF),
  make_op_line(DTK_LS,        NULL,                process_binary, iDIFF),
  make_op_line(DTK_LE,        NULL,                process_binary, iDIFF),
  make_op_line(DTK_STRING,    process_string,      NULL,           iNONE),
  make_op_line(DTK_INT,       process_integer,     NULL,           iNONE),
  make_op_line(DTK_FLOAT,     process_float,       NULL,           iNONE),
  make_op_line(DTK_AND,       NULL,                process_and,    iAND),
  make_op_line(DTK_ELSE,      NULL,                NULL,           iNONE),
  make_op_line(DTK_FALSE,     process_native_type, NULL,           iNONE),
  make_op_line(DTK_NIL,       process_native_type, NULL,           iNONE),
  make_op_line(DTK_TRUE,      process_native_type, NULL,           iNONE),
  make_op_line(DTK_FUN,       NULL,                NULL,           iNONE),
  make_op_line(DTK_IF,        NULL,                NULL,           iNONE),
  make_op_line(DTK_OR,        NULL,                process_or,     iOR),
  make_op_line(DTK_RETURN,    NULL,                NULL,           iNONE),
  make_op_line(DTK_ERROR,     NULL,                NULL,           iNONE),
  make_op_line(DTK_EOF,       NULL,                NULL,           iNONE),
  make_op_line(DTK_CONCAT,    NULL,                process_binary, iTERM),
  make_op_line(DTK_LET,       process_variable,    NULL,           iNONE),
  make_op_line(DTK_ID,        process_id,          NULL,           iNONE),
};

/* Helpers */

static void init_parser() {
  current = (parser_builder*) malloc(sizeof(parser_builder));
  current->type = SCP_ROOT;

  gp_def_tbl = (parser_definition_table*) malloc(sizeof(parser_definition_table));
  gp_def_tbl->names = (char**) malloc(sizeof(char*) * LOCAL_SIZE_FACTOR);
  gp_def_tbl->count = 0;
  gp_def_tbl->cap = LOCAL_SIZE_FACTOR;
}

static void push_def_on_table(char* name) {
  gp_def_tbl->count++;

  if (gp_def_tbl->count >= gp_def_tbl->cap) {
    gp_def_tbl->cap = gp_def_tbl->cap + LOCAL_SIZE_FACTOR;
    gp_def_tbl->names = realloc(gp_def_tbl->names, gp_def_tbl->cap * sizeof(char *));
  }

  gp_def_tbl->names[gp_def_tbl->count -1] = name;
}

static bool has_def_on_table(char* name) {
  for (int i = 0; i < gp_def_tbl->count; i++) {
    if (strcmp(gp_def_tbl->names[i], name) == 0) {
      return true;
    }
  }

  return false;
}

static char* get_lex_str(const char* chars, int length) {
  char* result = (char*) malloc(length + 1);
  memcpy(result, chars, length);
  result[length] = '\0';

  return result;
}

#define GET_PRIORITY(v) (&op_lines[v])

#define FATAL(v) dfatal(&parser.prev, v)

#define FATAL_CURR(v) dfatal(&parser.current, v)

#define GET_CURRENT_TOKEN_TYPE() parser.current.type

#define DPUSH_AST(t, v) push_d_ast(gsda, t, (d_byte_def) v)

static void get_next_token() {
  parser.prev = parser.current;

  while(true) {
    parser.current = next_token();
    if (parser.current.type != DTK_ERROR) break;

    FATAL_CURR(parser.current.first);
  }
}

static void process_token(dlex_types type, const char* message) {
  if (parser.current.type == type) {
    get_next_token();
    return;
  }

  FATAL_CURR(message);
}

static void expression();

static void process();

static void parse_priorities(priorities priorities);

void process_and(bool v) {
  UNUSED(v);
}

void process_binary(bool v) {
  UNUSED(v);
  dlex_types operatorType = parser.prev.type;
  operation_line* rule = GET_PRIORITY(operatorType);
  parse_priorities((priorities)(rule->priorities + 1));

  switch (operatorType) {
    case DTK_BNG_EQ:
    case DTK_DEQ:
    case DTK_BG:
    case DTK_BE:
    case DTK_LS:
    case DTK_LE:
    case DTK_CONCAT:
      break;
    case DTK_ADD: DPUSH_AST(DAT_ADD, NULL); break;
    case DTK_SUB: DPUSH_AST(DAT_SUB, NULL); break;
    case DTK_MUL: DPUSH_AST(DAT_MUL, NULL); break;
    case DTK_DIV: DPUSH_AST(DAT_DIV, NULL); break;
    default: return;
  }
}

void process_call(bool v) {
  UNUSED(v);
}

void process_native_type(bool v) {
  UNUSED(v);
  switch (parser.prev.type) {
    case DTK_NIL:
    case DTK_FALSE:
    case DTK_TRUE:
    default: return;
  }
}

void process_grouping(bool v) {
  UNUSED(v);
  expression();
  process_token(DTK_PAR_CLOSE, "Expect ')' after expression.");
}

void process_list(bool v) {
  UNUSED(v);
}

void process_integer(bool v) {
  UNUSED(v);
  double value = strtod(parser.prev.first, NULL);
  DPUSH_AST(DAT_INT, value);
}

void process_float(bool v) {
  UNUSED(v);
  double value = strtod(parser.prev.first, NULL);
  DPUSH_AST(DAT_FLOAT, value);
}

void process_or(bool v) {
  UNUSED(v);
}

void process_string(bool v) {
  UNUSED(v);
  DPUSH_AST(DAT_CONST, get_lex_str(parser.prev.first + 1, parser.prev.length - 2));
}

void process_id(bool v) {
  UNUSED(v);
  char* name = get_lex_str(parser.prev.first, parser.prev.length);
  DPUSH_AST(DAT_ID, name);

  if (!has_def_on_table(name)) {
    FATAL("is not defined.");
  }
}

void process_variable(bool v) {
  UNUSED(v);
  process_token(DTK_ID, "invalid expression, identifier is expected.");
  char* name = get_lex_str(parser.prev.first, parser.prev.length);

  if (has_def_on_table(name)) {
    FATAL("already defined.");
  }

  DPUSH_AST(DAT_VAR, name);
  push_def_on_table(name);
  process_token(DTK_EQ, "Missing \"=\" operator.");
  expression();
}

void process_unary(bool v) {
  UNUSED(v);
}

/* end of processors functions */

static void parse_priorities(priorities p) {
  get_next_token();
  
  parser_callback p_call = GET_PRIORITY(parser.prev.type)->prefix;
  if (p_call == NULL) {
    FATAL("Expect expression.");
    return;
  }

  bool v = p <= iASSIGNMENT;
  p_call(v);

  while (p <= GET_PRIORITY(parser.current.type)->priorities) {
    get_next_token();
    parser_callback infix_rule = GET_PRIORITY(parser.prev.type)->infix;

    infix_rule(v);
  }
}

static void expression() {
  parse_priorities(iASSIGNMENT);
}

static void block() {
  UNUSED(NULL);
}

static void process_scope_function() {
    while ((GET_CURRENT_TOKEN_TYPE() != DTK_END) && (GET_CURRENT_TOKEN_TYPE() != DTK_EOF)) {
    process();
  }

  process_token(DTK_END, "Expect 'end' after block.");
}

static void process_type() {
  get_next_token();
  switch (GET_CURRENT_TOKEN_TYPE()) {
  case DTK_KEY_ANY: break;
  case DTK_KEY_BOOL: break;
  case DTK_KEY_INT: break;
  case DTK_KEY_FLOAT: break;
  case DTK_KEY_STRING: break;
  
  default:
  // PUT ANY TYPE or DINAMIC TYPE
    break;
  }
}

static void process_type_definition() {
  if (DTK_DBCOLON == GET_CURRENT_TOKEN_TYPE()) {
    get_next_token();
    process_type();
    return;
  }
}

static void fun_declaration() {
  process_token(DTK_ID, "Expect function name.");

  DPUSH_AST(DAT_FUN, get_lex_str(parser.prev.first, parser.prev.length));

  process_token(DTK_PAR_OPEN, "Expect '(' after fun.");
  // process args
  process_token(DTK_PAR_CLOSE, "Expect ')' after fun.");

  process_type_definition();

  current->type = SCP_FUN;
  process_scope_function();
  
  current->type = SCP_ROOT;
}

static void if_definition() {
  UNUSED(NULL);
}

static void process() {
  switch (GET_CURRENT_TOKEN_TYPE()) {
    case DTK_FUN: {
      get_next_token();
      fun_declaration();
      break;
    }

    case DTK_IF: {
      get_next_token();
      if_definition();
      break;
    }

    case DTK_RETURN: {
      get_next_token();
      if (current->type == SCP_ROOT) {
        FATAL("Illegal return statement.");
      }

      expression();
      DPUSH_AST(DAT_RETURN, NULL);
      break;
    }
      
    case DTK_PUTS: {
      DPUSH_AST(DAT_PUTS, NULL);
      get_next_token();
      expression();
      break;
    }

    case DTK_DO: {
      get_next_token();
      block();
      break;
    }

    default: {
      expression();
      break;
    }
  }
}

int __parser__(d_ast* sda, const char* source) {
  gsda = sda;
  init_lexan(source);
  init_parser();
  
  parser.has_error = false;
  parser.panic_mode = false;

  get_next_token();

  while (GET_CURRENT_TOKEN_TYPE() != DTK_EOF) {
    process();
  }

  if (parser.has_error) {
    return 1;
  }
  
  return 0;
}

void dfatal(d_token* token, const char* message) {
  if (parser.panic_mode) return;
  parser.panic_mode = true;
  fprintf(stderr, "Error, line %d:\n", token->line);

  if (token->type == DTK_EOF) {
    fprintf(stderr, "  end of file.");
  }  else {
    fprintf(stderr, " '%.*s'", token->length, token->first);
  }

  fprintf(stderr, " %s\n", message);
  parser.has_error = true;
}
