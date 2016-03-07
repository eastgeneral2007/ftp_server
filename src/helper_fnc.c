#include "helper_fnc.h"

char *
ltrim(char *s)
{
	if(!s) return s;

	while(isspace(*s)) s++;
	return s;
}

char *
rtrim(char *s)
{
	if(!s) return s;

	char* back = s + strlen(s);
	while(isspace(*--back));
	*(back+1) = '\0';
	return s;
}

char *
trim(char *s)
{
	return rtrim(ltrim(s)); 
}

char *
str_action(char *s, int(*convert)(int))
{
	if(!s) return s;	

	char *i;
	for(i = s; i - s < strlen(s); i++) 
		*i = convert(*i);

	return s;
}

char *
str_upper(char *s)
{
	return str_action(s, toupper);
}

char *
str_lower(char *s)
{
	return str_action(s, tolower);
}



char* 
get_abs_path(char *file, char *cur_file)
{
	char *cur_path = "/proc/self/exe";
	char *abs_path = malloc(PATH_MAX);
	realpath(cur_path, abs_path);
	strcpy(abs_path + strlen(abs_path) -  strlen(cur_file), file);
	*(abs_path + strlen(abs_path) + strlen(file) + 1) = '\0';	
	return abs_path;
}
