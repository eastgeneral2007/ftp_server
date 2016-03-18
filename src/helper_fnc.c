#include "helper_fnc.h"

char *
ltrim_char(char *s, int(*is_char)(int))
{
	if(!s) 
		return s;

	while(is_char(*s)) 
		s++;
	return s;
}

char *
rtrim_char(char *s, int(*is_char)(int))
{
	if(!s) 
		return s;

	char* back = s + strlen(s);
	while(is_char(*--back));
	*(back+1) = '\0';

	return s;
}

char *
ltrim(char *s)
{
	return ltrim_char(s, isspace);
}

char *
rtrim(char *s)
{
	return rtrim_char(s, isspace);
}

char *
trim(char *s)
{
	return rtrim(ltrim(s)); 
}

char *
trim_char(char *s, int(*is_char)(int))
{
	return rtrim_char(ltrim_char(s, is_char), is_char);
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

int
is_absolute(char *path)
{
	return strlen(path) > 0 && *path == '/';	
}

int
isslash(int c)
{
	return c == '/';
}

//trim path withi " " and "/" slash a then remove duplicate slashes (string is modifed)
char *
remove_slashes(char *path)
{
	char *r_path = trim(trim_char(path, isslash));
	char *res = malloc(strlen(r_path) + 1);
	char *p = r_path;
	char *d = res; 
	int prev_slash = 0;

	while(*p != 0)
	{
		if(*p != '/')
		{
			*d = *p;
			d++;
			prev_slash = 0;
		}
		else if(*p == '/')
		{
			if(!prev_slash)
			{
				*d = *p;
				d++;
			}
			prev_slash = 1;
		}
		p++;
	}
	*d = *p;
	strcpy(path, res);
	free(res);
	return path;
}

//make path from dir_list, allocat burffer
char *
dir_lst_to_path(char **dir_list)
{
	char **i = dir_list;
	size_t len = 0;
	while(*i != (char*)NULL)
		len += strlen(*i++) + 1;//for slashes or \0
	
	char *res = malloc(len);
	strcpy(res, "");
	i = dir_list;
	while(*i != (char*)NULL)
	{
		strcat(res, *i);
		strcat(res, "/");	
		i++;
	}

	*(res + strlen(res) - 1) = '\0'; //remove last trailing slash
	return res;
}

//Expect normalized(no multiple slashes) trimmed path, allocat buffer for result
char *
remove_relatives(char *path)
{
	char *seg_ptr_end = path;

	char **dirs = malloc((strlen(path) + 1) * sizeof(char**));
	char **seg_ptr;
	for(seg_ptr = dirs; ; seg_ptr++)
	{
		int end = 0;
		*seg_ptr = seg_ptr_end;

		while(*seg_ptr_end != '/' && *seg_ptr_end != '\0')
			seg_ptr_end++;

		if(*seg_ptr_end != '\0')
			*seg_ptr_end = '\0';
		else
			end = 1;

		seg_ptr_end++;//skip '\0' char
		
		if(!strcmp(*seg_ptr, ".")) //==
		{
			seg_ptr--; //anull this iteration
		}
		else if(!strcmp(*seg_ptr, "..")) //==
		{
			seg_ptr -= 2;
			if(seg_ptr < dirs - 1)
				return NULL;
		}

		if(end)
		{
			*(++seg_ptr) = NULL;//Terminate ptrs array
			break;
		}
	}
	char *res = dir_lst_to_path(dirs);
	free(dirs);
	return res;
}

char *
get_full_path(char *root, char  **cur_path, char *params)
{
	char *c_path;
	if(is_absolute(params))
	{
		c_path = malloc(strlen(params));
		strcpy(c_path, params);
	}
	else
	{
		c_path = malloc(strlen(*cur_path) + 1 + strlen(params));
		strcpy(c_path, *cur_path);
		strcat(c_path, "/");
		strcat(c_path, params);
	}
	
	char *normalized_path = remove_relatives(remove_slashes(c_path));
	free(c_path);
	*cur_path = normalized_path;

	if(normalized_path)
	{
		char *res = malloc(strlen(root) + strlen(normalized_path) + 1);
		strcpy(res, root);
		strcat(res, normalized_path);
		return res;
	}
	else
	{
		return NULL;
	}
}
