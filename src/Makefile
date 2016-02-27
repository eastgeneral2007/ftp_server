X=../bin/
all: $Xserver.out $Xcontrol_connection.out 
	:	

$Xserver.out: server.c
	gcc -g -Wall -O0 $< -o $@

$Xcontrol_connection.out: control_connection.c
	gcc -g -Wall -O0 $< -o $@


clean: 
	rm -rf $X*
