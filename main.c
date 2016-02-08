#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>


void
addwrite(struct addrinfo *addr)
{
	printf(
	"flags: %d\n family: %d\n type: %d\n protocol: %d\n addrlen: %d\n ai_addr: %d\n name: %s\n next: %d\n", 
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

void
main(void)
{
	if(-10)
		printf("%d\n", getpid());

	int socketDescriptor; 
	struct addrinfo hints;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//null other fields	
	hints.ai_protocol = 0;
	hints.ai_addrlen =  0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	
	struct addrinfo *addr_res;	
	getaddrinfo(NULL, "ftp", &hints, &addr_res);
	
	struct addrinfo *s;
	for(s = addr_res; s != NULL; s = s->ai_next)
	{	
		//prints structures for inet and inet6
		addwrite(s); 
	
		socketDescriptor = socket(s->ai_family, s->ai_socktype, s->ai_protocol);
		if(socketDescriptor == -1 )
			err(1, "unable create socket");

		if(bind(socketDescriptor, s->ai_addr, s->ai_addrlen) == -1)
			err(1, "unable bind socket");
		else
			break;

	}
	freeaddrinfo(s);

	int opt = 1;
	if(setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1 )
		err(1, "unable se reusable socket" );
	
	if(listen(socketDescriptor, SOMAXCONN) == -1)
		err(1, "unable start listening socket");

	comunicate(socketDescriptor);

	close(socketDescriptor);

}
