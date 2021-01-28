#define _GNU_SOURCE
#define _USE_XOPEN

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>


#include "../Messages/MessageSignUp.h"
#include "../Messages/MessageLogin.h"
#include "../Messages/MessageLoginResponse.h"
#include "../Messages/MessageGiocata.h"
#include "../Messages/MessageVediGiocata.h"
#include "../Messages/MessageVediVincite.h"
#include "../Messages/MessageVediEstrazione.h"
#include "../Messages/MessageVediEstrazioneResponse.h"
#include "gestore_estrazioni.h"

struct thread_arg { //struttura usata per passare al client_handler il socket_id e l'indirizzo IP del client

	int ns;
	unsigned long addr_IP;
};

void* client_handler(struct thread_arg*);
int invia_errori_client(int, struct MessageLoginResponse *, int); //funzione utilizzata per inviare il messaggio d'errore al client nel caso si sia sbagliato un tentativo 
bool user_isBlocked(unsigned long); //funzione che verifica se un utente si è loggato 30min dopo il blocco dell'ip e quindi deve essere sbloccato, oppure no
char* serializza_tm(struct tm*); //funzione che mi restituisce un buffer data la struttura tm
struct tm* deserializza_tm(char*); //funzione che fa il viceversa
int estrai_tentativi_ip(unsigned long); //funzione che dato l'ip, restituisce il numero di tentativi sbagliati da quell'ip
int aggiorna_file_IP_list(int, unsigned long); //funzione che aggiorna il numero di tentativi se è stato effettuato un tentativo di login errato e verifica se sono stati effettuati 3 tentativi sbagliati, e nel caso blocca l'ip
bool signup_handler(char*, int);
bool login_handler(char*, int, char **, char **);
char* genera_session_id(int); //funzione che genera una sequenza casuale di 10 caratteri alfanumerici
bool invia_giocata_handler(char*, int, char *, char **);
void inserisci_giocata(FILE*, struct MessageGiocata*); //funzione che inserisce la giocata effettuata da un utente nel proprio file 
bool vedi_giocate_handler(char*, int, char *, char **);
int ottieni_numero_indice(char*); //funzione che restituisce un numero associato al nome di ogni ruota
bool vedi_estrazione_handler(char*, int, char *, char **);
bool vedi_vincite_handler(char*, int, char *, char **);

