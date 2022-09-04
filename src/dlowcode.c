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

int push_line_op(dlines_op* v, dline_pair* line) {
  v->len++;
  if (v->len >= v->cap) {
    v->cap += LINE_OP_SIZE;
    v->dlpair = realloc(v->dlpair, v->cap * sizeof(dline_pair));
  }

  v->dlpair[v->len -1] = *line;
  return 0;
}

static dlines_op* new_line_op() {
  dlines_op* v = (dlines_op*) malloc(sizeof(dlines_op));
  v->dlpair = (dline_pair*) malloc(sizeof(dline_pair) * LINE_OP_SIZE);
  v->len = 0;
  v->cap = LINE_OP_SIZE;
  return v;
}

dline_pair* new_line_pair(dlcode_op t, char* v) {
  dline_pair* l = (dline_pair*) malloc(sizeof(dline_pair));
  l->op = t;
  l->command = v;
  return l;
}

/**
 * Create Global state (OP codes)
 * 
 */
dlcode_state* __lowcode_init__() {
  dlcode_state* lcs = (dlcode_state*) malloc(sizeof(dlcode_state));
  lcs->text_section = new_line_op();
  lcs->data_section = new_line_op();
  lcs->start_global = new_line_op();
  
  return lcs;
}
