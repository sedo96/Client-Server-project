#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MESSAGEVEDIESTRAZIONEOPCODE 0x05
#define SESSION_ID_LENGTH 11


struct MessageVediEstrazione {

	int ultime_estrazioni;
	int numero_ruota;
	char session_id[SESSION_ID_LENGTH];
};

char* serializza_MessageVediEstrazione(struct MessageVediEstrazione*);
struct MessageVediEstrazione* deserializza_MessageVediEstrazione(char*);
