
#ifndef control_connection_h
#define control_connection_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void 
send_proto(int code, char *message);

int
process_cmd();

int
process_get_verify_cmd(char* expec_cmd);

#endif
