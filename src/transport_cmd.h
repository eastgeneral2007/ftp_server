#ifndef transport_control
#define transport_control

#include "control_connection.h"
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "transport_param_cmd.h"


int
exec_stor_cmd(char *params);

int
exec_retr_cmd(char *params);

#endif
