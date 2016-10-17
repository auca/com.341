#ifndef ISH_SHELL_UTILITIES_H
#define ISH_SHELL_UTILITIES_H

#include "ish_common.h"
#include "ish_cstring_utilities.h"

static const char ISH_Standard_Input_Redirection_Lexeme[]  = "<";
static const char ISH_Standard_Output_Redirection_Lexeme[] = ">";

/*
    Utility functions to search through environment variables,
    process arguments and argument strings.
*/

unsigned long ish_build_argument_array_from_input(
                  char *input,
                  char **arguments,
                  unsigned long max_argument_count
              );

char* ish_get_first_environment_variable(
          const char *key, unsigned long key_length,
          char **environment
      );

char* ish_extract_last_stdin_redirection_file(
          char **arguments
      );

char* ish_extract_last_stdout_redirection_file(
          char **arguments
      );

#endif
