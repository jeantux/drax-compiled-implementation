/* Drax Lang - 2022 */

#include "backend.h"
#include "x86_64.h"
#include "../dlowcode.h"
#include "../dgen.h"

int dx_init_data_section() {
  return dx_x86_64_data_section();
}

int dx_init_text_section() {
  return dx_x86_64_text_section();
}

int dx_init_start() {
  return dx_x86_64_start();
}

int dx_init_exit() {
  return dx_x86_64_exit();
}

/**
 * Convert lines_op to asm format
 * 
 */
int write_lines_to_buffer(dlines_op* v) {
  for (int i = 0; i < v->len; i++) {
    get_asm_code(&v->dlpair[i]);    
  }
  return 0;
}