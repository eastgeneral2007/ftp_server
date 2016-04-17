#ifndef transport_param_control
#define transport_param_control

#include <sys/wait.h>
#include "control_connection.h"

int
exec_pasv_cmd(char *params);

int
exec_epsv_cmd(char *params);

//frees trans structure and its content at sets variable to NULL
void	
free_trans(void);

//sends signal to transport connection process to read from its input
int
start_send(void);

//sends signal to transpourt connection process to write tu its ouput
int
start_recieve(void);
#endif
