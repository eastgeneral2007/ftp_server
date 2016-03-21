#ifndef commands_h
#define commands_h

int
exec_user_cmd(char *params);

int
exec_quit_cmd(char *params);

int
exec_list_cmd(char *params);

int
exec_mlsd_cmd(char *params);

int
exec_pasv_cmd(char *params);

int
exec_epsv_cmd(char *params);

int
exec_cwd_cmd(char *params);

int
exec_pwd_cmd(char *params);

int
exec_cdup_cmd(char *params);

int
exec_mkd_cmd(char *params);

int
exec_rmd_cmd(char *params);

int
exec_rnfr_cmd(char *params);

int
exec_rnto_cmd(char *params);

int
exec_dele_cmd(char *params);

int
exec_stor_cmd(char *params);

int
exec_retr_cmd(char *params);

#endif
