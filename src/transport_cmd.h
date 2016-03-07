#ifndef transport_control
#define transport_control

#include <sys/wait.h>
#include "control_connection.h"

extern struct trans_con *trans_con;

int
exec_pasv_cmd(char *params);

void	
free_trans(void);

#endif
