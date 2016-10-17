#ifndef ISH_CSTRING_UTILITIES_H
#define ISH_CSTRING_UTILITIES_H

#include "ish_common.h"

static const char ISH_Directory_Separator = '/';
static const char ISH_Path_Separator      = ':';

/*
    Utility functions similar to those in the standard
    library to break the dependency on it.
*/

unsigned long ish_get_cstring_length(
                  const char *string
              );

char *ish_replace_first_character_in_cstring(
          char *string,
          char character,
          char replacement
      );

int ish_is_space(int character);
int ish_is_digit(int character);
int ish_is_path_separator(int character);

int ish_is_lowercase(int character);
int ish_is_uppercase(int character);
int ish_convert_to_lowercase(int character);
int ish_convert_to_uppercase(int character);

char* ish_carve_token_in_cstring(
          char *string,
          int (*is_separator)(int character),
          char **cursor_after_token
      );

char* ish_get_token_in_cstring(
          char *string,
          int (*is_separator)(int character),
          char **cursor_after_token
      );

int ish_cstring_starts_with(
        const char *prefix,
        const char *string
    );

int ish_cstring_starts_with_ignoring_case(
        const char *prefix,
        const char *string
    );

int ish_are_cstrings_equal(
        const char *string,
        const char *another_string
    );

int ish_are_cstrings_equal_ignoring_case(
        const char *string,
        const char *another_string
    );

int ish_get_integer_from_cstring(
        const char *string
    );

void ish_combine_path_elements(
         const char *path_element,
         const char *another_path_element,
         char *combined_path,
         unsigned long maximum_combined_path_length
     );

#endif
