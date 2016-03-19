#include "login_cmd.h"

char *def_usr = "anonymous";

int
exec_user_cmd(char* params)
{
	free_session();	

	if(!strcmp(params, def_usr))
	{
		create_session(def_usr);
		send_proto(230, "User logged in proceed");	
	}
	else 
	{
		send_proto(530, "Invalid parameter: anonymous supported only!");
	}
	return 1;
}

int
exec_quit_cmd(char *params)
{
	if(!params_empty(params, 1))
		return 1;

	free_session();	
	send_proto(221, "Service closing control connection");
	return 0;
}
