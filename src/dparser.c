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

static void init_parser() {
// init parser
}

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
  make_op_line(DTK_NUMBER,    process_number,      NULL,           iNONE),
  make_op_line(DTK_AND,       NULL,                process_and,    iAND),
  make_op_line(DTK_ELSE,      NULL,                NULL,           iNONE),
  make_op_line(DTK_FALSE,     process_native_type, NULL,           iNONE),
  make_op_line(DTK_NIL,       process_native_type, NULL,           iNONE),
  make_op_line(DTK_TRUE,      process_native_type, NULL,           iNONE),
  make_op_line(DTK_FUN,       NULL,                NULL,           iNONE),
  make_op_line(DTK_IF,        NULL,                NULL,           iNONE),
  make_op_line(DTK_OR,        NULL,                process_or,     iOR),
  make_op_line(DTK_PUTS,      NULL,                NULL,           iNONE),
  make_op_line(DTK_RETURN,    NULL,                NULL,           iNONE),
  make_op_line(DTK_ERROR,     NULL,                NULL,           iNONE),
  make_op_line(DTK_EOF,       NULL,                NULL,           iNONE),
  make_op_line(DTK_CONCAT,    NULL,                process_binary, iTERM),
  make_op_line(DTK_ID,        process_variable,    NULL,           iNONE),
};

static char* get_lex_str(const char* chars, int length) {
  char* result = (char*) malloc(length + 1);
  memcpy(result, chars, length);
  result[length] = '\0';

  return result;
}

#define GET_PRIORITY(v) (&op_lines[v])

#define FATAL(v) dfatal(&parser.prev, v)

#define FATAL_CURR(v) dfatal(&parser.current, v)

#define GET_CURRENT_TOKEN() parser.current.type

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

void process_number(bool v) {
  UNUSED(v);
  double value = strtod(parser.prev.first, NULL);
  DPUSH_AST(DAT_NUMBER, value);
}

void process_or(bool v) {
  UNUSED(v);
}

void process_string(bool v) {
  UNUSED(v);
}

void process_variable(bool v) {
  UNUSED(v);
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

static void fun_declaration() {
  UNUSED(NULL);
}

static void if_definition() {
  UNUSED(NULL);
}

static void puts_definition() {
  DPUSH_AST(DAT_PUTS, get_lex_str(parser.current.first + 1, parser.current.length - 2));
}

static void process() {
  switch (GET_CURRENT_TOKEN()) {
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
      break;
    }
      
    case DTK_PUTS: {
      get_next_token();
      puts_definition();
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

  while (GET_CURRENT_TOKEN() != DTK_EOF) {
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
    fprintf(stderr, "  end of file");
  }  else {
    fprintf(stderr, " '%.*s'", token->length, token->first);
  }

  fprintf(stderr, " %s\n", message);
  parser.has_error = true;
}
