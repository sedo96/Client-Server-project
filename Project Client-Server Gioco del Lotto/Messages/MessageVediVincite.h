#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MESSAGEVEDIVINCITEOPCODE 0x06
#define MAX_NUMERI 10
#define SESSION_ID_LENGTH 11


struct MessageVediVincite {

	char opcode;
	int indice_ruota;
	int numeri[MAX_NUMERI];
	char tipo_vittoria; //iniziale della vincita ottenuta
	float importo_vinto;
	char session_id[SESSION_ID_LENGTH];
};

char* serializza_MessageVediVincite(struct MessageVediVincite*);
struct MessageVediVincite* deserializza_MessageVediVincite(char*);
struct MessageVediVincite* copiaRiga_MessageVediVincite(char*);
int ottieni_indice(char*);
char ottieni_iniziale(char*);
