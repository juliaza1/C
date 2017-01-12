#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char convertChar (char n) {
	// konverter som tabellen sier
	if (n == ' ') 
		return 0;
	if (n == ':')
		return 1;
	if (n == '@')
		return 2;
	if (n == '\n')
		return 3;

	return 0;
}

char decodeChar(char coded, int shift)
{
	// finn ut hvor mange biter vi må skifte til hoeyre, og dekod karakteren
	int pot = (int)pow(2, shift*2);
	char bits = (coded & 3*pot) >> shift*2;

	if (bits == 0) {
		return ' ';
	}
	if(bits == 1) {
		return ':';
	}
	if (bits == 2) {
		return '@';
	}
	if (bits == 3) {
		return '\n';
	}

	return 0;
}

int print_file (FILE *fp) {
    char c;
    fseek(fp,0,SEEK_SET);

    // les filen og skriv ut alle karakterene
    while(1) {
        c = getc(fp);

        if(c == EOF)
            break;

        else
            printf("%c", c);
    }
    return 0;
}

int encode_file (FILE *in, FILE *out) {

    char c, compressed = 0;
    int charNumber = 0;

    fseek(in,0,SEEK_SET);
    fseek(out, 0, SEEK_SET);
 
    while(1) {
        c = fgetc(in);

        if(c == EOF)
            break;
        else {
        	// pakk de to bitene inn i charen
        	char x = convertChar(c);
        	int rest = 3 - (charNumber % 4);
        	compressed |= x << (rest*2);
        }

        // skriv til filen for hver fjerde inputkarakter
        if (charNumber > 0 && (charNumber % 4) == 3) {
        	fputc(compressed, out);
        	compressed = 0;
        }

        charNumber++;
    }

    // soerg for at alt er skrevet
    fputc(compressed, out);

    return 0;
}

int decode_file(FILE* in) {
	char c;
	fseek(in, 0, SEEK_SET);

	while (1) {
		c = fgetc(in);

		if (c == EOF) 
			break;

		for (int i = 0; i < 4; ++i) {
			char decoded = decodeChar(c, 3 - i);
			printf("%c", decoded);
		}
	}

	return 0;
}

int main(int argc, char **argv) {

//______________________________________ print ______________________________________

	if ((strcmp(argv[1], "p")) == 0 ) {
		// sjekker om antall argumenter stemmer. avslutt hvis det ikke er tilfellet
		if (argc != 3) {
			printf("Feil antall argumenter. Bruk 'input_file' + 'output_file' \n");
			exit(1);
		}

		// Aapner filen med navn oppgitt i argv[2] til lesing
		FILE *file = fopen(argv[2], "r"); 	
		// Sjekk om filen er aapen. Hvis ikke, vis feilmelding og avslutt
		if (file == NULL) {
			printf("Filen kunne ikke åpnes.\n");
			exit(2);
		}	
		printf("Print file: \n");
        print_file(file);
		
        fclose(file);
        printf("\n");

		return 0;
	} 

	if ((strcmp(argv[1], "e")) == 0 ) {
		// sjekker om antall argumenter stemmer. avslutt hvis det ikke er tilfellet
		if (argc != 4) {
			printf("Feil antall argumenter. Bruk 'input_file' + 'output_file' \n");
			exit(1);
		}

		// Aapner filen med navn oppgitt i argv[2] til lesing
		FILE *file = fopen(argv[2], "r"); 	
		FILE *file2 = fopen(argv[3], "w"); 	
		// Sjekk om filen er aapen. Hvis ikke, vis feilmelding og avslutt
		if (file == NULL || file2 == NULL) {
			printf("Filen kunne ikke åpnes.\n");
			exit(2);
		}	
		printf("Encoding file: \n");
        encode_file(file, file2);
		
        fclose(file);
        fclose(file2);
        printf("\n");

		return 0;
	} 


	if ((strcmp(argv[1], "d")) == 0 ) {
		// sjekker om antall argumenter stemmer. avslutt hvis det ikke er tilfellet
		if (argc != 3) {
			printf("Feil antall argumenter. Bruk 'input_file' + 'output_file' \n");
			exit(1);
		}

		// Aapner filen med navn oppgitt i argv[2] til lesing
		FILE *file = fopen(argv[2], "r"); 	
		// Sjekk om filen er aapen. Hvis ikke, vis feilmelding og avslutt
		if (file == NULL) {
			printf("Filen kunne ikke åpnes.\n");
			exit(2);
		}	
		printf("Decoding file file: \n");
        decode_file(file);
		
        fclose(file);
        printf("\n");

		return 0;
	}
	
	return 0;
}



