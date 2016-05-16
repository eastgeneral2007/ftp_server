#include <signal.h>
#include <err.h>
#include <errno.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>

#include "helper_fnc.h"

int par_desc, transport = 0;
char *cur_file = NULL, *adr = NULL;
static int af = AF_UNSPEC;

char*
get_loc_adr6(int desc)
{
	struct sockaddr_in6 cur_adr;
	socklen_t len =  sizeof(struct sockaddr_in6); 
	
	if( -1 == getsockname(desc, (struct sockaddr*)&cur_adr, &len))
		err(1, "cannott read socked addr struct");

	char *res = malloc(INET6_ADDRSTRLEN);
	if(!inet_ntop(AF_INET6, &(&cur_adr)->sin6_addr, res, INET6_ADDRSTRLEN))
		err(1, "inet_ntop error");
	return res;
}

char*
get_loc_adr4(int desc)
{
	struct sockaddr_in cur_adr;
	socklen_t len =  sizeof(struct sockaddr_in); 
	
	if( -1 == getsockname(desc, (struct sockaddr*)&cur_adr, &len))
		err(1, "cannott read socked addr struct");

	char *res = malloc(INET_ADDRSTRLEN);
	if(!inet_ntop(AF_INET, &(&cur_adr)->sin_addr, res, INET_ADDRSTRLEN))
		err(1, "inet_ntop error");
	return res;
}

//gets ipaddress of interface client is connected to
char*
get_loc_adr(int desc)
{
	switch(af)
	{
		case AF_INET:
			return get_loc_adr4(desc);
		case AF_INET6:
			return get_loc_adr6(desc);
		default:

			errx(1, "inalid af specification");
			return NULL;
			break;

	}
}



//returns port socket is listening on 
char *
get_port(int socketDescriptor)
{
	
	int res_len = transport ? NI_MAXSERV : INET6_ADDRSTRLEN;
	struct sockaddr_in6 cur_adr;
	socklen_t len =  sizeof(struct sockaddr_in6); 
	
	if( -1 == getsockname(socketDescriptor , (struct sockaddr*)&cur_adr, &len))
		err(1, "cannott read socked addr struct- port ");

	char *res = malloc(res_len);
	int er = getnameinfo((struct sockaddr*)&cur_adr, len, NULL, 0, res, res_len, NI_NUMERICHOST | NI_NUMERICSERV);
	if(er)	
		printf("%s\n", gai_strerror(er));
	
	//dprintf(2, "transport port: %s\n", res);
	return res; 
}

#define ER(x) if(-1 == x) err(1, "transfer data error");

//sends data from inDesc to outDesc
void
send_data(int inDesc, int outDesc)
{
	char* buff[1024];
	int read_len;
	while((read_len = read(inDesc, buff, 1024)) > 0 )//TODO error
	{
		ER(read_len);
		ER(write(outDesc, buff, read_len));
	}
	ER(read_len);
}

