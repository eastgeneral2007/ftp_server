#ifndef transport_control
#define transport_control

#include <sys/wait.h>
#include "control_connection.h"

int
exec_pasv_cmd(char *params);

void	
free_trans(void);

#endif
