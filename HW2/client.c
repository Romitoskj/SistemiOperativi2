#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
Stampa errore della syscall e termina il programma.

- *msg: il messaggio da stampare
*/
void error(const char *msg);

/*
Crea un socket TCP/IP e lo connette al localhost sulla porta 60000.
Ritorna il socket descriptor creato.
*/
int create_socket();

/*
Legge dallo standard input e prepara la stringa da inviare al server.

- buf: indirizzo del buffer dove scrivere la stringa da inviare
- size: la dimensione del buffer
*/
void input(char *buf, size_t size);

/*
Stampa sullo standard output la stringa ricevuta in risposta dal server.

- buf: indirizzo della stringa ricevuta
*/
int output(char *buf);

int main(int argc, char *argv[]) {
    int sockfd = create_socket(); // socket descriptor
    int r; // numero byte letti dal socket
    size_t const BUF_SIZE = 256; // dimensione del buffer
    char buffer[BUF_SIZE]; // buffer stringa usata per input e output std e socket

    printf("CALCOLATRICE\n\n");
    printf("Potrai scrivere semplici espressioni della forma: 'operando1 operatore operando2'.\n");
    printf("Visualizzerai il risultato e il tempo di servizio a schermo.\n");
    printf("Per uscire digitare 'q' e premere invio.\n");

    while (1) {
        input(buffer, BUF_SIZE); // legge da stdin

        // scrive sul socket
        if (write(sockfd, buffer, strlen(buffer)) < 0)
            error("Errore nella scrittura sul socket");

        // se inserita 'q' esce dal ciclo
        if (strcmp(buffer, "q\n") == 0) break;

        // "pulisce il buffer"
        bzero(buffer, BUF_SIZE);
        // legge dal socket
        r = read(sockfd, buffer, BUF_SIZE - 1);
        if (r < 0)
            error("Errore nella lettura dal socket");
        else if(r == 0) {
            fprintf(stderr, "Server irragiungibile.\n");
            break;
        }

        if (output(buffer) < 0) { // per debug, il file server.c manda risposta nel giusto formato
            fprintf(stderr, "Risposta del server con formattazione errata.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Chiusura del programma...\n");
    close(sockfd); // chiusura socket
    return 0;
}


void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


int create_socket() {
    int sockfd; // socket descriptor
    struct sockaddr_in addr; // indirizzo e porta socket
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // crea un socket TCP/IP
    if (sockfd < 0) error("Errore nella creazione del socket");

    // inizializza indirizzo e porta socket
    addr.sin_family = AF_INET; // ipv4
    inet_aton("127.0.0.1", &addr.sin_addr); // localhost
    addr.sin_port = htons(60000); // porta di ascolto

    // connessione al server
    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
        error("Errore durante la connessione al server");

    return sockfd;
}


void input(char *buf, size_t const size) {
    float n1, n2; // operandi
    char op; // operatore
    
    bzero(buf, size); // "pulisce" il buffer
    printf("\n> ");
    fgets(buf, size - 1, stdin); // legge da stdin
    // se ha letto un float un char e un altro float il formato è corretto
    if (3 == sscanf(buf, "%f %c %f", &n1, &op, &n2)) {
        bzero(buf, size); // "pulisce" il buffer
        // assegna al buffer la stringa nel formato richiesto dal server
        sprintf(buf, "[%c, %f, %f]", op, n1, n2); 
    }
}


int output(char *buf) {
    float res; // risultato operazione
    long t1, t2, diff; // tempo ricezione richiesta, invio risposta e la loro differenza (in nano secondi)
    char err; // carattere che identifica un errore
    // f: formattazione, d: divisione per 0, o: operatore sconosciuto

    if (3 == sscanf(buf, "[%ld, %ld, %s]", &t1, &t2, buf)) {
        diff = t2 - t1;
        if (1 == sscanf(buf, "%f", &res))
            printf("= %f\t\t[T: %ld ns]\n", res, diff);
        else if (1 == sscanf(buf, "%c", &err)) {
            switch (err) {
                case 'f':
                    printf("%s\t\t[T: %ld ns]\n", "Errore di formattazione!", diff);
                    break;
                case 'd':
                    printf("%s\t\t[T: %ld ns]\n", "Divisione per 0!", diff);
                    break;
                case 'o':
                    printf("%s\t\t[T: %ld ns]\n", "Operatore non valido!", diff);
                    break;
            }
        } else return -1;
        return 0;
    } else return -1;
}
