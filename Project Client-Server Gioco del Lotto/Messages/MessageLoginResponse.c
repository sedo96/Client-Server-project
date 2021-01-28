#include "MessageLoginResponse.h"

struct MessageLoginResponse *deserializza_messageLoginResponse (char *sesmg) {
	struct MessageLoginResponse *msu = malloc(sizeof(struct MessageLoginResponse));
	if(!msu) {
		return NULL;
	}
	int step = 1;	
	
	strncpy(msu->username, &sesmg[step], MAXLENGTHUSERNAME);
	strncpy(msu->sessionID, &sesmg[MAXLENGTHUSERNAME+step], SESSIONIDLENGTH);
	msu->username[MAXLENGTHUSERNAME-1] = '\0'; 
	msu->sessionID[SESSIONIDLENGTH-1] = '\0';
	return msu;
}

char* serializza_messageLoginResponse(struct MessageLoginResponse *msu) {
	size_t len = MAXLENGTHUSERNAME + SESSIONIDLENGTH + sizeof(char);
	char *msg_ser = malloc(len);
	if(!msg_ser) {
		return NULL;
	}
	int step = 0;
	msg_ser[step] = MESSAGELOGINRESPONSEOPCODE;
	strncpy(&msg_ser[++step], msu->username, MAXLENGTHUSERNAME);
	strncpy(&msg_ser[MAXLENGTHUSERNAME+step], msu->sessionID, SESSIONIDLENGTH);
	return msg_ser;
}
