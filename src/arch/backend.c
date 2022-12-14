/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "../dlowcode.h"
#include "../dgen.h"
#include "../ddefs.h"
  
#ifdef __x86_64__
  #include "i386.h"
#endif

#include "gcc.h"

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
    dx_init_bss_section();
    write_lines_to_buffer(lcs->bss_section);

    dx_init_data_section();
    write_lines_to_buffer(lcs->data_section);

    dx_init_text_section();
    write_lines_to_buffer(lcs->text_section);

    write_lines_to_buffer(lcs->start_global);
    
    #ifdef __DRAX_BACKEND_ASM
      /**
       * GNU Assembly.
       * 
       * Ensures that the program exits correctly 
       * at the end of execution.
      */

      const char* exit_lbl = "exit";
      
      /* Must be implemented by arch implementation */
      df_asm_gen(SDCODE_RETURN FL);

      /* Jump to exit to avoid exec other functions */
      get_asm_code(new_line_cmd(DOP_JUMP, DRG_NONE, DRG_NONE, CAST_DRAX_BYTE(exit_lbl), TLC_NONE));

      write_lines_to_buffer(lcs->funcs_defs);

      get_asm_code(new_line_cmd(DOP_LABEL, DRG_NONE, DRG_NONE, CAST_DRAX_BYTE(exit_lbl), TLC_NONE));
    #endif
    dx_init_exit();

    #ifdef __DRAX_BACKEND_ASM
      system(ASMCOMPILER ASMFN1 ASM_AS_ARGS " -o "ASMFO0);
    #endif
    system(get_ln_cmd(outn));
  }

  #ifndef __DRAX_INSPECT
    system("rm "ASMFN1);
  #endif

  #ifdef __DRAX_BACKEND_ASM
    system("rm "ASMFO0);
  #endif

  return 0;
}