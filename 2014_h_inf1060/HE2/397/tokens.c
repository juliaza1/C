#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"

char** tok (char *input) {
	int current = 0;
	int cnt = 0;
	int lastSpace = 0;
	int length = 0;
	char **param = malloc(200*sizeof(char*));

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

	return param;
} 