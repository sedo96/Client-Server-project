#include "MessageSignUp.h"

struct MessageSignUp *deserializza_messageSignUp (char *sesmg) {
	struct MessageSignUp *msu = malloc(sizeof(struct MessageSignUp));
	if(!msu) {
		return NULL;
	}
	int step = 1;	
	
	strncpy(msu->username, &sesmg[step], MAXLENGTHUSERNAME);
	strncpy(msu->password, &sesmg[MAXLENGTHUSERNAME+step], MAXLENGTHPASSWORD);
	msu->username[MAXLENGTHUSERNAME-1] = '\0'; 
	msu->password[MAXLENGTHPASSWORD-1] = '\0';
	return msu;
}

char* serializza_messageSignUp(struct MessageSignUp *msu) {
	size_t len = MAXLENGTHUSERNAME + MAXLENGTHPASSWORD + sizeof(char);
	char *msg_ser = malloc(len);
	if(!msg_ser) {
		return NULL;
	}
	int step = 0;
	msg_ser[step] = MESSAGESIGNUPOPCODE;
	strncpy(&msg_ser[++step], msu->username, MAXLENGTHUSERNAME);
	strncpy(&msg_ser[MAXLENGTHUSERNAME+step], msu->password, MAXLENGTHPASSWORD);
	return msg_ser;
}

