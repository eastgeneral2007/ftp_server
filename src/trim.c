#include "trim.h"

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



