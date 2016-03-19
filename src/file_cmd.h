#ifndef file_cmd_
#define file_cmd_

#include "control_connection.h"
#include "helper_fnc.h"
#include <sys/stat.h>

int
exec_rnfr_cmd(char *params);

int 
exec_rnto_cmd(char *params);

int 
exec_dele_cmd(char *params);
#endif
