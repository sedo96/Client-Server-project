#include "MessageVediVincite.h"

char* serializza_MessageVediVincite(struct MessageVediVincite* mvv) {

	char *mvv_buff = malloc(sizeof(struct MessageVediVincite));
	if(!mvv_buff)
		return NULL;
	int i = 1;
	mvv_buff[0] = MESSAGEVEDIVINCITEOPCODE;
	mvv_buff[i] = mvv->indice_ruota;
	i += sizeof(int);
	memcpy(&mvv_buff[i], mvv->numeri, sizeof(int)*MAX_NUMERI);
	i += sizeof(int)*MAX_NUMERI;
	mvv_buff[i] = mvv->tipo_vittoria;
	i += sizeof(char);
	memcpy(&mvv_buff[i], &mvv->importo_vinto, sizeof(float));
	i += sizeof(float);
	memcpy(&mvv_buff[i], &mvv->session_id, SESSION_ID_LENGTH*sizeof(char));
	
	return mvv_buff;
	
}

struct MessageVediVincite* deserializza_MessageVediVincite(char *mvv_buff){
	struct MessageVediVincite *mvv = malloc(sizeof(struct MessageVediVincite));
	if(!mvv)
		return NULL;
	int i = 0;
	mvv->opcode = mvv_buff[i];
	i++;
	mvv->indice_ruota = mvv_buff[i];
	i+= sizeof(int);
	memcpy(mvv->numeri, &mvv_buff[i], sizeof(int)*MAX_NUMERI);
	i+= sizeof(int)*MAX_NUMERI;
	mvv->tipo_vittoria = mvv_buff[i];
	i+=sizeof(char);
	memcpy(&mvv->importo_vinto, &mvv_buff[i], sizeof(float));
	i += sizeof(float);
	memcpy(mvv->session_id, &mvv_buff[i], SESSION_ID_LENGTH*sizeof(char));
	return mvv;
}


struct MessageVediVincite* copiaRiga_MessageVediVincite(char *mvv_buff){

	struct MessageVediVincite *mvv = (struct MessageVediVincite*)malloc(sizeof(struct MessageVediVincite));
	int i = 0;
	memset(mvv->numeri, 0, sizeof(int)*MAX_NUMERI);
	char *temp, *temp1;
	temp = strtok(mvv_buff, " ");
	int indice = ottieni_indice(temp);
	mvv->indice_ruota = indice;
	while((temp1 = strtok(NULL, " "))!= NULL) {
		if(strncmp(temp1, ">>", 2)==0)
			break;
		mvv->numeri[i] = atoi(temp1);
		i++;
	}
	i=0;
	while((temp1 = strtok(NULL, " "))!= NULL) {
		if(strncmp(temp1, "/", 1)==0)
			break;
		if(i==0)
			mvv->tipo_vittoria = ottieni_iniziale(temp1);
		else
			mvv->importo_vinto = atof(temp1); 
		i++;
	}
	return mvv;
	
}

int ottieni_indice(char *nome_ruota){
	if(strcmp(nome_ruota, "Bari")==0)
		return 0;
	if(strcmp(nome_ruota, "Cagliari")==0)
		return 1;
	if(strcmp(nome_ruota, "Firenze")==0)
		return 2;
	if(strcmp(nome_ruota, "Genova")==0)
		return 3;
	if(strcmp(nome_ruota, "Milano")==0)
		return 4;
	if(strcmp(nome_ruota, "Napoli")==0)
		return 5;
	if(strcmp(nome_ruota, "Palermo")==0)
		return 6;
	if(strcmp(nome_ruota, "Roma")==0)
		return 7;
	if(strcmp(nome_ruota, "Torino")==0)
		return 8;
	if(strcmp(nome_ruota, "Venezia")==0)
		return 9;
	if(strcmp(nome_ruota, "Nazionale")==0)
		return 10;
	return -1;
}

char ottieni_iniziale(char *tipo_vincita){
	int len = strlen(tipo_vincita);
	if(strncmp(tipo_vincita, "Estratto", len)==0)
		return 'E';
	if(strncmp(tipo_vincita, "Ambo", len)==0)
		return 'A';
	if(strncmp(tipo_vincita, "Terno", len)==0)
		return 'T';
	if(strncmp(tipo_vincita, "Quaterna", len)==0)
		return 'Q';
	if(strncmp(tipo_vincita, "Cinquina", len)==0)
		return 'C';
return -1;
}