//read/write data from pipes to socket. If SIGUSR1 is recieved comunication is set from input to socket.
//Otherwise if SIGUSR2 is recieved communication is from socket to ouput.
void
transfer_data(int socketDescriptor)
{
	int res_sig;
	sigset_t set;
	int chanDesc;
	//dprintf(2, "before signal setup\n");

	ER(sigemptyset(&set));
	ER(sigaddset(&set, SIGUSR1));
	ER(sigaddset(&set, SIGUSR2));
	ER(sigprocmask(SIG_SETMASK, &set, NULL));
	
	ER((chanDesc = accept(socketDescriptor, NULL, 0))); 
	//dprintf(2, "socket accepted\n");

	if(sigwait(&set, &res_sig) != 0)
		exit(1);

	//dprintf(2, "signal accepted\n");

	if(res_sig == SIGUSR1)
		send_data(0, chanDesc);
	else if(res_sig == SIGUSR2)
		send_data(chanDesc, 1);
	else
		exit(1);
	
	ER(close(socketDescriptor));
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
			err(1, "unable accept connection");

		switch(fork())
		{
			case -1:
				err(1, "cannot fork"); 
				break;
			case 0: 
				close(0);
				close(1);	
				char *adr = get_loc_adr(chanDesc);
				dprintf(2, adr);
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
	int er = 0;
	unsigned short int_port = (unsigned short)parse_int(port, &er);
	
	//dprintf(2, "%s : %d", port, p);
	write(par_desc, &int_port, sizeof(unsigned short));
        close(par_desc);
        free(port);
}

#if 0

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

#endif

int
initSock(struct addrinfo *addr_res)
{
	struct	addrinfo *walk_ai = addr_res;
	int	set = 1;
	int	s;
	int	rv;

	do {
		s = socket(walk_ai->ai_family, walk_ai->ai_socktype,
		    walk_ai->ai_protocol);
		if (s < 0) {
			continue;
		}

		rv = setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &set, sizeof (set));
		if (rv == -1) {
			perror("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
			exit(1);
		}

		rv = bind(s, (struct sockaddr *)walk_ai->ai_addr,
		    walk_ai->ai_addrlen);
		if (rv == 0) {
			af = walk_ai->ai_family;
			break;
		}

		close(s);
		s = -1;
	} while ((walk_ai = walk_ai->ai_next) != NULL);

	return (s);
}

 //run the server either in transport on server mode 
 void
 startServer(char * port)
 {
/*
 * co se asi stane, kdyz uzivatel spusti program takhle:
 *	./server 2121
 *
 * getaddrinfo() je pouzit spravne, ale pro splneni zadani
 * je potreba zavolat dvakrat:
 *	jednou pro IPv4 (AF_INET) INADDR_ANY 
 *	podruhe pro IPv6 (AF_INET6) INADDR_ANY
 *
 * A vzhledem k tomu, ze server ma bezet na IPv4 a IPv6 je potreba dvou soketu.
 * Ale to uz je docela brutalni zasah do designu celeho projektu. Myslim, ze by se
 * to dalo zachranit pridanim command line optionu:
 *	-4	pusti server tak, ze bude poslouchat na vsech IPv4 adresach
 *		(INADDR_ANY)
 *
 *	-6	pusti server tak, ze bude poslouchat na vsech IPv6 adresach
 *		(INADDR_ANY)
 *
 * pokud nebude specifikovana optiona -4/-6 server pouzije adresu, kterou
 * getaddrinfo() vrati jako prvni. getaddrinfo() info typicky projde IPv4
 * adresy, pak IPv6 (pokud hints nespecifikuji AF).
 */
 	struct addrinfo hints;
	struct addrinfo *addr_res;	

 	memset(&hints, 0, sizeof(struct addrinfo));
 	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
/*
 * af by mela byt globalni promenna nastavena v main podle pritomnosti -4/-6
 */
	hints.ai_family = af;
 	
 	if(getaddrinfo(transport ? adr : NULL, transport ? NULL : (port ? port : "ftp"), &hints, &addr_res))
 		err(1, "cant get addrinfo" );
	int socketDescriptor = initSock(addr_res);
 	freeaddrinfo(addr_res);

	if (socketDescriptor < 0) {
		err(1, "could not create listen socket");
	}
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

static void
usage(void)
{
	fprintf(stderr, "usage: server.out [-46p]\n"
	    "\t-4\tforces to use IPv4 only\n"
	    "\t-6\tforces to use IPv6 only\n"
	    "\t-p\tselects port number to listen to\n"
	    "If neither option is specified program uses defaults:\n"
	    "\tAF_UNSPEC port 21");
	exit(1);	
}

int
main(int argc, char* argv[])
{
	int	opt;
	char	*port = "ftp";

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
		while ((opt = getopt(argc, argv, "46p:")) != -1) {
			switch (opt) {
			case '4':
				af = AF_INET;
				break;
			case '6':
				af = AF_INET6;
				break;
			case 'p':
				port = optarg;
				break;
			default:
				usage();
			}
 		}
		startServer(port);
 	}

	return (0);
}



