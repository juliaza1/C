#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "proto.h"
#include <sys/stat.h> //for lstat()
#define FALSE 0
#define TRUE !FALSE
#define BUFFSIZ 32768
#define MAX_FILE_NAME_CHARS 255
#define DIR_MAX 999999

 
extern int alphasort(); //Inbuilt sorting function
extern char *locked_path;
int scandir(const char *dirname, struct dirent ***namelist, int (*select)(const struct dirent *), int (*compar)(const struct dirent **, const struct dirent **)); //does not compile @ifi without this line
char *realpath(const char *path, char *resolved_path);

char pathname[MAXPATHLEN];
char ls[DIR_MAX+1];
struct stat buffer;
FILE *fp = NULL;
struct dirent **files;

int lstat(const char *restrict path, struct stat *restrict buf); //does not compile @ifi without this line
char *typeOfFile(mode_t);

char * typeOfFile(mode_t mode) { // domain, link, regular file or special file
    #define S_IFMT      0170000  //type of file 
    #define S_IFIFO     0010000  //named pipe (fifo) 
    #define S_IFCHR     0020000  //character special 
    #define S_IFDIR     0040000  //directory 
    #define S_IFBLK     0060000  //block special 
    #define S_IFREG     0100000  //regular 
    #define S_IFLNK     0120000  //symbolic link
    #define S_IFSOCK    0140000  //socket 
    #define S_IFWHT     0160000  //whiteout 
    ////does not compile @ifi without #define lines

    switch (mode & S_IFMT) {
        case S_IFREG:
            return("regular file");
        case S_IFDIR:
            return("directory");
        case S_IFCHR:
            return("special file");
        case S_IFBLK:
            return("special file");
        case S_IFLNK:
            return("symbolic link");
        case S_IFIFO:
            return("special file");
        case S_IFSOCK:
            return("special file");
        case S_IFWHT:
            return("special file");    
    }

    return("Illegal input\n");
}

//hopper over . og ..
int file_select(const struct direct *entry) {
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        return (FALSE);
    else
        return (TRUE);
}

// [1] list content of current directory (ls)
int my_ls(struct dirent ***files, char *ls) {
    int count;	
	 
    if(!getcwd(pathname, sizeof(pathname))) {
		perror("Error getting pathname\n");
	}
	//reads the directory dirname and builds an array of pointers to directory entries using malloc 
    //returns number of array (files) entries
    count = scandir(pathname, files, &file_select, alphasort);
    
    // If no files found, make a non-selectable menu item 
    if(count <= 0) {
      	perror("No files in this directory\n");
        return -1;
  	}
    char tmp[20];
    //lagrer 1 string av array-indeksene
    for (int i = 0; i < count; i++) {
        sprintf(tmp, "[%d]\t", i+1);
        strncat(ls, tmp, DIR_MAX);
        strncat(ls, (*files)[i]->d_name, DIR_MAX);   
        strncat(ls, "\n", DIR_MAX);
    }
     
    return count;	
}

// [2] print name of current directory (pwd)
char *my_pwd () {
	if(!getcwd(pathname, sizeof(pathname))) {
		perror("Error getting pathname\n");
	}

	return pathname;
}

// [3] change current directory (cd)

int changedir(const char *path) {
    if (chdir(path) != 0) {
        perror("error change directory\n");
    }
    printf("%s\n", my_pwd());
    return 0;
}

char *my_cd(int client_sock) {

    char request[MAX_REQUEST];

    //sending OK
    if (proto_send(client_sock, OK, strlen(OK), 0) < 0 ) {
        perror("Send failed\n");
    }

    int quit = 1;
    while (quit) {
        if (proto_send(client_sock, MESSAGE_CD, strlen(MESSAGE_CD), 0) < 0 ) {
            perror("Send failed\n");
        }
        if (proto_recv(client_sock, request, sizeof(request), 0) < 0 ) {
            perror("Receive failed\n");
        } 

        if (strcmp(request, "?") == 0 ) {
            if (proto_send(client_sock, OK, strlen(OK), 0) < 0 ) {
                perror("Send failed\n");
            }
            if (proto_send(client_sock, MESSAGE_CD_HELP, sizeof(MESSAGE_CD_HELP) ,0) < 0 ) {
                perror("Send failed\n");
            } 
            if (proto_recv(client_sock, request, sizeof(request), 0) < 0 ) {
                perror("Receive failed\n");
            } 
            break;
        } else if (strcmp(request, "q") == 0) {
            quit = 0;
        } else {
            break;
        }
    } 
    if (quit == 0) {
        return OK; //not changing directory
    } else {
        char *path = realpath(request, NULL);
        int locked_len = strlen(locked_path);
        if (strncmp(path, locked_path, locked_len) == 0) {
            if (changedir(request) != 0) { //changing directory 
                perror("Failed to change directory\n");
            }
            return my_pwd(); //answer = new current directory
        } else {
            return ERROR_CD;
        }
    }
}

