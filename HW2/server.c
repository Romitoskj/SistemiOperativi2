#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

// struttura per passare i parametri alla funzione di start dei thread
struct args {
    int sock;
    FILE *log;
    char ip[INET_ADDRSTRLEN];
};

/*
Stampa errore della syscall e termina il programma.

- *msg: il messaggio da stampare
*/
void error(const char *msg);

/*
Crea un socket TCP/IP in ascolto sulla porta 60000.
*/
int create_socket();

/*
Funzione da cui parte l'esecuzione di ogni nuovo thread. Ogni thread nuovo
thread corrisponde ad un nuovo client connesso al server.

- *args: puntatore ad una struttura args allocata in memoria
*/
void * start(void *args);

/*
Calcola il risultato dell'operazione richiesta e lo scrive nel buffer. In caso
di errore il buffer conterrà invece il carattere che identifica l'errore.

- *buffer: puntatore alla stringa che conterrà il risultato
- n1 e n2: i due operandi
- op: l'operatore (+ - * /)
*/
void calc(char *buffer, float n1, float n2, char op);

/*
Scrive sul file di log i dati relativi all'operazione effettuata. Garantisce
l'accesso concorrente ai vari thread.

- *log: file descriptor del file di log
- *ip: puntatore alla stringa che rappresenta l'indirizzo ip del client che ha 
    effettuato la richiesta
- n1 e n2: i due operandi
- op: l'operatore
- *res: la stringa che contiene il risultato o il carattere che codifica l'errore
- *timestamp: la stringa che contiene la data e l'ora della richiesta
*/
void log_write(FILE *log, char *ip, float n1, float n2, char op, char *res, char *timestamp);

// mutex per scrivere concorrentemente sul file di log
pthread_mutex_t lock;

int main(int argc, char *argv[]) {
    int sockfd = create_socket(), newsockfd; // socket descriptor
    pthread_t tid; // id dell'ultimo thread creato
    struct sockaddr_in client; // indirizzo e porta socket
    socklen_t client_len = sizeof(struct sockaddr_in); // dimensione sockaddr_in client
    FILE *log; // file descriptor del log
    struct args *param; // puntatore ai parametri del thread
    int e; // codice errore creazione thread

    if (pthread_mutex_init(&lock, NULL) != 0)  // inizializza il semaforo
        error("Errore nell'inizializzazione del mutex");

    // apertura file di log
    log = fopen("log.txt", "a");
        if (log == NULL) error("Apertura file di log");

    printf("Running...\n");
    while (1) {
        // accetta un nuovo client
        newsockfd = accept(sockfd, (struct sockaddr*) &client, &client_len);
        if (newsockfd < 0) error("Errore nell'accettazione del client");

        // alloca memoria per i parametri dello start del thread
         param = malloc(sizeof(struct args));
        // inizializza parametri
        param->sock = newsockfd; // socket descriptor
        param->log = log; // file descriptor del log
        // stringa dell'indirizzo ip del client
        if (inet_ntop(AF_INET, &(client.sin_addr), param->ip, INET_ADDRSTRLEN) == NULL)
            error("Errore nel convertire l'indirizzo del client in stringa");

        // crea thread del client
        e = pthread_create(&tid, 0, start, (void *) param);
        if (e != 0) {
            fprintf(stderr, "Errore nella creazione del thread: %s", strerror(e));
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}


void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


int create_socket() {
    int sockfd;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // crea un socket TCP/IP
    if (sockfd < 0) error("Errore nella creazione del socket");

    // inizializza indirizzo e porta socket
    addr.sin_family = AF_INET; // ipv4
    addr.sin_addr.s_addr = INADDR_ANY; // qualsiasi indirizzo
    addr.sin_port = htons(60000); // porta di ascolto

    // binding del socket
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        error("Errore nel binding del socket");

    if (listen(sockfd, 5) < 0) // può accettare massimo 5 client in coda
        error("Listen");

    return sockfd;
}


void * start(void *args) {
    // assegna parametri a variabili locali e dealloca la memoria
    struct args *a = (struct args*) args;
    int sock = a->sock;
    FILE *log = a->log;
    char ip[INET_ADDRSTRLEN];
    strcpy(ip, a->ip);
    free(a);

    float n1, n2; // operandi
    char buffer[256], out[512], op; // stringhe per input e output del socket e carattere dell'operando
    struct timespec t1, t2; // tempo ricezione richiesta, invio risposta e la loro differenza (in nano secondi)

    while (1) {
        // legge dal socket
        bzero(buffer, 256);
        if (read(sock, buffer, 255) <= 0)
            error("Errore nella lettura dal socket");

        // assegna il tempo della ricezione della richiesta
        if (clock_gettime(CLOCK_REALTIME, &t1) < 0) 
            error("clock_gettime");

        // se il formato della stringa letta è corretto
        if ( 3 == sscanf(buffer, "[ %c , %f , %f ]", &op, &n1, &n2))
            calc(buffer, n1, n2, op); // calcola il risultato
        else if (strcmp(buffer, "q\n") == 0) // altrimenti se è uguale a "q\n"
            break; // interrompi il ciclo
        else // altrimenti
            strcpy(buffer, "f"); // errore di formattazione
        
        // scrivi sul socket
        bzero(out, 512);
        if (clock_gettime(CLOCK_REALTIME, &t2) < 0)  // assegna il tempo di invio della risposta
            error("clock_gettime");
        sprintf(out, "[%ld, %ld, %s]", t1.tv_nsec, t2.tv_nsec, buffer);
        if (write(sock, out, strlen(out)) < 0) 
            error("Errore nella scrittura sul socket");

        // scrittura sul log
        log_write(log, ip, n1, n2, op, buffer, ctime(&t1.tv_sec));
    }
    close(sock); // chiude la connessione con il client
    return NULL;
}


void calc(char* buf, float n1, float n2, char op) {
    switch (op) {
            case '+':
                sprintf(buf, "%f", n1 + n2);
                break;
            case '-':
                sprintf(buf, "%f", n1 - n2);
                break;
            case '*':
                sprintf(buf, "%f", n1 * n2);
                break;
            case '/':
                if (n2 != 0) {
                    sprintf(buf, "%f", n1 / n2);
                } else {
                    strcpy(buf, "d"); // divisione per 0
                }                
                break;
            
            default:
                strcpy(buf, "o"); // operatore sconosciuto
        }
}


void log_write(FILE *log, char *ip, float n1, float n2, char op, char *buf, char *timestamp) {
    char res[256];
    if (pthread_mutex_lock(&lock) != 0) // blocca il semaforo
        error("Locking mutex");
    
    if (strcmp(buf, "f") == 0) // errore di formattazione
        fprintf(log, "CLIENT: %s,\tINPUT: Input non valido,\tRISULTATO: Errore (formattazione),\tDATA E ORA: %s", ip, timestamp);
    else {
        if (strcmp(buf, "o") == 0) // operatore non valido
            strcpy(res, "Errore (operatore non valido)");
        else if (strcmp(buf, "d") == 0) // divisione per zero
            strcpy(res, "Errore (divisione per 0)");
        else strcpy (res, buf); // altrimenti risultato valido
        
        // stampa sul file
        fprintf(log, "CLIENT: %s,\tINPUT: %f %c %f,\tRISULTATO: %s,\tDATA E ORA: %s", ip, n1, op, n2, res, timestamp);
    }
    
    if (fflush(log) != 0) // salva il buffer sul file
        error("Flush");
    if (pthread_mutex_unlock(&lock) != 0) // sblocca il semaforo
        error("Unlocking mutex");
}
