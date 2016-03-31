#include "dir_cmd.h"

int
exec_mkd_cmd(char *params)
{
	if(params_empty(params, 0))
		return 1;

	char *cur = session->cur_path;
	char *full_path = get_full_path(session->root_path, &cur, params);

	if( -1 != mkdir(full_path, 0770))
	{
		send_path_proto(cur);
	}
	else
	{
		send_proto(550, "Requested action not taken.");
	}
	free(cur);
	free(full_path);

	return 1;
}

int
exec_rmd_cmd(char *params)
{
	if(params_empty(params, 0))
		return 1;

	char *cur = session->cur_path;
	char *full_path = get_full_path(session->root_path, &cur, params);

	if( -1 != rmdir(full_path))
	{
		send_proto(250, "Requested file action okay, completed.");
	}
	else
	{
		send_proto(550, "Requested action not taken.");
	}
	free(cur);
	free(full_path);

	return 1;

}
