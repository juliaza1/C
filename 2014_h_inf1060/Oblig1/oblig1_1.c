// /*
//     Mail fra Aleksi Luukkonen ang. norske bokstaver.
//     -> lov å ignorere dem i oppgave 1
// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct node {
    char *name;
    struct node* next;
} 
node;

int read(FILE *fp);

int isVowel(char c);

void freeNodes(node* head) {
    node* prev = NULL;

    for (node* current = head; current; current=current->next) {
        if (prev) {
            free(prev);
        }

        free(current->name);
        prev = current;
    }

    if (prev) {
        free(prev);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
node* readFile(const char* file) {
    FILE *fp = fopen(file, "r");
    char *line = NULL; 
    size_t len = 0;
    size_t read;

    node* head = NULL; // first node
    node* ret = NULL;

    if (fp == NULL)
        exit(EXIT_FAILURE);

    //leser inn fila linje for linje og skriver ut. Lagrer siste linje forst
    while ((read = getline(&line, &len, fp)) != -1)
    {
        node *n = (node*)malloc(sizeof(*n));

        if (ret == NULL) {
            ret = n;
        }

        n->name = strdup(line); /* copy the line since it can get reused by getline */

        if (head != NULL) {
            head->next = n;
        }
        
        head = n;
    }
    // frigir ressursene
    if (line)
        free(line);

    fclose(fp);

    return ret;
}

int main(int argc, char **argv) {
    // sjekker for riktig antall argumenter
    if (argc != 3) {
    printf("Feil antall argumenter. Bruk 'command' + 'input_file' \n");
    exit(1);
    }

    node* file = readFile(argv[2]);

////////////////////// print //////////////////////////////////////////////////////
    
    if ((strcmp(argv[1], "print")) == 0 ) {
        printf("\nPrint file:\n");

        for (node* cur=file; cur; cur=cur->next) {
            printf("%s\n", cur->name);
        }
    }

////////////////////// replace //////////////////////////////////////////////////////

    if ((strcmp(argv[1], "replace")) == 0 ) {

        printf("\nReplace with:\n");
        char replace_c;
        scanf("%c", &replace_c);

        if (!isVowel(replace_c)) {
            freeNodes(file);
            exit(2);
        }

        node *n = file;
        // itererer over listen og sjekker hver char pr linje
        // hvis det er en vokal erstatter det den med input-char
        while(1) {
            int i = 0;
            while(n->name[i] != '\0')
            {
                if(n->name[i] == 'a' || n->name[i] == 'e' || n->name[i] == 'i' 
                    || n->name[i] == 'o' || n->name[i] == 'u' || n->name[i] == 'y') {
                    n->name[i] = replace_c;
                }
                printf("%c", n->name[i]);

                i =  i + 1;
            }

            if( n->next == NULL)
                break;
            n = n->next;

        }
    }
////////////////////// remove //////////////////////////////////////////////////////

    if ((strcmp(argv[1], "remove")) == 0 ) {

        printf("\nRemove vowels:\n");
        
        // remove fungerer som replace bare at den skriver ut ved å hoppe over vokalene
        node *n = file;
        while(1) {
            int i = 0;
            while(n->name[i] != '\0')
            {
                if(n->name[i] != 'a' && n->name[i] != 'e' && n->name[i] != 'i' 
                    && n->name[i] != 'o' && n->name[i] != 'u' && n->name[i] != 'y')
                    printf("%c", n->name[i]);
                i =  i + 1;
            }

            if( n->next == NULL)
                break;
            n = n->next;
        }
    }


////////////////////// random //////////////////////////////////////////////////////

    if ((strcmp(argv[1], "random")) == 0 ) {

        printf("\nRandom line:");
        node *n = file;

        // genererer random nummer som ikke overskriver maks antall linjer
        srand(time(NULL));
        int count = 0;
        for (node* ctr=file; ctr; ctr=ctr->next) count++;

        int randnr = rand() % count;
        int linecnt = 0;

        // skriver ut random linje
        for(linecnt = 0; linecnt < randnr; linecnt++) n = n->next;
        printf("\t %s \n", n->name);

    }


////////////////////// length //////////////////////////////////////////////////////

    if ((strcmp(argv[1], "len")) == 0 ) {
        node *n = file;
        int countchar = 0;

        // itererer over listen og teller char
        while(1) {
            int i = 0;

            while(n->name[i] != '\0')
            {
                countchar++;
                i++;
            }

            if( n->next == NULL)
                break;
            n = n->next;

        }

        printf("\nThe text is %d characters long.\n", countchar);
    }

    freeNodes(file);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// funksjon som sjekker input
int isVowel(char c)
{
    if(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y') {
        return 1;
    }
    else {
        printf("Ikke vokal!\n");
        return 0;
    }
    return 0;
}


