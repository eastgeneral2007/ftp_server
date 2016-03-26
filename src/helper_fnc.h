#ifndef trin_h
#define trim_h

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

char *
rtrim(char *s);

char *
ltrim(char *s);

char *
trim(char *s);


char *
str_upper(char *s);

char *
str_lower(char *s);

char* 
get_abs_path(char *file, char *cur_file);

char *
remove_relatives(char *path);

int
is_absolute(char *path);

char *
remove_slashes(char *path);

char *
get_full_path(char *root, char  **cur_path, char *params);

int 
parse_int(char *input, int* error_res);
#endif
