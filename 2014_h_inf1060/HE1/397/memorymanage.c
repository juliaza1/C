#include "memorymanage.h"
# include <string.h>
# include <stdio.h>
# include <stdlib.h>

int getBit(unsigned long long number, int index) {
	unsigned long long newNumber = 1;
	newNumber <<= index;

	// AND
	return (number & newNumber) >> index;
}

unsigned long long toggleBit(unsigned long long number, int index) {
	unsigned long long newNumber = 1;
	newNumber <<= index;

	// XOR
	return number ^ newNumber;
}

int findEmpty(unsigned long long number) {
	for ( int i = 0; i < 64; i++ ) {
		if ( getBit (number, i) == 0 ) {
			return i;
		}
	}
	return -1;
}

char* getBlock(char* heap, int index) {
	return heap + index * 8;
}

void freenode(node *n, char *heap, unsigned long long *bitmap) {
	for (int i = 0; i < n->length; i++) {
		*bitmap = toggleBit(*bitmap, n->dataindex[i]);
		bzero(getBlock(heap, n->dataindex[i]), 8);
	}

	free(n);
}