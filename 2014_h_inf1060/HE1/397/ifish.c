/*
KANDIDATNUMMER: 397
*/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <dirent.h>
# include <unistd.h>
# include <math.h>
# include "safefork.h"
# include "memorymanage.h"
# define MAX_CMD 120
# define DEBUG 1


char** tokenize (char *input) {
	int current = 0;
	int cnt = 0;
	int lastSpace = 0;
	int length = 0;
	char **param = malloc(21*sizeof(char*));
	int i = 0;

	do {
		if (isspace ( input[current]) || current == strlen(input)) {
			if (current > lastSpace) {
				length = current - lastSpace;
				param[cnt] = malloc(length * sizeof (char) + 1);
				strncpy ( param[cnt], input + lastSpace , length );
				param[cnt][length] = '\0';
				cnt++;
			}

			lastSpace = current + 1;
		}
	} while ( input[current++] != '\0' );

	param[cnt] = NULL;

	#ifdef DEBUG 
		while (param[i] != NULL) {
			printf("%s%s\n", "Debug info (tokens): ",  param[i]);
			i++;
		}
	#endif 

	return param;
} 

int run (char **param, char* pathSrc) {
	int cnt_ = 0;
	int result = 0;

	if (param[0] == NULL) {
		return 0;
	}

	while (param[cnt_] != NULL) {
		cnt_++;
	}

	cnt_ = cnt_ - 1;
	int wait = strcmp (param[cnt_], "&");

	if (wait == 0) {
		param[cnt_] = NULL;
	}

	char* path = malloc(strlen(pathSrc) * sizeof(char));
	strcpy(path, pathSrc);

	const char s[2] = ":";
   	char *token;
   
   	// get the first token
  	token = strtok(path, s);
   	
   	// walk through other tokens
   	while( token != NULL ) {
    	token = strtok(NULL, s);

    	DIR *dir = opendir(token);

    	if (dir != NULL) {
	    	struct dirent* file;

	    	while ((file = readdir(dir)) != NULL) {
	    		if (strcmp (file->d_name, param[0]) == 0) {
	    			// create child process
	    			pid_t pid = safefork();

	    			if ( pid == -1 ) {
	    				printf("%s\n", "Something went terribly wrong");
	    			}

	    			// child process
	    			else if ( pid == 0 ) {
	    				// find path of file to run -> combine token and file->d_name -> add /
	    				char *foundPath = malloc((strlen(token) + strlen(file->d_name)+1)*sizeof(char));
	    				sprintf (foundPath, "%s/%s", token, file->d_name) ;
	    				// execve-saken kjøres her
	    				execve(foundPath, param, NULL);
	    				free(foundPath);
	    				exit(0);
	    			}

	    			// parent
	    			else {	    				
	    				int status;
	    				if ( wait != 0) {
	    				// find last param entry before NULL. As long as not & --> waitpid
	    					waitpid(pid, &status, 0);
	    				} else {
	    					printf("%s %d\n", "Started process id:", pid);
	    				}
	    				result = 1;
	    			}
	    		}
	    	}

	    	closedir(dir);
	    }
   	}
	return result;
}

node* insertCmd (node* head, unsigned long long* bitmap, char* heap, char* command) {
	char* src = command;
	node *n = (node*)malloc(sizeof(node));
	n->length = 0;
	n->next = head;
	n->previous = NULL;

	if (head != NULL) {
		head-> previous = n;
	}

	int remain = ceil(strlen(command) / 8);

	while (remain >= 0) {
		int idex = findEmpty(*bitmap);
		// free space
		if (idex == -1) {
			node* current = n;
			node* prev = NULL;

			while (current-> next) {
				prev = current;
				current = current->next;
			}
			prev->next = NULL;

			freenode(current, heap, bitmap);
		} else {
			char* block = getBlock (heap, idex);
			strncpy(block, src, 8);
			src += 8;
			n->dataindex[n->length++] = idex;
			*bitmap = toggleBit(*bitmap, idex);
			remain--;
		}
	}
	return n; 
}

char *getCmd(node *current, char *heap) {
	char *cmd = malloc(sizeof(char) * 8 * current->length);
	for (int i = 0; i < current->length; i++) {
		strncpy(cmd + i * 8, getBlock(heap,current->dataindex[i]), 8);
	}
	return cmd;
}

