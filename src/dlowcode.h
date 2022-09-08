
#ifndef __DLOW_CODE
#define __DLOW_CODE

typedef enum dlcode_op {
  DOP_ADD,       /* add [r1] [r2] */
  DOP_MOV,       /* mov [r1] [r2] */
  DOP_CALL,      /* call label */
  DOP_MRK_ID,    /* [label]: || db [label] '...' */
  DOP_LABEL,     /* [label]: */

  DOP_CONST,     /* reg. 'value' */
  DOP_PUTS,
  DOP_VAR,

  DOP_SYSCALL,
  DOP_EXIT,
} dlcode_op;

typedef struct dline_pair {
  dlcode_op op;
  char* command;
} dline_pair;


typedef struct dlines_op {
  dline_pair* dlpair;
  int len;
  int cap;
} dlines_op;


typedef struct dlcode_state {
  // dlines_op bss_section;
  dlines_op* text_section;
  dlines_op* data_section;
  dlines_op* start_global;
} dlcode_state;

dlcode_state* __lowcode_init__();

int push_line_op(dlines_op* v, dline_pair* line);

dline_pair* new_line_pair(dlcode_op t, char* v);

#endif
