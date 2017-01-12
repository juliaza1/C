#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h> //inet_addr
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/stat.h> //for lstat()
#include <math.h>
#include "tokens.h"
#include "commands.h"
#include "proto.h"

#define DIR_MAX 999999
#define MAX_FILE_NAME_CHARS 255

int conn = 0;

char *locked_path = NULL;

//FIXME HANDLE EMPTY INPUT

int handle_connection(int client_sock) {

	int run = 1;
	int count;
	char ls[DIR_MAX+1];
	struct dirent **files;
	char catbuf[DIR_MAX];


	assert(client_sock > 0); 
	/* tests the expression & if false,calling process is terminated. A diagnostic message is written to stderr and the abort(3) function is called, effectively terminating the program. */
	printf("Connection accepted\n");

	//Receive request
	char request[MAX_REQUEST];
	char reQuest;
	char *answer = NULL;

	do {
		memset(request, 0, sizeof(MAX_REQUEST));

		if (proto_send(client_sock, MESSAGE_MAIN, strlen(MESSAGE_MAIN), 0) < 0 ) {
			perror("Send failed\n");
		}
		if (proto_recv(client_sock, request, sizeof(request), 0) < 0 ) {
			perror("Receive failed\n");
		}
		#ifdef DEBUG
		printf("Request received\n");
		printf("Got request: '%s'\n", request );
		#endif

		//TOKENIZE input
		char **param;
		if (strlen(request) != 0) {
			param = tok(request); //used a self-written tokenizer instead of strtok()
			//checks if input from server is legal
			if (strcmp(*param, "1") == 0 || strcmp(*param, "2") == 0 || strcmp(*param, "3") == 0 || strcmp(*param, "4") == 0 || strcmp(*param, "5") == 0 || strcmp(*param, "?") == 0 || strcmp(*param, "q") == 0) {
				reQuest = *param[0];
			} else {
				answer = "Wrong input\n";
			}
			#ifdef DEBUG
			printf("Server got reQuest (after tokenize): '%c'\n", reQuest ); 
			#endif
		} else {
			reQuest = '&';
		}


		switch(reQuest) {
		    case '1'  :
		    	count = my_ls(&files, ls);
		    	answer = ls;
		       	break; 
		        
		    case '2'  :
		       	answer = my_pwd();
		       	break; 

		    case '3'  :
		    	//FIXME navigere mellom alle kataloger innenfor den katalogen som tjeneren startes i, men aldri utenfor denne katalogen
		    	answer = my_cd(client_sock);
	         	break; 

	        case '4'  :
	        	/*
				FIXME: BUG IF INPUT FROM CLIENT IS NOT AN INTEGER
				FIXME: BUG IF INPUT FROM CLIENT > COUNT
	        	*/
				answer = my_lstat(client_sock);
		        break; 
	     
	        case '5'  :
	        	answer = my_cat(client_sock, catbuf, DIR_MAX);
		        break; 

	        case '?'  :
	        	//sending OK
		        if (proto_send(client_sock, OK, strlen(OK), 0) < 0 ) {
		        	perror("Send failed");
		        }
		        //sending overview of main menu
		        if (proto_send(client_sock, MESSAGE_HELP, strlen(MESSAGE_HELP), 0) < 0) {
		        	perror("Send failed");
		        }
		        //receiving any input
         		if (proto_recv(client_sock, request, sizeof(request), 0) < 0 ) {
	         		perror("Receive failed\n");
	         	} 
	           	answer = OK; // FIXME recv 2 should be OK and then main menu again
	           	break; 

	        case 'q'  :
				answer = "quit";
				printf("I am going to quit...\n");
	           	run = 0;
	           	
	           	break; 

		    default : 
	           printf("Command not found\n");
	           answer = "NO";
		}

		//Send Answer 
		if (proto_send(client_sock, answer, strlen(answer),0) < 0) {
			perror("Sending failed");
		}
		printf("THIS ANSWER IS SENDT TO CLIENT '%s'\n\n", answer);
      answer = NULL;
	} while(run);

	close(client_sock);
    fprintf(stderr, "client %d is exiting\n", client_sock);
	return 0;
}

int main (int argc , char *argv[]) {

	int server_quit = 0;
	int c;
	int socket_desc, client_sock;
	struct sockaddr_in server, client;
	signal(SIGCHLD, SIG_IGN); // no zombies!
	
	locked_path = getcwd(NULL, 0);

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Couldn't create socket\n");
	}

	//(FIXME) Taking port number as cmd line argument
	//FIXME input is string -> Port = 0; input is x.z -> Port = x
	//FIXME get & sethostname()
	int port;
    if(argc > 1)
        port = atoi(argv[1]);
    else
        port = 8888;

	printf("PORTNUMBER: %d\n", port );

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	
	int flag = 1;

	// int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)); //avoid adress in use

	//Bind
	if (bind(socket_desc,(struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Bind failed");
		exit(1);
	}
	printf("Bind done\n");

	//Listen
	listen(socket_desc , 3);

	do {
		pid_t pid;
		//Accept an incoming connection
		printf("Waiting for incomming connection...\n");
		c = sizeof(struct sockaddr_in);
		client_sock = accept (socket_desc, (struct  sockaddr *)&client, (socklen_t*)&c);
		if(client_sock<0) {
			perror("Accept failed\n");
			//sending error message to client before closing
			if (proto_send(client_sock, "Accept failed. Close.\n", 20, 0) < 0 ) {
				perror("Send failed");
			} 
			close(client_sock);
			continue;
		}

		

		
		pid = fork();
		if (pid == 0) { //child
			handle_connection(client_sock); //main menu -> loop

		
		} else if (pid > 0) { //parent
			conn++; //teller for children
			printf("ANTALL CONNECTIONS: %d\n", conn );
		} else {
			perror("fork failed");
			//sending error message to client before closing
			if (proto_send(client_sock, "Fork operation failed. Close.\n", 50, 0) < 0 ) {
				perror("Send failed");
			} 			
			close(client_sock);
		}
		//FIXME DECREMENT CONNECTION COUNTER IN SIGNALHANDLER
	} while (!server_quit);
	return 0;
}



