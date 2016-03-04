#include "login_cmd.h"

int
exec_user_cmd(char* params)
{
	if(strcmp(params, "anonymous"))
	{
		send_proto(230, "User logged in proceed");	
		return 1;
	}
	else 
	{
		send_proto(530, "Invalid parameter");
		return 0;
	}
}

