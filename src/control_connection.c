#include "control_connection.h"
#include "commands.h"

void
free_trans(void);


char *loc_adr;
char *cur_file;
int ipv4;
struct session *session;

void
create_session(char *user)
{
	session = malloc(sizeof(struct session));
	session->user = malloc(strlen(user) + 1);
	strcpy(session->user, user);   
	session->cur_path = malloc(1);
	*session->cur_path = '\0';
	session->root_path = get_abs_path("root/", cur_file);
	session->trans_con = NULL;
	session->next_seq_cmd = NULL;
}

void
free_session(void)
{
	if(session)
	{
		if(session->trans_con)
			free_trans();
		free(session->root_path);
		free(session->cur_path);
		free(session->user);
		free(session);
		session = NULL;
	}
}

void 
send_proto(int code, char *message)
{
	printf("%d %s\n", code, message);
	fflush(stdout);
}

int
params_empty(char *params, int empty_is_valid)
{
	int res = !strcmp(trim(params), "");

	if(res ^ empty_is_valid)
		send_proto(501, "Invalid parameters" ); 

	return res;
}

int
check_user(int(*cmd)(char*), char *token, char *params)
{
	if(session == NULL)
	{
		send_proto(530, "Please log in.");
		return 1;
	}

	if(session->next_seq_cmd != NULL && strcmp(session->next_seq_cmd, token))
	{
		send_proto(501, "Bad sequence of commands.");
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
		return check_user(exec_pasv_cmd, token, params);
	}
	else if(!strcmp(token, "EPSV"))
	{
		return check_user(exec_epsv_cmd, token, params);
	}
	else if(!strcmp(token, "LIST"))
	{
		return check_user(exec_list_cmd, token, params);
	}
	else if(!strcmp(token, "MLSD"))
	{
		return check_user(exec_mlsd_cmd, token, params);
	}
	else if(!strcmp(token, "PWD"))
	{
		return check_user(exec_pwd_cmd, token, params);
	}
	else if(!strcmp(token, "CWD"))
	{
		return check_user(exec_cwd_cmd, token, params);
	}
	else if(!strcmp(token, "CDUP"))
	{
		return check_user(exec_cdup_cmd, token, params);
	}
	else if(!strcmp(token, "MKD"))
	{
		return check_user(exec_mkd_cmd, token, params);
	}
	else if(!strcmp(token, "RMD"))
	{
		return check_user(exec_rmd_cmd, token, params);
	}
	else if(!strcmp(token, "RNFR"))
	{
		return check_user(exec_rnfr_cmd, token, params);
	}
	else if(!strcmp(token, "RNTO"))
	{
		return check_user(exec_rnto_cmd, token, params);
	}
	else if(!strcmp(token, "DELE"))
	{
		return check_user(exec_dele_cmd, token, params);
	}
	else if(!strcmp(token, "STOR"))
	{
		return check_user(exec_stor_cmd, token, params);
	}
	else if(!strcmp(token, "RETR"))
	{
		return check_user(exec_retr_cmd, token, params);
	}
	else
	{
		send_proto(202, "Command not implemented, superfluous at this site");
		return 1;
	}
}


FILE *stream; 

int
process_verify_cmd(char* expec_cmd)
{
	int res = 1;
	size_t n = 0;
	char *cmd =  NULL;
	if ( -1 == getline(&cmd, &n, stream))
	{
		free(cmd);
		return  0;
	}

	char* params = cmd; 
	char* token = str_upper(strsep(&params, " \v\f\r\n\t"));

	if(expec_cmd != NULL && !strcmp(token,expec_cmd))
	{
		free(cmd);
		return  0;
	}

	res = res && select_cmd(token, trim(params));	
	free(cmd);
	return res;
}

void
send_path_proto(char *rel_path)
{
	char str[] = "\"/%s\" created.";
	char *txt = malloc(strlen(str) + strlen(rel_path) + 1);
	sprintf(txt, str, rel_path);
	
	send_proto(257, txt);
	free(txt);
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
		dprintf(2, "usage: %s <ip_address_of_interface_client_is_connected_to>\n", basename(argv[0]));
        	return 1;
        }
	cur_file = basename(argv[0]);
	ipv4 = 0 != strchr(argv[1], '.');
	loc_adr = argv[1];

	stream = fdopen(0, "r");
	if(!stream)
	{
		send_proto(421, "Service not available.");
		return 1;
	}

	send_proto(220, "Service ready for anonymous user.");
	int success = 1; 
	while(success)
		success = process_cmd();

	if(session) 
		free_session();

	fclose(stream);
	return 0;
}


