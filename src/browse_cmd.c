#include "browse_cmd.h"

char * cur_path;

int
exec_list_cmd(char *params)
{
	if(trans_con == NULL)
	{
		send_proto(425, "Cant open data connection. User pasv command first."); 
		return 1;
	}

	pid_t son;
	if((son = fork()) == 0)//producent
	{	
		close(1); 
		dup(trans_con->trans_in); 
		//dprintf(2, "echo output to %d\n", trans_con->trans_in);
		execl("/bin/ls", "ls", params, NULL);
		//execl("/bin/echo", "echo", params, NULL);
		//TODO error outputs of ls
	}
	close(trans_con->trans_in);
	
	int status_prod, status_trans;
	pid_t x = waitpid(son, &status_prod, WCONTINUED);
	pid_t y = waitpid(trans_con->pid, &status_trans, WCONTINUED);

	if(x == -1 || y == -1 || status_prod || status_trans) 
		send_proto(426, "Connection closed transfer aborted");
	else
		send_proto(226, "Closing data connection. Transfer complete");

	free_trans();
	return 1;	
}

int
exec_mlsd_cmd(char *params)
{
	return 1;
}

int
exec_cwd_cmd(char *params)
{
	return 1;
}

