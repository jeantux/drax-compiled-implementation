/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "../dlowcode.h"
#include "../dgen.h"
#include "../ddefs.h"
  
#ifdef __x86_64__
  #include "x86_32.h"
  
  #define ASMLD       "ld"
  #define ASMCOMPILER "as "
  #define ASM_AS_ARGS " --32 "
  #define ASM_LD_ARGS " -m elf_i386 "

  static int dx_init_bss_section() {
    return dx_x86_32_bss_section();
  }

  static int dx_init_data_section() {
    return dx_x86_32_data_section();
  }

  static int dx_init_text_section() {
    return dx_x86_32_text_section();
  }

  static int dx_init_exit() {
    return dx_x86_32_exit(NULL);
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
  #ifdef __DRAX_INSPECT
    system("rm "ASMFN1);
  #endif
  if (lcs) {
    const char* exit_lbl = "exit";
    dx_init_bss_section();
    write_lines_to_buffer(lcs->bss_section);

    dx_init_data_section();
    write_lines_to_buffer(lcs->data_section);

    dx_init_text_section();
    write_lines_to_buffer(lcs->text_section);

    write_lines_to_buffer(lcs->start_global);
    /* Must be implemented by arch implementation */
    df_asm_gen(SDCODE_RETURN FL);

    /* Jump to exit to avoid exec other functions */
    get_asm_code(new_line_cmd(DOP_JUMP, DRG_NONE, DRG_NONE, CAST_DRAX_BYTE(exit_lbl)));

    write_lines_to_buffer(lcs->funcs_defs);

    get_asm_code(new_line_cmd(DOP_LABEL, DRG_NONE, DRG_NONE, CAST_DRAX_BYTE(exit_lbl)));
    dx_init_exit();

    system(ASMCOMPILER ASMFN1 ASM_AS_ARGS " -o "ASMFO0);
    system(get_ln_cmd(outn));
  }

  #ifndef __DRAX_INSPECT
    system("rm "ASMFN1);
  #endif

  system("rm "ASMFO0);

  return 0;
}