//[4] lstat
char *my_lstat(int client_sock) {
    char request[MAX_REQUEST];
   
    char ls[DIR_MAX+1];

    //sending OK
    if (proto_send(client_sock, OK, strlen(OK), 0) < 0 ) {
        perror("Send failed\n");
    }

    //creating file list using my_ls()
    strncat(ls, "Get file information for:\n", DIR_MAX);
    int count = my_ls(&files, ls);
    //add quit to ls
    strncat(ls, "[q]\tLeave this menu\n", DIR_MAX);
    strncat(ls, MESSAGE_FILE, DIR_MAX);
    strncat(ls, "\n", DIR_MAX);

    int index = 0;

    int quit = 1;
    do {
        //sending file list
        if (proto_send(client_sock, ls, strlen(ls), 0) < 0 ) {
            perror("Send failed\n");
        }
        //receiving file number
        if (proto_recv(client_sock, request, sizeof(request), 0) < 0 ) {
            perror("Receive failed\n");
        } 

        //string to int
        index = (atoi(request) -1);

        if (index > count) {
            perror("array index out of reach");
            if (proto_send(client_sock, "Array index out of reach\n", 26, 0) < 0 ) {
                perror("Send failed\n");
            }
            quit = 0;
        } else if (strcmp(request, "q") == 0) {
            quit = 0;
        } else {
            break;
        }
    } while(quit);

    if (quit == 0) {
        return OK; //jumped out of while because of an error
    } else {
        if (lstat(files[index]->d_name, &buffer) < 0) {
            perror("lstat");
        }
        return typeOfFile(buffer.st_mode);
    }
}

// 5 cat
char *my_cat(int client_sock, char *catbuf, int catsize) {

    char ch; 
    int count;
    char request[MAX_REQUEST];


    if (proto_send(client_sock, OK, strlen(OK), 0) < 0 ) {
        perror("Send failed\n");
    }

    //creating file list using my_ls()
    strncat(ls, "Press key to choose file:\n", DIR_MAX);
    count = my_ls(&files, ls);
    //add quit to ls
    strncat(ls, "[q]\tLeave this menu\n", DIR_MAX);
    strncat(ls, MESSAGE_FILE, DIR_MAX);
    strncat(ls, "\n", DIR_MAX);

    int index = 0;

    int quit = 1;
    while (quit) {

        //sending file list
        if (proto_send(client_sock, ls, strlen(ls), 0) < 0 ) {
            perror("Send failed\n");
        }
        //receiving file number
        if (proto_recv(client_sock, request, sizeof(request), 0) < 0 ) {
            perror("Receive failed\n");
        } 

        //string to int
        index = (atoi(request) -1);

        if (index > count) {
            perror("array index out of reach");
            if (proto_send(client_sock, "Array index out of reach\n", 26, 0) < 0 ) {
                perror("Send failed\n");
            }
            break;
        } else if (strcmp(request, "q") == 0) {
            quit = 0;
        } else {
            break;
        }
    }

    if (quit == 0) {
        return OK; 
    } else {
        fp = fopen(files[index]->d_name, "r");
        if(fp != NULL) {
            //read file and feed contents to cat
            while((ch = fgetc(fp)) != EOF) {

                if (ch < 32 && (ch < 7 || ch > 13)) {  //all characters below 32 are non-printable. replace with '.'
                    ch = '.';
                }
                strncat(catbuf, &ch, catsize);
            }
            fclose(fp);
            return catbuf;
        }
    }
    return ERROR;
}













