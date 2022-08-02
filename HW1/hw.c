#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "input.h"
#include "diff.h"


int main(int argc, char *argv[]) {
    // pathname dei due file e del file output
    char path1[100] = "", path2[100] = "", path_out[100] = "";
    FILE* out = NULL; // puntatore allo stream del file output se fornito con -o
    bool options[5]; // maschera delle 5 possibili opzioni (-o a parte)

    for (int i = 0; i < 5; i++) { // inizializza le opzioni a false
        options[i] = false;
    }

    input(argc, argv, options, path1, path2, path_out); // elabora gli argomenti del comando

    if (path1[0] == '\0' || path2[0] == '\0'){ // se non sono stati forniti entrambi i pathname
        fprintf(stderr, "Devi fornire due filename.\n");
        exit(EXIT_FAILURE); // errore
    }

    valida_opt(options); // verifica la correttezza delle opzioni

    if (path_out[0] != '\0') { // se è stato fornito un file di output
        out = freopen(path_out, "w", stdout); // ridereziona stout su file out
        if (out == NULL) {// se l'apertura genera errore (es file non esiste)
            perror(path_out); // stampa errore
            exit(EXIT_FAILURE); // termina
        }
    }
    
    diff(path1, path2, options); // calcola il diff dei due file

    if (out != NULL) fclose(out); // se è stato fornito un file di output lo chiude

    return 0;
}
