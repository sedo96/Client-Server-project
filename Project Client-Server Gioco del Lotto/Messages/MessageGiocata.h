#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MESSAGEGIOCATAOPCODE 0x03
#define NUMERORUOTE 11
#define MAXNUMERI 10
#define MAXIMPORTI 5
#define SESSION_ID_LENGTH 11


struct MessageGiocata {

	char opcode;			//1
	uint8_t quanteruote;	//1
	uint8_t quantigiocati;	//1
	uint8_t quantiimporti;	//1
	char ruote[NUMERORUOTE]; //1*11
	int numeri[MAXNUMERI];	//4*10
	float importi[MAXIMPORTI];	//4*5
	char session_id[SESSION_ID_LENGTH]; //10
};

struct MessageGiocata* deserializza_messageGiocata(char*);
char* serializza_messageGiocata(struct MessageGiocata*);

