#include <string.h>
#include <stdlib.h>

#define MESSAGESIGNUPOPCODE 0x00
#define MAXLENGTHUSERNAME 17
#define MAXLENGTHPASSWORD 17
#define MESSAGESIGNUPLENGTH MAXLENGTHUSERNAME+MAXLENGTHPASSWORD+1


/*
|	  1		|	  17			  17
|			|
|	OPCODE	|	USERNAME	|	PASSWORD
|			|
|			|
*/

struct MessageSignUp {
	char username[MAXLENGTHUSERNAME];					//17 byte
	char password[MAXLENGTHPASSWORD];					//17 byte
};


char* serializza_messageSignUp(struct MessageSignUp*);
struct MessageSignUp* deserializza_messageSignUp(char*);


