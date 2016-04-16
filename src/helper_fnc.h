#ifndef trim_h
#define trim_h

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>
#define _GNU_SOURCE
#include <stdio.h>

//trims string from left by white spaces string is edited
char *
rtrim(char *s);

//trims string from right by white spaces string is edited
char *
ltrim(char *s);


//trims string by white spaces string is edited
char *
trim(char *s);

//converts string to upper case string is edited
char *
str_upper(char *s);

//converts string to lower case string is edited
char *
str_lower(char *s);

//get absolute path of running cur_file and appends file to it (buffer is allocated)
char* 
get_abs_path(char *file, char *cur_file);

char *
remove_relatives(char *path);

int
is_absolute(char *path);

//trims path with leading and trailing slashes and remove duplicate shlases.  string is edited
char *
remove_slashes(char *path);

//params is string representing path, either absolute or relative in respect to cur_path and root_path
//remove relatives from params and returns path concatenated of root cur_path params (path is in respect to filesystem), buffer is allocated.
//to cur_path store new current path relative to root, buffer is allocated
char *
get_full_path(char *root, char  **cur_path, char *params);

// returns positive integer represnting string input. to error_res store whether error ocurred
int 
parse_int(char *input, int* error_res);
#endif
