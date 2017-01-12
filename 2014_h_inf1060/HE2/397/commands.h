#ifndef _COMMANDS_H_
#define _COMMANDS_H_

	int my_ls(struct dirent ***files, char *ls);
	char* my_pwd();
	int changedir(const char *path);
	char* my_cd(int client_sock);
	char* my_lstat(int client_sock);
	char *my_cat(int client_sock, char *catbuf, int catsize);
	 
#endif