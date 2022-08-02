#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "input.h"


bool starts_with(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}


void input(int argc, char *argv[], bool options[], char path1[], char path2[], char path_out[]) {
    for (int i = 1; i < argc; i++) { // per ogni argomento del comando
        if (starts_with("-", argv[i])) { // se inizia con - è una singola o più opzioni
            if (strlen(argv[i]) == 2) { // se lunga due caratteri è una sola opzione
                if (argv[i][1] == 'o') { // se -o
                    if (path_out[0] != '\0') { // se il file di output è già stato inserito
                        fprintf(stderr, "Puoi fornire il file di output con -o solo una volta.\n");
                        exit(EXIT_FAILURE); // errore
                    }
                    if (i == argc - 1 || argv[i + 1][0] == '-') { // se non ha un altro argomento dopo
                        fprintf(stderr, "L'opzione -o richiede il path di un file come argomento.\n");
                        exit(EXIT_FAILURE); // errore
                    } else strcpy(path_out, argv[++i]); // copia path output argomento successivo e saltalo (pre-incrementando i)
                } else add_opt(argv[i][1], options); // altrimenti aggiungi opzione
            } else add_n_opt(argv[i], options); // altrimenti sono più opzioni
        } else if (path1[0] == '\0') // altimenti se non è già stato inserito il path del primo file
            strcpy(path1, argv[i]); // imposta il path del primo file
        else if (path2[0] == '\0') // altimenti se non è già stato inserito il path del primo file
            strcpy(path2, argv[i]); // imposta il path del primo file
        else { // altrimenti (sono già stati forniti tuti i filename)
            fprintf(stderr, "Puoi fornire massimo due filename.\n");
            exit(EXIT_FAILURE); // errore
        }
    }
}


void add_opt(char o, bool options[]) {
    switch (o) {
        case 'g':
            options[0] = true;
            return;
        case 's':
            options[1] = true;
            return;
        case 'd':
            options[2] = true;
            return;
        case 'u':
            options[3] = true;
            return;
        case 'v':
            options[4] = true;
            return;
        case 'o': // opzione o gestita a parte
            fprintf(stderr, "L'opzione -%c necessita di un pathname come argomento.\n", o);
            exit(EXIT_FAILURE); // errore
        default: // se non è un opzione valida
            fprintf(stderr, "L'opzione -%c non è valida.\n", o); // errore
            exit(EXIT_FAILURE);
    }
}


void add_n_opt(char o[], bool options[]) {
    for (int i = 1; i < strlen(o); i++)
        add_opt(o[i], options);
}


void valida_opt(bool options[]) {
    bool opt = false;
    for (int i = 0; i < 5; i++) {
        opt = opt || options[i];
    }
    if (!opt) { // se non ci sono opzioni
        fprintf(stderr, "Devi fornire almeno un'opzione.\n"); // errore
        exit(EXIT_FAILURE);
    }
    
    if (options[2] && options[3]) { // se ci sono -d e -u insieme
        fprintf(stderr, "Le opzioni -d e -u non possono essere combinate.\n"); // errore
        exit(EXIT_FAILURE);
    }
    if (!options[2] && !options[3] && options[4]) { // se c'è -v ma non -d e -u
        fprintf(stderr, "L'opzione -v può essere combinata solo a -d e -u.\n"); // errore
        exit(EXIT_FAILURE);
    }
}