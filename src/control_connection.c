#include "control_connection.h"

int
exec_user_cmd(char *params);

int
exec_quit_cmd(char *params);

void 
send_proto(int code, char *message)
{
	printf("%d %s\n", code, message);
}

int
select_cmd(char *token, char *params)
{
	if(!strcmp(token, "USER")) // 0 -> equals
	{
		return exec_user_cmd(params);
	}
	else if(!strcmp(token, "QUIT"))
	{
		return exec_quit_cmd(params);
	}
	else
	{
		send_proto(202, "Command not implemented, superfluous at this site");
		return 1;
	}
}



int
process_verify_cmd(char* expec_cmd)
{
	int res = 1;
	size_t n = 0;
	char *cmd =  NULL;
	if ( -1 == getline(&cmd, &n, fdopen(0, "r")))//TODO fdopen res
		res =  0;

	char* params = cmd; 
	char* token = str_upper(strsep(&params, " \v\f\r\n\t"));

	if(expec_cmd != NULL && !strcmp(token,expec_cmd))
		res = 0;

	res = res && select_cmd(token, trim(params));	
	free(cmd);
	return res;
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
	{
		success = process_cmd();
	}
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


