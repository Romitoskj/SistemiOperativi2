/*
Controlla se una stringa inizia con un prefisso. Ritorna true se inizia con il
prefisso, false altrimenti.

- pre è la stringa del prefisso
- str la stringa su cui controllare
*/
bool starts_with(const char *pre, const char *str);


/*
Legge gli argomenti del comando e prepara l'elaborazione dei file.

- argc è il numero di argomenti del comando
- argv è l'array degli argomenti del comando
- options è l'array booleano delle opzioni con cui calcolare il diff
- path1 e path2 sono i path dei file di cui calcolare il diff
- path_out è il path del file di output se viene usata l'opzione -o
*/
void input(int argc, char *argv[], bool options[], char path1[], char path2[], char path_out[]);


/*
Aggiunge un'opzioe all'array delle opzioni passato.

- o è l'opzione da aggiungere
- options è l'array booleano delle opzioni con cui calcolare il diff
*/
void add_opt(char o, bool options[]);


/*
Aggiunge più opzioni all'array delle opzioni passato.

- o è l'array delle opzioni da aggiungere
- options è l'array booleano delle opzioni con cui calcolare il diff
*/
void add_n_opt(char o[], bool options[]);


/*
Termina il programma se ci sono combinazioni di opzioni errate.

- options array delle opzioni
*/
void valida_opt(bool options[]);
