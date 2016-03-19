#include "browse_cmd.h"

char * cur_path;
char * root_path;

int
verifi_ex_dir(char *full_abs_path)
{
	printf("%s\n", full_abs_path);	

	struct stat info;
	if(-1 == stat(full_abs_path, &info))
		return 0;
	
	return S_ISDIR(info.st_mode);
}

int
exec_list_cmd(char *params)
{
	if(session->trans_con == NULL)
	{
		send_proto(425, "Cant open data connection. User pasv command first."); 
		return 1;
	}

	char *c_path = session->cur_path;
	char *ls_param = get_full_path(session->root_path,  &c_path, params);
	free(c_path);

	if(!verifi_ex_dir(ls_param))
	{
		free(ls_param);
		send_proto(550, "Requested action not taken.");
		return 1;
	}

	pid_t son;
	if((son = fork()) == 0)//producent
	{	
		close(1); 
		dup(session->trans_con->trans_in); 
		execl("/bin/ls", "ls", "-la", ls_param, NULL);
		//TODO error outputs of ls
	}
	close(session->trans_con->trans_in);
	free(ls_param);
	
	int status_prod = -1, status_trans = -1;
	pid_t x = waitpid(son, &status_prod, WCONTINUED);
	
	pid_t y = x == -1 || WEXITSTATUS(status_prod) ? -1 : waitpid(session->trans_con->pid, &status_trans, WCONTINUED);

	if(x == -1 || y == -1 ||  WEXITSTATUS(status_prod) || WEXITSTATUS(status_trans)) 
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
exec_pwd_cmd(char *params)
{
	if(!params_empty(params))
		return 1;

	char str[] = "\"%s\" path.";
	char *txt = malloc(strlen(str) + strlen(session->cur_path) + 1);
	sprintf(txt, str, session->cur_path);
	
	send_proto(257, txt);
	free(txt);
	return 1;
}



int
exec_cwd_cmd(char *params)
{
	char *cur = session->cur_path;
	char *full_abs_path = get_full_path(session->root_path, &cur, params);
	if(verifi_ex_dir(full_abs_path))
	{
		free(session->cur_path);
		session->cur_path = cur;
		send_proto(112, "Requested file action completed. OK");
	}
	else
	{
		free(cur);
		send_proto(550, "Requested action not taken.");
	}
	free(full_abs_path);

	return 1;
}

int
exec_cdup_cmd(char *params)
{
	if(!params_empty(params))
		return 1;

	exec_cwd_cmd("..");
	return 1;
}
