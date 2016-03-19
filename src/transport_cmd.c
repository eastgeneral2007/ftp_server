#include "transport_cmd.h"

int
pasv_error(void)
{
	send_proto(421, "Service not avalible, error internal error ocured");
	return 1;
}

int
reply_pasv(void)
{ 
	char *ip4 = strrchr(loc_adr, ':') + 1;
	char ip_f[strlen(ip4) + 1];
	char *s = strcpy(ip_f, ip4);
	while( 0 != (s = strchr(s, '.')))
		*s = ',';		

	char message[255]; 
	if(ipv4)
	{
		unsigned short x = session->trans_con->port;
		snprintf(message, 255, "Entering pasive mode. %d (%s,%d,%d)", x, ip_f, *((unsigned char*)&x + 1), *((unsigned char*)&x));
		send_proto(227, message);
	}
	else
	{ 
		//TODO set message
		send_proto(229, message); 
	}
	return 1;
}
	
	
void	
free_trans(void)
{
	if(session->trans_con)
	{
		kill(session->trans_con->pid, 9);
		close(session->trans_con->trans_in);
		close(session->trans_con->trans_out);
		free(session->trans_con);
	}
	session->trans_con = NULL;
}

int
exec_pasv_cmd(char *params)
{	
	if(!params_empty(params))
		return 1;

	free_trans();	

	int parent[2], trans_in[2], trans_out[2];
	pipe(parent); pipe(trans_in); pipe(trans_out);

	session->trans_con = malloc(sizeof(struct trans_con));
	if((session->trans_con->pid = fork()) == 0)
	{
		close(0);
		dup(trans_in[0]);
		close(trans_in[1]);

		close(1);
		dup(trans_out[1]);
		close(trans_out[0]);

		char *file = "transport.out"; 
		char d[10];
		snprintf(d, 10, "%d", parent[1]);
		close(parent[0]);
		execl(get_abs_path(file, cur_file), file, loc_adr, d, (char *)NULL);
	}
	
	if(session->trans_con->pid == -1)
		return pasv_error();

	close(parent[1]);	
	close(trans_in[0]);
	close(trans_out[1]);
	
	session->trans_con->trans_in = trans_in[1];
	session->trans_con->trans_out = trans_out[0];
	
	if(!read(parent[0], &session->trans_con->port, sizeof(session->trans_con->port)))
		return pasv_error();
	//dprintf(2, "port-> %d\n", session->trans_con->port);
	return reply_pasv(); 
}
