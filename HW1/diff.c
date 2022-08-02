#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "diff.h"


void diff(char path1[], char path2[], bool options[]) {
    const int n_lines = 50; // numero righe da leggere in ogni elaborazione (50)
    char *l_f1[n_lines], *l_f2[n_lines]; // righe dei due file
    int lette_f1, lette_f2; // contatori righe lette
    int count = 0; // conteggio delle elaborazioni dei blocchi di righe
    FILE* f1 = fopen(path1, "r"); // file pointer path1
    FILE* f2 = fopen(path2, "r"); // file pointer path2
    bool u = true; // flag che indica se i file sono uguali
    
    
    if (f1 == NULL) { // se l'apertura genera errore (es file non esiste)
        perror(path1); // stampa errore
        return; // ritorna errore
    }
    if (f2 == NULL) {
        perror(path2);
        return;
    }

    do {
        // legge n_lines righe dai due file
        lette_f1 = leggi_n_righe(f1, l_f1, n_lines);
        lette_f2 = leggi_n_righe(f2, l_f2, n_lines);

        if (lette_f1 == 0 && lette_f2 == 0) // se lette 0 righe da entrambi i file
            break; // interrompi ciclo
        
        // se sono ancora uguali e l'opzione -g e/o -s sono settate
        if (u && (options[0] || options[1])) // se sono già diversi non valuta di nuovo
            u = f_uguali(l_f1, l_f2, lette_f1, lette_f2); // controlla se le righe lette sono uguali

        if (options[2] || options[3]) // se l'opzione -d o -u sono settate
            enum_righe(path1, path2, l_f1, l_f2, lette_f1, lette_f2, count * n_lines, options[3], options[4]);
            // stampa i numeri delle righe diverse o uguali a seconda opzione

        count ++; // incrementa numero delle elaborazioni

        // dealloca righe lette
        for (int i = 0; i < lette_f1; i++)
            free(l_f1[i]);
        for (int i = 0; i < lette_f2; i++)
            free(l_f2[i]);
    } while (lette_f1 == n_lines && lette_f2 == n_lines); // finché vengono lette n_lines da entrambi i file

    // chiude i file
    fclose(f1);
    fclose(f2);

    if (options[0] && !u) // output opzione g
        printf("%s e %s sono differenti\n", path1, path2);
    if (options[1] && u) // output opzione s
        printf("%s e %s sono uguali\n", path1, path2);
}




int leggi_n_righe(FILE* F, char* lines[], const size_t n) {
    const size_t len = 80;  // lunghezza stimata di una riga
    size_t buf_size = len;  // byte allocati ultima riga
    size_t char_num = 0;    // lunghezza effettiva dell'ultima riga
    int lette;              // righe lette

    // finché non legge EOF o legge n righe
    for (lette = 0; char_num != -1 && lette < n; lette++) {
        lines[lette] = malloc(len); // alloca memoria per una riga
        buf_size = len; // imposta la dimensione del buffer alla lunghezza simata di una riga
        char_num = getline(&lines[lette], &buf_size, F);
        // legge la prossima riga da F, la memorizza e restituisce numero caratteri

        if (char_num == -1) { // se ha letto EOF
            free(lines[lette]); // dealloco lo spazio non usato
            return lette; // ritorna numero righe lette
        }

        if (lines[lette][char_num - 1] == '\n') // rimuove new line dalla riga se presente
            lines[lette][char_num - 1] = '\0';
    }

    return lette; // ritorna numero di righe lette
}




bool f_uguali(char* f1_lines[], char* f2_lines[], int f1_lette, int f2_lette) {
    if (f1_lette != f2_lette) // se hanno lunghezze diverse
        return false; // sono diversi
    for (int i = 0; i < f1_lette; i++) { // per ogni riga di f1
        if(strcmp(f1_lines[i], f2_lines[i]) != 0) // se la corrispondente di f2 è diversa
            return false; // sono diversi
    }
    return true; // altrimenti uguali
}




void enum_righe(char* f1, char* f2, char* f1_lines[], char* f2_lines[], int f1_lette, int f2_lette, int tot, bool u, bool v) {
    int len = (f1_lette < f2_lette) ? f1_lette : f2_lette; // numero righe da confrontare: min(f1_lette, f2_lette)
    bool uguali; // flag che indica se due righe corrispondenti sono uguali

    for (int i = 0; i < len; i++) { // per ogni riga dei due file
        uguali = strcmp(f1_lines[i], f2_lines[i]) == 0; // confronta la corrispondente

        if (u && uguali || !u && !uguali) { // se sono richieste le righe uguali e sono effettivamente uguali
                                            // o se sono richieste le righe diverse e sono effettivamente diverse
            if (v) { // a seconda opzione -v stampa solo il numero di riga o il contenuto di esse
                printf("#%d,\t%s:\t\"%s\"\n#%d,\t%s:\t\"%s\"\n", tot + i + 1, f1, f1_lines[i], tot + i + 1, f2, f2_lines[i]);
            } else 
                printf("#%d\n", tot + i + 1);
        }
    }
}
