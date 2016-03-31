#include "file_cmd.h"

char *rnfrom = NULL;

int
exec_rnfr_cmd(char *params)
{
	struct stat info;
	char *cur = session->cur_path;
	char *full_path = get_full_path(session->root_path, &cur, params);
	
	if(strlen(cur) > 0 && -1 != stat(full_path, &info))
	{
		rnfrom = full_path;	
		send_proto(350, "Requested file action pending further informations.");
		session->next_seq_cmd = "RNTO";
	}
	else
	{
		free(full_path);
		send_proto(550, "Requested action not taken");	
	}

	free(cur);
	return 1;	
}

int 
exec_rnto_cmd(char *params)
{
	if(rnfrom == NULL)
	{
		send_proto(501, "Bad sequence of command.");
		return 1;
	}

	session->next_seq_cmd = NULL;

	if(params_empty(params, 0))
		return 1;

	char *cur = session->cur_path;
	char *full_path = get_full_path(session->root_path, &cur, params);
	if( -1 != rename(rnfrom, full_path))
	{
		send_proto(250, "Requested file action complete.");
	}
	else
	{
		send_proto(553, "Requested action not taken.");

	}

	free(cur);	
	free(full_path);
	free(rnfrom);
	rnfrom = NULL;
	return 1;
}

int 
exec_dele_cmd(char *params)
{
	if(params_empty(params, 0))
		return 1;
	
	char *cur = session->cur_path;
	char *full_path = get_full_path(session->root_path, &cur, params);
	struct stat info;

	if(strlen(cur) > 0 && -1 != stat(full_path, &info) && !S_ISDIR(info.st_mode) && -1 != remove(full_path))
		send_proto(250, "Requested file action complete.");
	else
		send_proto(550, "Requested action not taken");

	free(full_path);
	free(cur);	
	return 1;
}
