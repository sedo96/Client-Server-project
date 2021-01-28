#include "MessageVediEstrazioneResponse.h"

char* serializza_MessageVediEstrazioneResponse(struct MessageVediEstrazioneResponse* mve) {

	char *mve_buff = malloc(sizeof(struct MessageVediEstrazioneResponse));
	if(!mve_buff)
		return NULL;
	
	memcpy(&mve_buff[0], mve->numeri, sizeof(char)*TOTALE_NUMERI);
	
	return mve_buff;
}

struct MessageVediEstrazioneResponse* deserializza_MessageVediEstrazioneResponse(char *mve_buff){
	
	struct MessageVediEstrazioneResponse *mve = malloc(sizeof(struct MessageVediEstrazioneResponse));
	if(!mve_buff)
		return NULL;

	memcpy(mve->numeri, &mve_buff[0], sizeof(char)*TOTALE_NUMERI);
	
	return mve;
}
