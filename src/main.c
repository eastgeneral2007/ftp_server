#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *
remove_slashes(char *path)
{
	char *res = malloc(sizeof(path));
	char *p = path;
	char *d = res; 

	while(*p != 0)
	{
		if(*p != '/')
		{
			*d = *p;
			d++;
		}
		p++;
	}
	*d = *p;
	return res;
}

int
main(int argc, char *argv[])
{
	printf("%s\n", remove_slashes("////aaa/aa/a//a//aa////////////aaa/aaaa/aaaaa/aaaa/aaa/////////////aaa/"));
	printf("%s\n", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	printf("%s\n", remove_slashes("////aaa/aa/a//a//aa////////////aaa/aaaa/aaaaa/aaaa/aaa/////////////aaa/"));
	printf("%s\n", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	printf("%s\n", remove_slashes("////aaa/aa/a//a//aa////////////aaa/aaaa/aaaaa/aaaa/aaa/////////////aaa/"));
	printf("%s\n", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	printf("%s\n", remove_slashes("////aaa/aa/a//a//aa////////////aaa/aaaa/aaaaa/aaaa/aaa/////////////aaa/"));
	printf("%s\n", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	printf("%s\n", remove_slashes("////aaa/aa/a//a//aa////////////aaa/aaaa/aaaaa/aaaa/aaa/////////////aaa/"));
	printf("%s\n", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

}
