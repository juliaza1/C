CC = gcc
CFLAGS = -g -Wall 
CFLAGS := $(CFLAGS) -std=c99
LDFLAGS := $(CFLAGS) -lm

all: serv sock

serv: serv.o tokens.o commands.o proto.o
	$(CC) $(LDFLAGS) -o serv serv.o tokens.o commands.o proto.o

serv.o:  serv.c tokens.h commands.h
	$(CC) $(CFLAGS) -c serv.c 

tokens.o: tokens.c tokens.h
	$(CC) $(CFLAGS) -c tokens.c 

commands.o: commands.c commands.h
	$(CC) $(CFLAGS) -c commands.c

sock: sock.o proto.o
	$(CC) $(LDFLAGS) -o sock sock.o proto.o

sock.o: sock.c 
	$(CC) $(CFLAGS) -c sock.c 


.PHONY: clean
	
clean: 
	$(RM) serv *.o *~
