#ifndef transport_param_control
#define transport_param_control

#include <sys/wait.h>
#include "control_connection.h"

int
exec_pasv_cmd(char *params);

int
exec_epsv_cmd(char *params);

void	
free_trans(void);

#endif
