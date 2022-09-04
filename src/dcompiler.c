/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcompiler.h"
#include "dlowcode.h"
#include "dgen.h"
#include "ddefs.h"
#include "arch/backend.h"

static char* get_ln_cmd(const char* name) {
  const char* fcmd = ASMLD " -s "ASMFO0 " "ASMLIBS " -o ";
  int sz = strlen(fcmd) + strlen(name) + 1;
  char* cmd = (char*) calloc(sizeof(char), sz);

  strcat(cmd, fcmd);
  strcat(cmd, name);
  cmd[sz] = '\0';
  return cmd;
}

int __lowcode_process__(dlcode_state* lcs, const char* outn) {
  if (lcs) {

    dx_init_data_section();
    write_lines_to_buffer(lcs->data_section);

    dx_init_text_section();
    write_lines_to_buffer(lcs->text_section);

    dx_init_start();
    write_lines_to_buffer(lcs->start_global);
    
    dx_init_exit();

    system("as " ASMFN1 " -o "ASMFO0);
    system(get_ln_cmd(outn));
  }

  system("rm "ASMFN1);
  system("rm "ASMFO0);

  return 0;
}