#$< first target prereqisitas
#$@ target
#$? all prerequisitas delimited by space

CC=gcc
OPTS=-g -Wall -O0

BIN=./bin/
SRC=./src/

all: $(BIN)server.out $(BIN)control_connection.out 
	:	

$(BIN)server.out: $(SRC)server.c
	$(CC)  -o $@ $< $(OPTS)

$(BIN)control_connection.out: $(BIN)control_connection.o $(BIN)login_cmd.o $(BIN)trim.o
	$(CC)  -o $@ $^ $(OPTS)

$(BIN)control_connection.o: $(SRC)control_connection.c $(SRC)control_connection.h
	$(CC)  -c -o $@ $< $(OPTS)
	
$(BIN)login_cmd.o: $(SRC)login_cmd.c $(SRC)login_cmd.h
	$(CC)  -c -o $@ $< $(OPTS)


$(BIN)trim.o: $(SRC)trim.c $(SRC)trim.h
	$(CC)  -c -o $@ $< $(OPTS)

clean: 
	rm -rf $(BIN)*
