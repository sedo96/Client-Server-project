#include <string.h>
#include <stdlib.h>

#define MESSAGELOGINOPCODE 0x01
#define MAXLENGTHUSERNAME 17
#define MAXLENGTHPASSWORD 17
#define MESSAGELOGINLENGTH MAXLENGTHUSERNAME+MAXLENGTHPASSWORD+1

struct MessageLogin {

	char username[MAXLENGTHUSERNAME];	//17 byte
	char password[MAXLENGTHPASSWORD];	//17 byte
};

char* serializza_messageLogin(struct MessageLogin*);
struct MessageLogin* deserializza_messageLogin(char*);
