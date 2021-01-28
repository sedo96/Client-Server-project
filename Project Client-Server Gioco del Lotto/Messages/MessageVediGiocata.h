#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MESSAGEVEDIGIOCATAOPCODE 0x04
#define TOT_RUOTE 11
#define TOT_NUMERI 10
#define TOT_IMPORTI 5
#define SESSION_ID_LENGTH 11

struct MessageVediGiocata {

	char ruote[TOT_RUOTE];
	int numeri[TOT_NUMERI];
	float importi[TOT_IMPORTI];
	char session_id[SESSION_ID_LENGTH];
};

char* serializza_MessageVediGiocata(struct MessageVediGiocata*);
struct MessageVediGiocata* deserializza_MessageVediGiocata(char*);

