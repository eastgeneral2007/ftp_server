#include "transport_cmd.h"

int
exec_stor_cmd(char *params)
{	
	if(session->trans_con == NULL)
	{
		send_proto(425, "Cant open data connection. User pasv command first."); 
		return 1;
	}
	
	char *cur = session->cur_path;	
	char *full_path = get_full_path(session->root_path, &cur, params);
	struct stat f_info;
	int stat_r = stat(full_path, &f_info);
	int f_desc;

	if(strlen(cur) > 0 /*not root dir */ && 
		((stat_r == -1 && errno == ENOENT ) || ( stat_r == 0 && S_ISREG(f_info.st_mode))) &&
		(-1 != (f_desc = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0770))))
	{
		send_proto(150, "File status okay, about to open data connection");

		close(session->trans_con->trans_in);
		char* buff[1024];
		int read_len;
		while((read_len = read(session->trans_con->trans_out, buff, 1024)) > 0 )
			write(f_desc, buff, read_len); 

		close(session->trans_con->trans_out);

		pid_t status_trans;
		pid_t y = waitpid(session->trans_con->pid, &status_trans, WCONTINUED);
		if( y == -1 || WEXITSTATUS(status_trans)) 
			send_proto(426, "Connection closed transfer aborted");
		else
			send_proto(226, "Closing data connection. Transfer complete");

		free_trans();
	}
	else
	{
		send_proto(550, "Requested action not taken.");
	}

	free(cur);
	free(full_path);
	return 1;	
}

int exec_retr_cmd(char *params)
{
	if(session->trans_con == NULL)
	{
		send_proto(425, "Cant open data connection. User pasv command first."); 
		return 1;
	}
	
	char *cur = session->cur_path;	
	char *full_path = get_full_path(session->root_path, &cur, params);
	struct stat f_info;
	int stat_r = stat(full_path, &f_info);
	int f_desc;

	if( stat_r == 0 && S_ISREG(f_info.st_mode) && -1 != (f_desc = open(full_path, O_RDONLY)))
	{
		send_proto(150, "File status okay, about to open data connection");

		char* buff[1024];
		int read_len;
		while((read_len = read(f_desc, buff, 1024)) > 0 )
			write(session->trans_con->trans_in, buff, read_len); 

		close(session->trans_con->trans_in);

		pid_t status_trans;
		pid_t y = waitpid(session->trans_con->pid, &status_trans, WCONTINUED);
		if( y == -1 || WEXITSTATUS(status_trans)) 
			send_proto(426, "Connection closed transfer aborted");
		else
			send_proto(226, "Closing data connection. Transfer complete");

		free_trans();
	}
	else
	{
		send_proto(550, "Requested action not taken.");
	}

	free(cur);
	free(full_path);
	return 1;	
}
