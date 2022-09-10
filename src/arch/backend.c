/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "../dlowcode.h"
#include "../dgen.h"
#include "../ddefs.h"
  
#ifdef __x86_64__
  #include "x86_64.h"
  
  #define ASMLD       "ld"
  #define ASMCOMPILER "as "
  #define ASM_AS_ARGS " --32 "
  #define ASM_LD_ARGS " -m elf_i386 "

  static int dx_init_data_section() {
    return dx_x86_64_data_section();
  }

  static int dx_init_text_section() {
    return dx_x86_64_text_section();
  }

  static int dx_init_start() {
    return dx_x86_64_start();
  }

  static int dx_init_exit() {
    return dx_x86_64_exit();
  }

#endif

static char* get_ln_cmd(const char* name) {
  const char* fcmd = ASMLD ASM_LD_ARGS " -s "ASMFO0 " "ASMLIBS " -o ";
  int sz = strlen(fcmd) + strlen(name) + 1;
  char* cmd = (char*) calloc(sizeof(char), sz);

  strcat(cmd, fcmd);
  strcat(cmd, name);
  cmd[sz] = '\0';
  return cmd;
}

/**
 * Convert lines_op to asm format
 * 
 */
static int write_lines_to_buffer(dlines_cmd* v) {
  for (int i = 0; i < v->len; i++) {
    get_asm_code(&v->dlcurr_cmd[i]);    
  }
  return 0;
}

int dx_code_generation(dlcode_state* lcs, const char* outn) {
  system("rm "ASMFN1); // debug
  if (lcs) {
    dx_init_data_section();
    write_lines_to_buffer(lcs->data_section);

    dx_init_text_section();
    write_lines_to_buffer(lcs->text_section);

    dx_init_start();
    write_lines_to_buffer(lcs->start_global);
    
    dx_init_exit();

    system(ASMCOMPILER ASMFN1 ASM_AS_ARGS " -o "ASMFO0);
    system(get_ln_cmd(outn));
  }

  // system("rm "ASMFN1);
  system("rm "ASMFO0);

  return 0;
}