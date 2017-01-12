#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

int proto_send(int sock, char *msg, int len, int flags)
{
    int rc = 0;
    // send decimals + len + colon
    int send_len = len + log10l(len) + 3;
    int sent = 0;
    char *buf = calloc(1, send_len);
    assert(buf);
    char *bp = buf;
    rc = sprintf(buf, "%d:%s", len, msg);

    do{
        rc = send(sock, bp, send_len, flags);
        if(rc < 0){
            perror("send failed");
            return -1;
        }
        sent += rc;
        bp += rc;
    }while(send_len < sent);
    #ifdef DEBUG
    printf("proto_sent '%s'\n", buf);
    #endif
    free(buf);

    return sent;
}

int proto_recv(int sock, char *buf, int max_size, int flags)
{
    int rc;
    int got_len = 0;
    int dec_len = 0;
    int expected = 0;
    char *msg;
    static char *next = NULL;
    static int next_size = 0;
    static char *alloc= NULL;

    // case 1; we have more in our receive buffer
    if(next) {
       // a =( condition ? if-stm : else-stm )
       if(next_size > max_size) {
          strncpy(buf, next, max_size);
          next_size -= max_size;
          next += max_size;
          return max_size;
       } else {
          strncpy(buf, next, next_size);
          free(alloc);
          alloc = next = NULL;
          rc = next_size;
          next_size = 0;
          return rc;
       }
    }
       
#define MAX_LEN_DIGITS 7
    next = alloc = malloc(max_size + MAX_LEN_DIGITS);
    // case 2; we have nothing in receive buffer
    // might have to recv multiple times
    do {
        int r_len;
        char *bp = next + got_len;
        r_len = recv(sock, bp, max_size + MAX_LEN_DIGITS - got_len, flags);
        #ifdef DEBUG
        printf("got '%s'\n", bp);
        #endif
        if(!got_len) {
            msg = strchr(next, ':');
            if(!msg){
                perror("protocol error in recv");
                return -1; // error out
            }
            *msg = '\0'; // null terminate length
            msg++; // make msg point to the message

            rc = sscanf(next, "%d", &expected);
            assert(rc == 1);
            dec_len = log10l(expected) + 1;
            expected += dec_len + 2;
        }
        got_len += r_len;
    }while(got_len < expected);

    next_size = got_len;
    
    //case 3; received too much. need to store across calls
    //case 4; buffer is smaller than expect_len
    // so we can't fit the whole receive in it!
    if(max_size < expected) {
       strncpy(buf, msg, max_size);
       next += max_size;
       next_size -= max_size;
       rc = max_size;
    }else{
       strncpy(buf, msg, expected - dec_len);
       next += expected;
       next_size -= expected;
       rc = expected;
    }
    if(got_len <= expected) {
       free(alloc);
       alloc = next = NULL;
    }else{
       msg = strchr(next, ':');
       if(msg) {
          *msg = '\0';
          msg++;
          sscanf(next, "%d", &expected); // not needed
          next = msg;
       }else{
          fprintf(stderr, "continuing with %d next\n", next_size);
       }

      // else message is empty or just a continuation

    }
       
    return rc;

}


