#include "MessageVediGiocata.h"

char* serializza_MessageVediGiocata(struct MessageVediGiocata* mvg) {

	char *mvg_buff = malloc(sizeof(struct MessageVediGiocata));
	if(!mvg_buff)
		return NULL;
	int i=1;
	mvg_buff[0] = MESSAGEVEDIGIOCATAOPCODE;
	memcpy(&mvg_buff[i], &mvg->ruote, TOT_RUOTE);
	i+= TOT_RUOTE;
	memcpy(&mvg_buff[i], mvg->numeri, TOT_NUMERI*sizeof(int));
	i+= TOT_NUMERI*sizeof(int);
	memcpy(&mvg_buff[i], mvg->importi, TOT_IMPORTI*sizeof(float));

	i += TOT_IMPORTI*sizeof(float);
	
	memcpy(&mvg_buff[i], mvg->session_id, SESSION_ID_LENGTH*sizeof(char));
	
	return mvg_buff;
}

struct MessageVediGiocata* deserializza_MessageVediGiocata(char *mvg_buff){

	struct MessageVediGiocata *mvg = (struct MessageVediGiocata*)malloc(sizeof(struct MessageVediGiocata));
	
	int i = 1;
	memcpy(mvg->ruote, &mvg_buff[i], TOT_RUOTE);
	i+= TOT_RUOTE;
	memcpy(mvg->numeri, &mvg_buff[i], TOT_NUMERI*sizeof(int));
	i+= TOT_NUMERI*sizeof(int);
	memcpy(mvg->importi, &mvg_buff[i], TOT_IMPORTI*sizeof(float));

	i += TOT_IMPORTI*sizeof(float);
	
	memcpy(mvg->session_id, &mvg_buff[i], SESSION_ID_LENGTH*sizeof(char));
	return mvg;
	
}





