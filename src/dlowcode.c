/**
 * @file dlowcode.c
 * @author Jean Carlos (jeantux.com)
 * @brief Backend to asm code generate
 * @version 0.1-dev
 * @date 2022-08-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlowcode.h"
#include "dgen.h"

/* Ultils */

#define LINE_OP_SIZE 1024 /* so short */

int push_line_op(dlines_cmd* v, dline_cmd* line) {
  v->len++;
  if (v->len >= v->cap) {
    v->cap += LINE_OP_SIZE;
    v->dlcurr_cmd = realloc(v->dlcurr_cmd, v->cap * sizeof(dline_cmd));
  }

  v->dlcurr_cmd[v->len -1] = *line;
  return 0;
}

static dlines_cmd* new_lines_cmd() {
  dlines_cmd* v = (dlines_cmd*) malloc(sizeof(dlines_cmd));
  v->dlcurr_cmd = (dline_cmd*) malloc(sizeof(dline_cmd) * LINE_OP_SIZE);
  v->len = 0;
  v->cap = LINE_OP_SIZE;
  return v;
}

dline_cmd* new_line_cmd(dlcode_op t, dlcode_register r0, dlcode_register r1, d_byte_def v) {
  dline_cmd* l = (dline_cmd*) malloc(sizeof(dline_cmd));
  l->rg_qtt = r1 == DRG_NONE ? 1 : 2;
  l->op = t;
  l->rg0 = r0;
  l->rg1 = r1;
  l->value = v;
  return l;
}

/**
 * Create Global state (OP codes)
 * 
 */
dlcode_state* __lowcode_init__() {
  dlcode_state* lcs = (dlcode_state*) malloc(sizeof(dlcode_state));
  lcs->text_section = new_lines_cmd();
  lcs->data_section = new_lines_cmd();
  lcs->start_global = new_lines_cmd();
  lcs->funcs_defs = new_lines_cmd();
  
  return lcs;
}
