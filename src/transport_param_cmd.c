#include "transport_param_cmd.h"
#define ER(x) if(-1 == x) exit(1);

int
pasv_error(void)
{
	send_proto(421, "Service not avalible, error internal error ocured");
	return 1;
}

int
error_close(int p1[], int p2[], int p3[])
{
	close(p1[0]);
	close(p1[1]);
	close(p2[0]);
	close(p2[1]);
	close(p3[0]);
	close(p3[1]);
	free_trans();
	pasv_error();
	return 0;
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
setup_pasv(void)
{	

	free_trans();	

	int r0, r1, r2; 
	int parent[2], trans_in[2], trans_out[2];
	r0 = -1 == pipe(parent); 
	r1 = -1 == pipe(trans_in); 
	r2 = -1 == pipe(trans_out);
	if(r0 || r1 || r2)
		return error_close(parent, trans_in, trans_out);

	session->trans_con = malloc(sizeof(struct trans_con));
	if((session->trans_con->pid = fork()) == 0)
	{
		ER(close(0))
		ER(dup(trans_in[0]))
		ER(close(trans_in[1]))

		ER(close(1))
		ER(dup(trans_out[1]))
		ER(close(trans_out[0]))

		char *file = "transport.out"; 
		char d[10];
		snprintf(d, 10, "%d", parent[1]);
		ER(close(parent[0]))
		execl(get_abs_path(file, cur_file), file, loc_adr, d, (char *)NULL);
	}
	
	if(session->trans_con->pid == -1)
		return error_close(parent, trans_in, trans_out);

	close(parent[1]);	
	close(trans_in[0]);
	close(trans_out[1]);
	
	session->trans_con->trans_in = trans_in[1];
	session->trans_con->trans_out = trans_out[0];
	
	if(!read(parent[0], &session->trans_con->port, sizeof(session->trans_con->port)))
		return error_close(parent, trans_in, trans_out);
	return 1;
}

int
exec_pasv_cmd(char *params)
{
	if(!params_empty(params, 1))
		return 1;
	if(!ipv4)
		return pasv_error();

	setup_pasv();

	if(!session->trans_con)
		return 1;

	char *ip4 = strrchr(loc_adr, ':') + 1;
	char ip_f[strlen(ip4) + 1];
	char *s = strcpy(ip_f, ip4);
	while( 0 != (s = strchr(s, '.')))
		*s = ',';		

	char message[255]; 
	unsigned short x = session->trans_con->port;
	snprintf(message, 255, "Entering pasive mode. %d (%s,%d,%d)", x, ip_f, *((unsigned char*)&x + 1), *((unsigned char*)&x));
	send_proto(227, message);
	return 1;
}

int
exec_epsv_cmd(char *params)
{
	if(!params_empty(params, 1))
		return 1;
	if(ipv4)
		return pasv_error();

	setup_pasv();

	if(!session->trans_con)
		return 1;
	char message[255]; 
	snprintf(message, 255, "Entering pasive mode. (|||%d|)", session->trans_con->port);
	send_proto(229, message); 
	return 1;
}
