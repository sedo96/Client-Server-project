#include "MessageVediEstrazione.h"

char* serializza_MessageVediEstrazione(struct MessageVediEstrazione* mve) {

	char *mve_buff = malloc(sizeof(struct MessageVediEstrazione));
	if(!mve_buff)
		return NULL;
	int i=1;
	mve_buff[0] = MESSAGEVEDIESTRAZIONEOPCODE;
	memcpy(&mve_buff[i], &mve->ultime_estrazioni, sizeof(int));
	i+= sizeof(int);
	memcpy(&mve_buff[i], &mve->numero_ruota, sizeof(int));
	i+= sizeof(int);
	
	memcpy(&mve_buff[i], mve->session_id, SESSION_ID_LENGTH*sizeof(char));
	
	return mve_buff;
}

struct MessageVediEstrazione* deserializza_MessageVediEstrazione(char *mve_buff){
	
	struct MessageVediEstrazione *mve = malloc(sizeof(struct MessageVediEstrazione));
	if(!mve_buff)
		return NULL;
	int i=1;

	memcpy(&mve->ultime_estrazioni, &mve_buff[i], sizeof(int));
	i+= sizeof(int);
	memcpy(&mve->numero_ruota, &mve_buff[i], sizeof(int));
	i+= sizeof(int);
	
	memcpy(mve->session_id, &mve_buff[i], SESSION_ID_LENGTH*sizeof(char));
	
	return mve;
}
