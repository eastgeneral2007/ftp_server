#ifndef dir_cmd_
#define dir_cmd_

#include "control_connection.h"
#include <unistd.h>
#include <sys/stat.h>
#include "helper_fnc.h"

int
exec_mkd_cmd(char *params);

int
exec_rmd_cmd(char *params);

#endif
