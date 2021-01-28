#include <string.h>
#include <stdlib.h>

#define MESSAGELOGINRESPONSEOPCODE 0x02
#define MAXLENGTHUSERNAME 17
#define SESSIONIDLENGTH 11
#define MESSAGELOGINRESPONSELENGTH MAXLENGTHUSERNAME+SESSIONIDLENGTH+1

struct MessageLoginResponse {

	char username[MAXLENGTHUSERNAME];	//17 byte
	char sessionID[SESSIONIDLENGTH];	//11 byte
};

char* serializza_messageLoginResponse(struct MessageLoginResponse*);
struct MessageLoginResponse* deserializza_messageLoginResponse(char*);