void printhistory(node *head, char *heap) {
	int cnt = 0;
	node *current = head;
	node* prev = NULL;

	while (current != NULL) {
		prev = current;
		current = current->next;
		cnt++;
	}

	current = prev;
	printf("%s%d%s\n", "History list of the last ", cnt, " commands");

	while (current != NULL) {
		char *temp = getCmd(current, heap);
		printf("%s\n", temp);
		free(temp);
		current = current->previous;
	}
}

void freetokens(char** tokens) {
	int i = 0;

	while (tokens[i] != NULL) {
		free(tokens[i++]);
	}

	free(tokens);
}

int countnodes(node *node) {
	int cnt = 0;
	while (node) {
		node = node->next;
		cnt++;
	}

	return cnt;
}

int main (int argc, char *argv[]) {
	node *head = NULL;
	unsigned long long bitmap = 0;
	// allocates memory for max input and heap (bitmap)
	char input[MAX_CMD]; 
	char *heap = malloc (8 * 64);
	int cmd_count = 1;
	int copied = 0;

	char *path = getenv("PATH");

	while (1) {
		printf("%s%s%d ", getenv("USER"), "@ifish ", cmd_count++);

		if (copied == 0) {
			// reads input incl '\n'
			if (fgets(input, MAX_CMD, stdin) == NULL ) {
				exit(0);
			}

			// removes new line og replaces with '\0'
			int length = strlen(input);

			if ( length == 1) {
				continue;
			}

			input[length-1] = '\0';
		} else {
			copied = 0;
		}

		#ifdef DEBUG 
			fprintf(stderr, "%s%s\n", "Debug info (line read): ", input);
		#endif 

		head = insertCmd(head, &bitmap, heap, input);

		#ifdef DEBUG
			for (int i = 0; i < 64; i++) {
				int sth = getBit(bitmap, i);
				fprintf(stderr, "%d", sth);

				if ( i == 31) {
					fprintf(stderr, "\n");
				}
			}

			fprintf(stderr, "\n\n");

			for (int i = 0; i < 16; i++) {

				for (int j = 0; j < 4; j++) {
					char* block = getBlock(heap, i*4 + j);
					fprintf(stderr, "##%.*s", 8, block);
				}

				fprintf(stderr, "##\n");
			}
		#endif	

		char **array = tokenize(input);
		int tokens = 0;

		while ( array[tokens] != NULL ) {
			tokens++;
		}


		if ( strcmp(array[0],"h") == 0 ) {
			if (tokens == 1) {
				printhistory(head, heap);
			} else if (tokens == 2) {
				node *current = head;
				int iex = atoi(array[1]);
				if (iex >= countnodes(head)) {
					printf("%d%s\n", iex, " would go outside of the command list");
					freetokens(array);
					continue;
				}
				while (iex > 0) {
					current = current->next;
					iex--;
				}

				//  make sure the command is executed next time
				char *cmd = getCmd(current, heap);
				copied = 1;
				bzero(input, MAX_CMD);
				sprintf(input, "%s", cmd);
				free(cmd);
			} else if (tokens == 3 && strcmp(array[1], "-d") == 0) {
				int idx = atoi(array[2]);
				node *current = head;

				if (idx >= countnodes(head)) {
					printf("%d%s\n", idx, " would go outside of the command list");
					freetokens(array);
					continue;
				}
				while (idx > 0) {
					current = current->next;
					idx--;
				}

				// detach current from the list

				if (current == head) {
					head = current->next;
				}

				if (current->next) {
					current->next->previous = current->previous;
				}

				if (current->previous) {
					current->previous->next = current->next;
				}

				// free memory
				freenode(current, heap, &bitmap);
			}

		} else if ( strcmp (input, "exit") == 0 || strcmp (input, "quit") == 0) {
			exit(0);
		}	
		else if ( !run(array, path) ) {
			printf("%s\n", "Could not find command in path");
		}

		freetokens(array);
	}

	free(heap);
	node* n = head;

	while(n) {
		head = n;
		n = n->next;
		free(head);
	}
}

