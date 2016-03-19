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

void
dprintf_date(int desc, char *str_format, struct timespec *file_time)
{
	struct tm t;
	localtime_r(&file_time->tv_sec, &t);
	char str_t[15]; // = "YYYYmmddHHMMSS"; 4 + 2 + 2 + 2 +2 + 2 + 1 = 15
	strftime(str_t, 15, "%Y%m%d%H%M%S", &t);
	dprintf(desc, str_format, str_t);
}

void
dprintf_type(int desc, struct stat *f_info, char *filename)
{
	char *type;
	if(!strcmp(filename, "."))
		type = "cdir";
	else if(!strcmp(filename, ".."))
		type = "pdir";
	else if(S_ISDIR(f_info->st_mode))
		type = "dir";
	else if(S_ISREG((f_info->st_mode)))
	{
		type = "file";
		dprintf(desc, "size=%ld;", f_info->st_size);
	}
	else 
		type = "unspec";

	dprintf(desc, "type=%s;", type);
}

void
write_file_info(int desc, struct stat *file_info, char *filename)
{
	dprintf_date(desc, "create=%s;", &file_info->st_ctim);
	dprintf_date(desc, "modify=%s;", &file_info->st_mtim);
	dprintf_type(desc, file_info, filename); 
	dprintf(desc, " %s\n", filename);	
}

//expecting valid dir path//writes data and close pipe
void
write_mlsd_data(int desc, char * dir_path)
{
	struct stat file_info;
	struct dirent *dir_item;
	DIR * dir_stream = opendir(dir_path);

	while(NULL != (dir_item = readdir(dir_stream)))//TODO what about errno ? ? 
	{
		char * file_path = malloc(strlen(dir_path) + 1 + strlen(dir_item->d_name) + 1);
		strcpy(file_path, dir_path);
		strcat(file_path, "/");
		strcat(file_path, dir_item->d_name);
		int stat_res = stat(file_path, &file_info);
		free(file_path);
		if(-1 != stat_res)
			write_file_info(desc, &file_info, dir_item->d_name);
	}
	close(desc);
}

int
exec_mlsd_cmd(char *params)
{
	if(session->trans_con == NULL)
	{
		send_proto(425, "Cant open data connection. User pasv command first."); 
		return 1;
	}

	char *c_path = session->cur_path;
	char *dir_path = get_full_path(session->root_path,  &c_path, params);
	free(c_path);

	if(!verifi_ex_dir(dir_path))
	{
		free(dir_path);
		send_proto(550, "Requested action not taken.");
		return 1;
	}

	write_mlsd_data(session->trans_con->trans_in, dir_path); 

	free(dir_path);
	
	int status_trans = -1;
	
	pid_t y = waitpid(session->trans_con->pid, &status_trans, WCONTINUED);

	if( y == -1 || WEXITSTATUS(status_trans)) 
		send_proto(426, "Connection closed transfer aborted");
	else
		send_proto(226, "Closing data connection. Transfer complete");

	free_trans();
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
