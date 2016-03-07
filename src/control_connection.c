#include "control_connection.h"

int
exec_user_cmd(char *params);

int
exec_quit_cmd(char *params);

int
exec_list_cmd(char *params);

int
exec_pasv_cmd(char *params);


char *loc_adr;
char *cur_file;
int ipv4;

void 
send_proto(int code, char *message)
{
	printf("%d %s\n", code, message);
	fflush(stdout);
}

int
check_user(int(*cmd)(char*), char *params)
{
	if(user == NULL)
	{
		send_proto(530, "Please log in.");
		return 1;
	}
	return cmd(params);
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
	else if(!strcmp(token, "PASV"))
	{
		return check_user(exec_pasv_cmd, params);
	}
	else if(!strcmp(token, "LIST"))
	{
		return check_user(exec_list_cmd, params);
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
        if(argc != 2)
        {
        	dprintf(2, "usage: %s <ip address of interface client is connected to>\n", basename(argv[0]));
        	return 1;
        }
	cur_file = basename(argv[0]);
	ipv4 = 0 != strchr(argv[1], '.');
	loc_adr = argv[1];

	send_proto(220, "Service ready for anonymous user.");
	int success = 1; 
	while(success)
		success = process_cmd();

	if(trans_con)
		kill(trans_con->pid, 9);

	return 0;
}




