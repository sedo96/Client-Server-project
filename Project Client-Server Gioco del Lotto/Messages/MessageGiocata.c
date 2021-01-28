#include "MessageGiocata.h"

struct MessageGiocata* deserializza_messageGiocata (char *sesmg) {

	struct MessageGiocata *msu =  malloc(sizeof(struct MessageGiocata));
	if(!msu) {
		return 0;
	}

	
	msu->opcode = MESSAGEGIOCATAOPCODE;
	msu->quanteruote = sesmg[1];
	msu->quantigiocati = sesmg[2];
	msu->quantiimporti = sesmg[3];

	int step = 4;	
	

	memcpy(msu->ruote, &sesmg[step], NUMERORUOTE); 
	step += NUMERORUOTE;


	memcpy(msu->numeri, &sesmg[step], MAXNUMERI*sizeof(int));
	step += MAXNUMERI*sizeof(int);

	memcpy(msu->importi, &sesmg[step], MAXIMPORTI*sizeof(float));
	step += MAXIMPORTI*sizeof(float);
	
	memcpy(msu->session_id, &sesmg[step], SESSION_ID_LENGTH*sizeof(char));
	return msu;
}

char* serializza_messageGiocata(struct MessageGiocata *msu) {
	int len = sizeof(struct MessageGiocata);

	char *msg_ser = malloc(len);
	if(!msg_ser) {
		return NULL;
	}
	int step = 0;
	
	msg_ser[step] = MESSAGEGIOCATAOPCODE;
	step++;
	msg_ser[step] = msu->quanteruote;
	step++;
	msg_ser[step] = msu->quantigiocati;
	step++;
	msg_ser[step] = msu->quantiimporti;
	step++;


	memcpy(&msg_ser[step], msu->ruote, NUMERORUOTE);
	step += NUMERORUOTE;

	memcpy(&msg_ser[step], msu->numeri, MAXNUMERI*sizeof(int));
	step += MAXNUMERI*sizeof(int);

	memcpy(&msg_ser[step], msu->importi, MAXIMPORTI*sizeof(float));

	step += MAXIMPORTI*sizeof(float);
	
	memcpy(&msg_ser[step], msu->session_id, SESSION_ID_LENGTH*sizeof(char));

	return msg_ser;
}
