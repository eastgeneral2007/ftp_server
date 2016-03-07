#ifndef trin_h
#define trim_h

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

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

#endif
