#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define TOTALE_NUMERI 55

struct MessageVediEstrazioneResponse {

	char numeri[TOTALE_NUMERI];
};

char* serializza_MessageVediEstrazioneResponse(struct MessageVediEstrazioneResponse*);
struct MessageVediEstrazioneResponse* deserializza_MessageVediEstrazioneResponse(char*);
