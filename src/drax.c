#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "dlowcode.h"
#include "dcompiler.h"
#include "dparser.h"
#include "dgen.h"
#include "dio.h"
#include "dflags.h"
#include "ddefs.h"


static int process_file(char* path, const char* outn) {
  char* content = NULL;
  if(get_file_content(path, &content)) {
    // error
    return 1;
  }

  dlcode_state* lcs = __lowcode_init__();
  d_ast* sda = new_d_ast(); /* Shared Drax AST */


  if(__parser__(sda, content)) { exit(1); };
  __compile__(sda, lcs, outn);
  return 0;
}

int main(int argc, char** argv) {
  int t;
  char* _outn = parse_flags(&t, argc, argv);
  char* path = get_path(argc, argv);
  if (t == 1 && path) {
    return process_file(path, _outn);
  }

  return 0;
}