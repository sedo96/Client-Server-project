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
#include <time.h>

#define TOT_RUOTE 11
#define NUMERI_PER_RUOTA 5
#define NUMERI_GIOCATI 10
#define MAX_IMPORTI 5

struct Schedina {

	char ruote[TOT_RUOTE];
	int numeri[NUMERI_GIOCATI];
	float importi[MAX_IMPORTI];
	
};

void* thread_estrazioni(int *); //effettua le estrazioni ogni periodo di tempo
int trova_numero_indice(char *); //funzione che restituisce un numero associato al nome di ogni ruota
struct Schedina* deserializza_schedina(char *);
void calcola_vincite(char *); //funzione che calcola la vincita delle giocate attive nell'ultima estrazione
int ambi_generabili(int);//funzione che restituisce quanti ambi si possono generare in base a quanti numeri sono stati giocati
int terni_generabili(int);
int quaterne_generabili(int);
int cinquine_generabili(int);
