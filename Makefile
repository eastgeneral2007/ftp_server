BIN=./bin/
SRC=./src/

all: $(BIN)server.out $(BIN)control_connection.out 
	:	

$(BIN)server.out: $(SRC)server.c
	gcc -g -Wall -O0 $< -o $@

$(BIN)control_connection.out: $(SRC)control_connection.c
	gcc -g -Wall -O0 $< -o $@



clean: 
	rm -rf $(BIN)*
