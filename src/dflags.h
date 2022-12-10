/* Drax Lang - 2022 */

#ifndef __DFLAGS
#define __DFLAGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DRAX_VM_VERSION  "v0.0.1-dev"

void version_app();

char* parse_flags(int* res, int argc, char** argv);

#endif
