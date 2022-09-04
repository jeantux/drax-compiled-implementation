/* Drax Lang - 2022 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "dgen.h"
#include "ddefs.h"

int df_asm_gen(const char* format, ...) {
  FILE *fp;

  va_list args;

  fp=fopen(ASMFN1, "a");
  if(fp == NULL) {
    exit(-1);
  }
  va_start(args, format);

  vfprintf(fp, format, args);
  va_end(args);

  fclose(fp);
  return 0;
}
