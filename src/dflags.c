/* Drax Lang - 2022 */

#include "dflags.h"

void version_app() {
  printf("Drax Language (%s)\n", DRAX_VM_VERSION);
}

/**
 * Drax flags implementation - 2022
 *
 * Simple flag identifier.
 */

static int non_flag(char * name) {
  if (name[0] != '-') return 1;

  return 0;
}


static int argcmp(char sname, const char * name, char * arg) {
  if (arg[0] == '-') {
    if (arg[1] == '-' && name != NULL) {
      return strcmp(name, arg);
    }

    if (arg[1] == sname) return 0;
  }

  return 1;
}

char* get_path(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    if(non_flag(argv[i]) == 0) {
      if(strcmp("-o", argv[i]) == 0) {
        i++;
      }
    } else {
      return argv[i];
    }

  }

  return NULL;
}

static char* set_output_name(char** argv, int idx) {
  if (strcmp(argv[idx], "-o") != 0) {
    size_t len = strlen(argv[idx]);
    char* parsed_name = (char*) malloc(sizeof(char) * len);
    strncpy(parsed_name, argv[idx] + 2, len -2);

    return parsed_name;
  }  

  return argv[idx + 1];
}

#define DEFAULT_NAME "main"

char* parse_flags(int argc, char** argv) {
  if (argc <= 1) {
    return NULL;
  }

  char* out = (char*) DEFAULT_NAME;

  for (int i = 1; i < argc; i++) {   
    if (argcmp('v', "--version", argv[i]) == 0) {
      version_app();
    }

    if (argcmp('o', NULL, argv[i]) == 0) {
      if (argc - i == 1) {
          printf("Error: The -o flag expects 1 argument.\n");
          return NULL;
      }
      /* intead of return add in global var */
      out = set_output_name(argv, i);
    }
  }

  return out;
}
