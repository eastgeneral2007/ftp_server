#include "control_connection.h"

int
exec_user_cmd(char *params);

void 
send_proto(int code, char *message)
{
	printf("%d %s\n", code, message);
}


int
process_verify_cmd(char* expec_cmd)
{
	size_t n = 0;
	char *cmd =  NULL;
	if ( -1 == getline(&cmd, &n, fdopen(0, "r")))
		return 0;

	char* params = cmd; 
	char* token = strsep(&params, " \n");

	if(expec_cmd != NULL && !strcmp(token,expec_cmd))
		return 0;

	if(!strcmp(token, "USER")) // 0 -> equals
		return exec_user_cmd(params);
	else
		return 0;

	free(cmd);
}

int
process_cmd()
{
	return process_verify_cmd(NULL);
}


int
main(int argc, char *argv[])
{
	send_proto(220, "Service ready for anonymous user.");
	int success = 1; 
	while(success)
		success = process_cmd();
	return 0;

	//dprintf(2, "connection established\n");
	//char buff[3];
	//FILE *input = fdopen(0, "r");
	//char *s = NULL;	
	//do	
	//	dprintf(2, s = fgets(buff, sizeof(buff), input));
	//while(s != NULL);
	//dprintf(2, "closing connection\n");
}


