
#ifndef __DLOW_CODE
#define __DLOW_CODE

#include "ddefs.h"

typedef enum dlcode_register {
  DRG_NONE,
  DRG_RX0,
  DRG_RX1,
  DRG_RX2,
  DRG_RX3,
  DRG_RX4,
  DRG_RX5,
  DRG_RX6,
  DRG_RX7,
} dlcode_register;

typedef enum dlcode_op {
  DOP_ADD,    /* add [r1] [r2] */
  DOP_SUB,    /* sub [r1] [r2] */
  DOP_MUL,    /* mul [r1] [r2] */
  DOP_DIV,    /* div [r1] [r2] */

  DOP_MOV,    /* mov [r1] [r2] */
  DOP_PUSH,
  DOP_POP,

  DOP_CALL,   /* call label */
  DOP_JUMP,
  DOP_MRK_ID, /* [label]: || db [label] '...' */
  DOP_LABEL,  /* [label]: */
  DOP_GLOBAL, /* globl __main */

  DOP_CONST,  /* reg. 'value' */
  DOP_PUTS,
  DOP_VAR,
  DOP_RETURN, /* ret */

  /* bss */
  DOP_COMM,  /* Common Memory Area */
  DOP_LCOMM, /* Local Common Memory Area */

  DOP_SYSCALL,
  DOP_EXIT,
} dlcode_op;

typedef struct dline_cmd {
  dlcode_op op;
  int rg_qtt;
  dlcode_register rg0;
  dlcode_register rg1;
  d_byte_def value;
} dline_cmd;


typedef struct dlines_cmd {
  dline_cmd* dlcurr_cmd;
  int len;
  int cap;
} dlines_cmd;


typedef struct dlcode_state {
  dlines_cmd* bss_section;
  dlines_cmd* text_section;
  dlines_cmd* data_section;
  dlines_cmd* start_global;
  dlines_cmd* funcs_defs;
} dlcode_state;

dlcode_state* __lowcode_init__();

int push_line_op(dlines_cmd* v, dline_cmd* line);

dline_cmd* new_line_cmd(dlcode_op t, dlcode_register r0, dlcode_register r1, d_byte_def v);

#endif
