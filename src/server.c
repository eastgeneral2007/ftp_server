#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>


void
addwrite(struct addrinfo *addr)
{
	printf(
	"flags: %d\n family: %d\n type: %d\n protocol: %d\n addrlen: %d\n ai_addr: %p\n name: %s\n next: %p\n", 
	addr->ai_flags ,
	addr->ai_family ,
	addr->ai_socktype ,
	addr->ai_protocol ,
	addr->ai_addrlen ,
	addr->ai_addr ,
	addr->ai_canonname ,
	addr->ai_next );
}

void
accept_connections(int socketDescriptor)
{
	for(;;)
	{
		int chanDesc;
		if( (chanDesc = accept(socketDescriptor, NULL, 0)) == -1 )
			err(1, "unable accept connection");
	
		char abs_path[PATH_MAX], *file = "./control_connection.out";
		realpath(file, abs_path);
		pid_t pid = fork();	
		switch(pid)
		{
			case -1:
				err(1, "cannot fork");
				break;
			case 0: 
				close(0);
				close(1);	
				dup(chanDesc);
				dup(chanDesc); close(chanDesc);
				execl(abs_path, file, (char*)NULL);
				break;
			default:
				close(chanDesc);				
				break;
		}
	}
}

void
comunicate(int socketDescriptor)
{
	int chanDesc;
	if( (chanDesc = accept(socketDescriptor, NULL, 0)) == -1 )
		err(1, "unable accept connection");
	
	FILE *channel = fdopen(chanDesc, "r");
	size_t buf_size = 0;
	char *line = NULL;	
	ssize_t len = 0;
	
	do
	{
		errno = 0;
		if( (len = getline(&line, &buf_size, channel)) == -1 && errno )
			err(1, "problem reading line " );

		printf("%s", line );
		free(line);
		line = NULL;
		buf_size = 0;
	}while(len != -1 );
	fclose(channel);
}	

int
init_dualstack(struct addrinfo *addr_res)
{
	int socketDescriptor = -1;
	struct protoent *tcp_ent = getprotobyname("tcp");
	if(!tcp_ent)
		err(1, "unable to find tcp flag" );
	
	struct addrinfo *s;
	for(s = addr_res; s != NULL; s = s->ai_next)
	{	
		addwrite(s); 
		if( s->ai_family == AF_INET6 && s->ai_protocol == tcp_ent->p_proto)
		{
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

void
startServer(void)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	
	struct addrinfo *addr_res;	
	if(getaddrinfo(NULL, "ftp", &hints, &addr_res))
		err(1, "cant get addrinfo" );
	int socketDescriptor = init_dualstack(addr_res);
	freeaddrinfo(addr_res);


	if(listen(socketDescriptor , SOMAXCONN) == -1)
		err(1, "unable start listening socket");

	accept_connections(socketDescriptor);
	close(socketDescriptor);
}

int
main(int argc, char* argv[])
{
	startServer();
	return (0);
}
