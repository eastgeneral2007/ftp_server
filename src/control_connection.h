#ifndef control_connection_h
#define control_connection_h

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <err.h>

#include "helper_fnc.h"

//ip address client is connected to 
extern char *loc_adr;
//basename of executable
extern char *cur_file;
//bool if connection is ipv4
extern int ipv4;

struct trans_con{
	int trans_in, trans_out;
	unsigned short port;
	pid_t pid;
};

//holds user login session, if no user is logged in it is NULL
extern struct session{
	char *user;
	char *cur_path;
	char *root_path;
	struct trans_con *trans_con;
	char *next_seq_cmd;
} *session;


//sends string response in appropriate format back to client
void 
send_proto(int code, char *message);


//allocate session structure and inicialize it
void
create_session(char *user);

//dispose sesion data and unallocate structure and sets to NULL
void
free_session(void);

//returns whether params is empty and send response to client a returns bool depenging on is_empty_valid parameter
int
params_empty(char *params, int is_empty_valid);

//send response to client in appropriate format with path response
void
send_path_proto(char * rel_path);
#endif
