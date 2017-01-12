#include <stdio.h>
#include <string.h>    
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <arpa/inet.h> //inet_addr
#include <stdlib.h>
#include <netdb.h>
#include "proto.h"
#define INPUT_SIZE 255
#define DIR_MAX 999999



/*Structure of IPv4 AF_INET sockets (in <arpa/inet.h>):
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};
 
struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};
 
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};*/

//ifi headers lack this

struct addrinfo {
             int ai_flags;           /* input flags */
             int ai_family;          /* protocol family for socket */
             int ai_socktype;        /* socket type */
             int ai_protocol;        /* protocol for socket */
             socklen_t ai_addrlen;   /* length of socket-address */
             struct sockaddr *ai_addr; /* socket-address for socket */
             char *ai_canonname;     /* canonical name for service location */
             struct addrinfo *ai_next; /* pointer to next in list */
     };
int getaddrinfo(const char *hostname, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
const char *gai_strerror(int errcode);



int main(int argc , char *argv[]) {
    int socket_desc;
    int connected = 1;
    char *port;
    char *host;
    struct addrinfo *ai;

    if(argc > 1)
        host = argv[1];
    else
        host = "127.0.0.1";
    if(argc > 2)
        port = argv[2];
    else
        port = "8888";

    char server_reply[DIR_MAX];
    char input[INPUT_SIZE];

    printf("Connecting to: %s, %s\n", host, port );
    int rc = getaddrinfo(host, port, NULL, &ai);
    if (rc) {
        fprintf(stderr, "Failed to get addrinfo: %s\n", gai_strerror(rc));
        exit(1);
    }
    //Create socket
    socket_desc = socket(ai->ai_family , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
    }

    int activate = 1;
    setsockopt(socket_desc, IPPROTO_TCP, TCP_NODELAY, &activate, sizeof(int));

    //Connect to remote server
    if (connect(socket_desc , ai->ai_addr , ai->ai_addrlen) < 0) {
        perror("Connect error");
        return 1;
    }
     
    printf("Connected\n");
    while (connected) {
        memset(input, 0, INPUT_SIZE);
        memset(server_reply, 0, DIR_MAX);

        if (proto_recv(socket_desc, server_reply , DIR_MAX , 0) < 0) {
            perror("recv failed");
        }
        printf("%s\n", server_reply );
        

	    // User input
	    fgets(input, INPUT_SIZE, stdin);

        // slette \n fra input
        int len = strlen(input);
        input[len-1] ='\0';
        #ifdef DEBUG
            printf("SEND INPUT: '%s'\n", input );
        #endif
	    //Send some data
	    if (proto_send(socket_desc , input , len, 0) < 0) {
	        perror("Sending failed");
	        return 1;
	    }
	    // printf("Request sent\n");
        memset(server_reply, 0, DIR_MAX);
	     
	    //Receive a reply from the server
	    if (proto_recv(socket_desc, server_reply , DIR_MAX , 0) < 0) {
	        perror("recv failed");
	    }

        //ANSWERS 
        printf("\n");
        printf("%s\n", server_reply);

        //q
        if (strcmp(server_reply, "quit") == 0) {
            printf("Quitting...\n");
            
            connected = 0;
        }
	}
    return 0;
}
