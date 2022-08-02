/*
Calcola il diff tra i file passati in input e lo stampa a video.

- path1 e path2 sono stringhe che rappresentano i percorsi ai file.
- options è un array booleano che rappresenta le varie opzioni per calcolare il diff:
    option  | posizione
        -g  |   0
        -s  |   1
        -d  |   2
        -u  |   3
        -v  |   4
*/
void diff(char path1[], char path2[], bool options[]);


/*
Legge n righe dal file F dato in input e assegna i puntatori alle righe all'array
lines.

- F è il puntatore allo stream del file
- lines è l'array dove inserire il risultato, ossia i puntatori alle righe
- n è il numero di righe da leggere
*/
int leggi_n_righe(FILE* F, char* lines[], const size_t n);


/*
Controlla se le righe dei due file sono uguali. Ritorna true se uguali false
altrimenti.

- f1_lines e f2_lines sono gli array dei puntatori alle righe lette dai due file
- f1_lette e f2_lette sono i numeri delle righe lette dei due file
*/
bool f_uguali(char* f1_lines[], char* f2_lines[], int f1_lette, int f2_lette);


/*
Date in input le righe lette dei due file stampa i numeri delle righe diverse o
uguali a seconda del flag u (se true le righe uguali, altrimenti le diverse). Se
il flag v è true stampa a video le anche il testo delle righe.

- f1 e f2 sono le stringheche rappresentano i percorsi dei due file
- f1_lines e f2_lines sono gli array dei puntatori alle righe lette dei due file
- f1_lette e f2_lette sono i numeri delle righe lette dei due file
- tot è il totale delle righe lette nelle elaborazioni precedenti
- u e v sono i flag che rappresentano le opzioni -u e -v
*/
void enum_righe(char* f1, char* f2, char* f1_lines[], char* f2_lines[], int f1_lette, int f2_lette, int tot, bool u, bool v);
