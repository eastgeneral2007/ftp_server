#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#include <arpa/inet.h>

#include "helper_fnc.h"

int par_desc, transport = 0;
char *cur_file = NULL, *adr = NULL;

//gets ipaddress of interface client is connected to
char*
get_loc_adr(int desc)
{
	struct sockaddr cur_adr;	
	socklen_t len = sizeof(struct sockaddr_in6); 
	
	if( -1 == getsockname(desc, &cur_adr, &len))
		err(1, "cannott read socked addr struct");

	char *res = malloc(INET6_ADDRSTRLEN);

	if(!inet_ntop(AF_INET6, &((struct sockaddr_in6*)&cur_adr)->sin6_addr, res, INET6_ADDRSTRLEN))
		err(1, "inet_ntop fucky me up");

	return res;
}

//returns port socket is listening on 
char *
get_port(int socketDescriptor)
{
	struct sockaddr cur_adr;	
	socklen_t len =sizeof(struct sockaddr); 
	
	if( -1 == getsockname(socketDescriptor , &cur_adr, &len))
		err(1, "cannott read socked addr struct");

	char *res = malloc(transport ? NI_MAXSERV : INET6_ADDRSTRLEN);

	//if(!inet_ntop(AF_INET6, &((struct sockaddr_in6*)&cur_adr)->sin6_addr, res, INET6_ADDRSTRLEN))
	//	err(1, "inet_ntop fucky me up");

	int er = getnameinfo(&cur_adr, len, NULL, 0, res, sizeof(res), 0);
	if(er)	
		printf( gai_strerror(er));
	
//	printf("res: %s\n", res);
	return res; 
}

//read/write data from pipes to socket
void
transfer_data(int socketDescriptor)
{
	int chanDesc;
	if( (chanDesc = accept(socketDescriptor, NULL, 0)) == -1 )
		err(1, "unable accept connection");
	int input_readed = 0;
	char* buff[1024];
	int read_len;
	//dprintf(2, "Reading from stdin -> network\n");
	while((read_len = read(0, buff, 1024)) > 0 )//TODO direction of comunication
	{
		//dprintf(2, "Writing...\n");
		input_readed = 1;
		write(chanDesc, buff, read_len); //TODO error check
	}

	if(read_len == -1)
		err(1, "error while reding from soocket" );
	
	if(input_readed)//if perform only one direction comunication !
		exit(0);

	//dprintf(2, "Writing...skipped\n");
	//dprintf(2, "Reading from network -> stdout\n");
	while((read_len = read(chanDesc, buff, 1024)) > 0 )//TODO direction of comunication
	{
		//dprintf(2, "Writing...\n");
		write(1, buff, read_len); //TODO error check
	}

	if(read_len == -1)
		err(1, "error while reding from soocket" );

	
	close(chanDesc);
	close(socketDescriptor);
	exit(0);
}

//accepts connections of clients and forks theirs control connections
void
accept_connections(int socketDescriptor)
{
	char  *file = "control_connection.out";
	for(;;)
	{
		int chanDesc;
		if( (chanDesc = accept(socketDescriptor, NULL, 0)) == -1 )
			err(1, "unable accept connection");//TODO

		switch(fork())
		{
			case -1:
				err(1, "cannot fork"); //TODO
				break;
			case 0: 
				close(0);
				close(1);	
				char *adr = get_loc_adr(chanDesc);
				dup(chanDesc);
				dup(chanDesc); close(chanDesc);
				execl(get_abs_path(file, cur_file), file, adr, (char*)NULL);
				break;
			default:
				close(chanDesc);				
				break;
		}
	}
}




//sends port number trasport is listening on to parent process
void
report_port(int socketDescriptor)
{
	char *port = get_port(socketDescriptor);
	unsigned short p = (unsigned short)atoi(port);
	
	//dprintf(2, "%s : %d", port, p);
	write(par_desc, &p, sizeof(unsigned short));
        close(par_desc);
        free(port);
}


//inicialize dual stack socket
int
init_dualstack(struct addrinfo *addr_res)
{	int socketDescriptor = -1;
	struct protoent *tcp_ent = getprotobyname("tcp");
	if(!tcp_ent)
		err(1, "unable to find tcp flag" );
	
	struct addrinfo *s;
	for(s = addr_res; s != NULL; s = s->ai_next)
	{	
		if( s->ai_family == AF_INET6 && s->ai_protocol == tcp_ent->p_proto)
		{

//			char *res = malloc(INET6_ADDRSTRLEN);
//			getnameinfo(s->ai_addr, s->ai_addrlen, res, sizeof(res), NULL, 0, NI_NUMERICHOST);
//			printf(res);
//


			socketDescriptor = socket(s->ai_family, s->ai_socktype, s->ai_protocol);
			if(socketDescriptor == -1 )
				err(1, "unable create socket");
	
			int opt = 1, opt1 = 0;
			if(setsockopt(socketDescriptor, IPPROTO_IPV6, IPV6_V6ONLY, &opt1, sizeof(opt1)) == -1 ) 
				err(1, "unable set dual-stack socket" );

			if(setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1 )
				err(1, "unable set reusable socket" );


			if(bind(socketDescriptor, s->ai_addr, s->ai_addrlen) == -1)
				err(1, "unable bind socket");
		}
	}
	return socketDescriptor;
}

//run the server either in transport on server mode 
void
startServer(char * port)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	
	struct addrinfo *addr_res;	
	if(getaddrinfo(transport ? adr : NULL, transport ? NULL : (port ? port : "ftp"), &hints, &addr_res))
		err(1, "cant get addrinfo" );
	int socketDescriptor = init_dualstack(addr_res);
	freeaddrinfo(addr_res);

	//((struct sockaddr_in6*)addr_res->ai_addr)
	if(listen(socketDescriptor , SOMAXCONN) == -1)
		err(1, "unable start listening socket");
	
	if(transport)
	{
		report_port(socketDescriptor);
		transfer_data(socketDescriptor);
	}
	else
	{
		accept_connections(socketDescriptor);
	}

	close(socketDescriptor);
}

int
main(int argc, char* argv[])
{
	cur_file = basename(argv[0]);
	transport = !strcmp(cur_file, "transport.out");
	if(transport)
	{
		int error;
		par_desc = parse_int(argv[2], &error);
		if(argc != 3 || error)
		{
			dprintf(2, "usage: transport.out <interface_adr> <parent_comunication_descriptor>\n");
			exit(1);
		}
		else
		{
			adr = argv[1];
			startServer(NULL);
		}
	}
	else
	{
		if(argc > 2 || !strcmp(argv[0], "server.out"))
		{
			dprintf(2, "usage: server.out [<listening_port>]\n");
			exit(1);
		}
		else if( argc == 2)
			startServer(argv[1]);
		else
			startServer(NULL);
	}

	return (0);
}

