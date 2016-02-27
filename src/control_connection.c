#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	dprintf(2, "connection established\n");
	
	char buff[3];
	FILE *input = fdopen(0, "r");
	
	char *s = NULL;	
	do	
		dprintf(2, s = fgets(buff, sizeof(buff), input));
	while(s != NULL);

	dprintf(2, "closing connection\n");
}
