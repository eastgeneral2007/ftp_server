#ifndef control_connection_h
#define control_connection_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <err.h>

#include "helper_fnc.h"

extern char *user;
extern char *loc_adr;
extern char *cur_file;
extern int ipv4;
extern struct trans_con{
	int trans_in, trans_out;
	unsigned short port;
	pid_t pid;
} *trans_con;


void 
send_proto(int code, char *message);

int
process_cmd();

int
process_get_verify_cmd(char* expec_cmd);

#endif
