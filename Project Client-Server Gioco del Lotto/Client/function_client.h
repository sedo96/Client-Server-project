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


#include "../Messages/MessageSignUp.h"
#include "../Messages/MessageLogin.h"
#include "../Messages/MessageLoginResponse.h"
#include "../Messages/MessageGiocata.h"
#include "../Messages/MessageVediGiocata.h"
#include "../Messages/MessageVediVincite.h"
#include "../Messages/MessageVediEstrazione.h"
#include "../Messages/MessageVediEstrazioneResponse.h"


int socket_id, addrlen;
struct sockaddr_in add;
unsigned short port_number;

char username_loggato[17]; //variabile globale dove salvo l'username che si logga
char session_id[11]; //variabile globale dove salvo il session_id generato casualmente dal server al login

void help(char*);
int signup(char*, char*);
int login(char*, char*);
void scrivi_giocata(); //funzione che preleva la giocata da terminale e la inserisce in una schedina
int ottieni_numero_indice(char*); //funzione che restituisce un numero associato al nome di ogni ruota
int invia_giocata(struct MessageGiocata);
bool vedi_giocate(char);
bool vedi_estrazioni(int, char*);
bool vedi_vincite();
char* ottieni_tipo_vittoria(char); //funzione che restituisce il tipo di vittoria in base all'iniziale passata come parametro
void esci();
