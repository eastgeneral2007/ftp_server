# ftp_server
##Specification:
#####done - anonymous login only
#####done - ipv4/ipv6 dual stack
#####done - login/logout by anonymous - USER, PASS, QUIT
#####done - pasive mode communication only - PASV, EPSV
#####done - deleteing server files - DELE
#####done - creating/deleting directories - MKD, RMD
#####done - path or change of current working directory - CWD, CDUP, PWD
#####done - list diectory (+ machine format) - LIST, MLSD
#####done - renaming of files/directories - RNFR, RNTO
#####done - store files on the server - STOR
#####done - download file from server - RETR
##Architecture & Usage
######Whole ftp server consist of two executables server.out control_connection.out. The server executable can operate in two modes depending on filename name, transport.out is just hard link to server.out. 
#####Server
######It take care of binding socket, listening on appropriate port and accepting clients connections. For each incoming conneciton server forks and execs control_connection.out with stdin set as socket out an stdout as socket in. Executable server.out takes one optional parameter representing port on which server is going to listen. If no parameter is specifed server is listening on default ftp port 21. It implements ipv4/ipv6 dualstack with ipv6 mapped ipv4 addresses. It is listening on every interface, wildcard socket is used.
###### usage: server.out [`<listening_port>`]
#####Control connection
######Control connection then handles communication with one client. After all communication with client is done process ends thus socket descriptors are closed. It support all commands specifed above in specification. 
###### usage: ./control_connection.out `<ip_address_of_interface_client_is_connected_to>`
#####Transport connection
######If client requires data transfer control connection process forks and execs transport.out with three pipes between them. Executable transport.out requires ip address and socket descriptor of pipe intended for communication with parent(control connection). Through this pipe transport sends two bytes representing port on which transport is listening on. 
######usage: transport.out `<interface_adr> <parent_comunication_descriptor>`
##Programmer documentation
#####Control connection
######It is splited to several modules. Control_connection.c has communication loop which parses clients command and execute right command handler. If the command is unrecognized common reply is sent to client. Because this is only small implementation of ftp server the commands handlers calls are just staticaly hardcoded. The entire list of them is in commands.h. Commands handler function names has following pattern exec_`<command_name>`_cmd(char *params) . Commands implementation is splitted to modules according to kind of commands job. Every command module can include control_connection.h file which provides fundamental state structures and helper function for communication. The structures are following:
######This variable holds ip address of interface client is connected to. It is in ipv6 ipv4 mapped format. It is mainly required for transport connection.
```c
extern char *loc_adr;
```
######Following variable is boolean  and holds whether connection runs ipv4 or ipv6 connection.
```c
extern int ipv4;
```
######This structure holds transport connection data. In/out descriptors for data transmissions, port on which is server transport connection listening on and process id of that process.
```c
struct trans_con{
	int trans_in, trans_out;
	unsigned short port;
	pid_t pid;
};
```
######Following structure holds user session. It can be created by create_session and unallocated by free_session functions. If any user is logged on it is filled with valid data otherwise session is NULL. User is username, cur_path is current server working directory and it is relative to root_path. Root_path is absolute path of users storage in respect to server filesystem. It shouldnt be possible to get higher than root_path. Trans_con variable points to struct trans_con which is representing valid transport connection, otherwise it is NULL. If there is any command requesting another command call following him. Such a command name can be saved to next_seq_cmd variable and conntrol connection then ensure right error message is sent to client when the bad sequence of command is provided by client. 
```c
extern struct session{
	char *user;
	char *cur_path;
	char *root_path;
	struct trans_con *trans_con;
	char *next_seq_cmd;
} *session;
```
#####Other
######Besides commands modules which simply provide respones to command there is helper_fnc.c module. The module provides some helper functions which are not avalible in standart libraries such as: triming by specifed char safe atoi conversion function and mainly function for parsing and simplifing servers paths. Detailed documentation for each function can be found in header files.
#####Tests
###### There are tests in /tests directory, which are intended mainly for memchcking. They are testing different cases of commands (sucess, failure, etc.). Please, before using these tests read readme in /test directory, because some tests need prerequisite. For other testing i've simply used client such as winscp to test all commands specifed in specification. 
