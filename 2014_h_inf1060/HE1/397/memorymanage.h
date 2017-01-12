#ifndef _MEMORYMANAGE_H_
#define _MEMORYMANAGE_H_

typedef struct node {
    int length;
    // 15: 120 long 8 bites per block
    int dataindex[15];
    struct node* next;
    struct node* previous;
} 
node;

int getBit(unsigned long long number, int index);
unsigned long long toggleBit(unsigned long long number, int index);
int findEmpty(unsigned long long number);
char* getBlock(char* heap, int index);
void freenode(node *n, char *heap, unsigned long long *bitmap);

#endif