#include "function_client.h"

void help(char *cmd) {
	int len = strlen(cmd); //lunghezza del comando che passo alla help
	char *appoggio = malloc(sizeof(char)*len); //variabile dove salvo il comando
	strncpy(appoggio, cmd, len-1);

	if(strcmp(appoggio, "signup")==0) {
			
		printf("2) !signup <username> <password> --> crea un nuovo utente\r\n");
	}
	else if(strcmp(appoggio, "login")==0) {
		printf("3) !login <username> <password> --> autentica un utente\r\n");
		return;
	}
	else if(strcmp(appoggio, "invia_giocata")==0) {
		printf("4) !invia_giocata g --> invia una giocata g al server\r\n");
		return;
	}
	else if(strcmp(appoggio, "vedi_giocate")==0) {
		printf("5) !vedi_giocate tipo --> visualizza le giocate precedenti dove tipo = {0,1} e permette di visualizzare le giocate passate '0' oppure le giocate attive '1' (ancora non estratte)'\r\n");
		return;
	}
	else if(strcmp(appoggio, "vedi_estrazione")==0) {
		printf("6) !vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni sulla ruota specificata\r\n");
		return;
	}
	else if(strcmp(appoggio, "esci")==0){
		printf("7) !esci --> termina il client\r\n");
		return;
	}
	else {
		printf("****************GIOCO DEL LOTTO******************\r\n");
		printf("Sono disponibili i seguenti comandi:\r\n");
		printf("\n");
		printf("1) !help <comando> --> mostra i dettagli di un comando\r\n");
		printf("2) !signup <username> <password> --> crea un nuovo utente\r\n");
		printf("3) !login <username> <password> --> autentica un utente\r\n");
		printf("4) !invia_giocata g --> invia una giocata g al server\r\n");
		printf("5) !vedi_giocate tipo --> visualizza le giocate precedenti dove tipo = {0,1} e permette di visualizzare le giocate passate '0' oppure le giocate attive '1' (ancora non estratte)'\r\n");
		printf("6) !vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni sulla ruota specificata\r\n");
		printf("7) !esci --> termina il client\r\n");
	}
	return;
}

int signup(char *username, char *password) {
	int len = MAXLENGTHUSERNAME + MAXLENGTHPASSWORD + 1; //lunghezza massima data dalla somma massima dei carattere dell'username e della password
	struct MessageSignUp msu; //struttura del messaggio di signup
	char *msg_ser; //buffer per inviare i parametri al server
	strncpy(msu.username, username, MAXLENGTHUSERNAME);
	strncpy(msu.password, password, MAXLENGTHPASSWORD);
	msg_ser = serializza_messageSignUp(&msu);
	ssize_t ret = 0; //variabile di ritorno della recv e della send
	if(!msg_ser)
		return -1;

	ret = send(socket_id, msg_ser, len, 0);
	if (ret <= 0)
		return -1;
	free(msg_ser);
	char *buffer_ritorno = malloc(sizeof(struct MessageSignUp));
	if(!buffer_ritorno)
		return -1;

	ret = recv(socket_id, buffer_ritorno, len, 0);
	if (ret <= 0)
		return -1;
	
	if(buffer_ritorno[0] == MESSAGESIGNUPOPCODE) {
		struct MessageSignUp *msu_signup;
		msu_signup = deserializza_messageSignUp(buffer_ritorno);
		if(strlen(msu_signup->username) == 0) {
			printf("Tentativo non andato a buon fine! Username già in uso, riprovare!\r\n");
			return -1;
		} else {
			printf("Tentativo andato a buon fine! Username e password registrati\n\r");
			return 0;
		}
		free(buffer_ritorno);
		free(msu_signup);
	} else {
		return -1;
	}
}

int login(char* username, char* password) {
	
	int len = MAXLENGTHUSERNAME + MAXLENGTHPASSWORD + 1;	
	struct MessageLogin msu; //struttura del messaggio di login
	char *msg_ser; //buffer per inviare il messaggio al login
	char *login_response_buffer = malloc(sizeof(struct MessageLoginResponse)); //buffer dove salvo i dati che mi passa il server
	struct MessageLoginResponse *mrl; //struttura del messaggio di login che passa il server al client
	strncpy(msu.username, username,MAXLENGTHUSERNAME);
	strncpy(msu.password, password,MAXLENGTHPASSWORD);
	msg_ser = serializza_messageLogin(&msu);

	
	if(!msg_ser)
		return -1;
	
	ssize_t ret = 0;
	
	ret = send(socket_id, msg_ser, len, 0);
	if (ret <= 0)
		return -1;
	free(msg_ser);
	
	ret = recv(socket_id, login_response_buffer, sizeof(struct MessageLoginResponse), 0);
	if (ret <= 0)
		return -1;
	
	mrl = deserializza_messageLoginResponse(login_response_buffer);

	
	if(strcmp(username, username_loggato)==0) {
		printf("Username già loggato\r\n");
		strcpy(username_loggato, username);
		return 0;
	}
	
	if(strcmp(mrl->username, username)!=0) {
		if(mrl->username[0] == 0x00){
			
			uint8_t tent_effettuati = mrl->sessionID[0];
			printf("Tentativi effettuati: %d\r\n", tent_effettuati);
			uint8_t tent_rimasti = 3-tent_effettuati;
			printf("Login fallito. Reinserire le credenziali. Tentantivi rimasti: %d\r\n", tent_rimasti);
			if(tent_rimasti == 0)
				printf("IP bloccato per 30 minuti. Impossibile accedere\r\n");
			return 0;
		}
		return 0;

	} else {
		printf("Login andato a buon fine\r\n");
		strcpy(username_loggato, mrl->username);
		strcpy(session_id, mrl->sessionID);
	}
	free(mrl);
	free(login_response_buffer);
	return 1;
}

void scrivi_giocata() {
	char *simbolo_r; //variabile per prelevare il simbolo -r dopo il comando invia_giocata
	char *ruote_giocate[MAXNUMERI];
	int elenco_numeri[MAXNUMERI]; 
	float elenco_importi[MAXIMPORTI];
	int quanti_giocati = 0;
	int quante_ruote_giocate = 0;
	int quanti_importi = 0;
	int i = 0;
	int j = 0;
	char *appoggio; //variabile d'appoggio per prelevare le ruote giocate
	char *appoggio_num; //variabile d'appoggio per prelevare i numeri giocati
	char *appoggio_num1; //variabile d'appoggio per prelevare gli importi
	struct MessageGiocata schedina;

	simbolo_r = strtok(NULL, " ");
			
	if((strcmp(simbolo_r, "-r"))!= 0) {
		printf("Errore, non è stato inserito il simbolo giusto\r\n");
		perror("Errore");
		return;
	}
	

	while(i < NUMERORUOTE) {
		appoggio = strtok(NULL, " ");
		if(appoggio == NULL) {
			return;
		}

		if(strncmp(appoggio,"-n", 2)==0) {
			break;
		}		
		
		if(strncmp(appoggio, "tutte", 5)==0){
			quante_ruote_giocate = NUMERORUOTE;
			continue;
		}
		ruote_giocate[i] = appoggio;	
		quante_ruote_giocate++;
		
		i++;
	}

		i = 0;

		while(i < MAXNUMERI) {
			appoggio_num = strtok(NULL, " ");

			if(strncmp(appoggio_num,"-i", 2)==0) {
				break;
			}
			
			if(appoggio_num == NULL || atoi(appoggio_num) > 90 || atoi(appoggio_num) <= 0 ) {
				printf("Errore sui numeri o sul comando\r\n");
				return;
			}
			while(j<quanti_giocati){
				if(elenco_numeri[j]==atoi(appoggio_num)){
					printf("Giocati 2 numeri uguali, giocata non valida\r\n");
					return;
				}
				j++;
			}
			elenco_numeri[i] = atoi(appoggio_num);
			quanti_giocati++;
			j = 0;	
			i++;
		}

		i = 0;
		
		while(i < MAXIMPORTI) {
			appoggio_num1 = strtok(NULL, " ");
				
				
			if(appoggio_num1 == NULL) {
				
				if(i > quanti_giocati) {
					printf("Numero di importi maggiori dei numeri giocati\r\n");
					return;
				}
				break;
			}
		
			if(atoi(appoggio_num1) < 0 ) {
				printf("Errore sugli importi o sul comando\r\n");
				return;
			}

			elenco_importi[i] = atoi(appoggio_num1);
			quanti_importi++;
			i++;

		}

		schedina.quanteruote = quante_ruote_giocate;
		schedina.quantigiocati = quanti_giocati;
		schedina.quantiimporti = quanti_importi;

		memset(schedina.ruote, '0', NUMERORUOTE);
		memset(schedina.numeri, 0x00, MAXNUMERI*sizeof(int));
		memset(schedina.importi, 0xFF, MAXIMPORTI*sizeof(float));
		int indice_ruota;
		if(quante_ruote_giocate == NUMERORUOTE){
			while(j<quante_ruote_giocate){
				schedina.ruote[j] = '1';
				j++;
			}
		} else {
			for(i = 0; i < quante_ruote_giocate; i++) {		
				indice_ruota = ottieni_numero_indice(ruote_giocate[i]);
				if(indice_ruota == -1) {
					printf("Ruota non valida\r\n");
					return;
				}	
				schedina.ruote[indice_ruota] = '1';
			}
		}
		for(i = 0; i < quanti_giocati; i++) {
			schedina.numeri[i] = elenco_numeri[i];
		}
			
		for(i = 0; i < quanti_importi; i++) {
			schedina.importi[i] = elenco_importi[i];
		}
		strcpy(schedina.session_id, session_id);
		invia_giocata(schedina);

}

int ottieni_numero_indice(char *nome_ruota){
	if(strncmp(nome_ruota, "Bari", 4)==0)
		return 0;
	if(strncmp(nome_ruota, "Cagliari", 8)==0)
		return 1;
	if(strncmp(nome_ruota, "Firenze", 7)==0)
		return 2;
	if(strncmp(nome_ruota, "Genova", 6)==0)
		return 3;
	if(strncmp(nome_ruota, "Milano", 6)==0)
		return 4;
	if(strncmp(nome_ruota, "Napoli", 6)==0)
		return 5;
	if(strncmp(nome_ruota, "Palermo", 6)==0)
		return 6;
	if(strncmp(nome_ruota, "Roma", 4)==0)
		return 7;
	if(strncmp(nome_ruota, "Torino", 6)==0)
		return 8;
	if(strncmp(nome_ruota, "Venezia", 7)==0)
		return 9;
	if(strncmp(nome_ruota, "Nazionale", 9)==0)
		return 10;
	return -1;
}

int invia_giocata(struct MessageGiocata schedina) {
	
	int len = sizeof(struct MessageGiocata);
	char *msg_ser = malloc(len); //buffer utilizzato per salvare la schedina da inviare al server
	char *giocata_response_buffer = malloc(len); //buffer dove salvo i valori di ritorno dal server

	msg_ser = serializza_messageGiocata(&schedina);
	
	if(!msg_ser)
		return -1;

	int ret = 0;
	
	ret = send(socket_id, msg_ser, len, 0);
	if (ret <= 0)
		return -1;
	
	int i = 0;
	
	while(i<2){ //faccio questo ciclo perchè quando sono riandato a controllarlo ho notato che in tutte le funzioni il server invia una prima volta un solo byte con valore 0, successivamente invia i valori giusti. Non sono riuscito a trovare il problema
		
		ret = recv(socket_id, giocata_response_buffer, len, 0); 
		if (ret <= 0)
			return -1;
		if(ret != sizeof(giocata_response_buffer)){
			i++;
			continue;
		} else 
			break;
	}
	if(giocata_response_buffer[0] == 0x03)
		printf("Giocata effettuata\r\n");
	else if(giocata_response_buffer[0] == -1)
		printf("Giocata non andata a buon fine, username non loggato\r\n");
	else
		printf("Giocata non andata a buon fine, session_ID non corretto\r\n");

	return 0;

}

bool vedi_giocate(char tipo) {
	ssize_t ret = 0;
	
	struct MessageVediGiocata *mvg = malloc(sizeof(struct MessageVediGiocata));
	char *richiedi_tipo_giocata = malloc(sizeof(struct MessageVediGiocata));
	char *giocate_effettuate = malloc(sizeof(struct MessageVediGiocata));
	
	char *elenco_ruote[NUMERORUOTE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};

	int i = 0;
	int righe_stampate = 1;

	//invio al server i l'opcode, il tipo e il session_ID utilizzando la struttura MessageVediGiocata e i campi ruote, numeri e session_id
	mvg->ruote[0] = MESSAGEVEDIGIOCATAOPCODE;
	mvg->numeri[0] = (tipo == '1') ? 1 : 0;
	strcpy(mvg->session_id, session_id);
	
	richiedi_tipo_giocata = serializza_MessageVediGiocata(mvg);

	ret = send(socket_id, richiedi_tipo_giocata, sizeof(struct MessageVediGiocata), 0);
	if (ret <= 0)
		return -1;
	free(richiedi_tipo_giocata);
	
	while(1) {
		ret = recv(socket_id, giocate_effettuate, sizeof(struct MessageVediGiocata), 0);
		
		
		if(ret != sizeof(struct MessageVediGiocata) && ret != 2)  
			continue;
		if(giocate_effettuate[0] == -1){
			printf("Richiesta una giocata, ma l'username non è loggato\r\n");
			free(giocate_effettuate);
			return false;
		}
		if(giocate_effettuate[0] == -2){
			printf("Richiesta una giocata, ma il session_ID non è corretto\r\n");	
			free(giocate_effettuate);
			return false;
		}
		if(ret==2){
			if(righe_stampate == 1)
				printf("Nessuna giocata presente in archivio\r\n");
			break;
		}
		mvg = deserializza_MessageVediGiocata(giocate_effettuate);
		printf("%d) ", righe_stampate);
		for(i=0; i<sizeof(mvg->ruote); i++) {
			if(mvg->ruote[i]=='1')
				printf("%s ", elenco_ruote[i]);
		}
		
		for(i=0; i<sizeof(mvg->numeri)/4; i++) {
			if(mvg->numeri[i]==0)
				break;
			printf("%d ", mvg->numeri[i]);
		}
		printf("* ");
		for(i=0; i<sizeof(mvg->importi)/4; i++) {
			if(mvg->importi[i+1]==0)
				break;
		}
		while(i>=0) {
			if(i==4 && mvg->importi[i]!=0) {
				printf("%f cinquina", mvg->importi[i]);
			} else if(i==3 && mvg->importi[i]!=0) {
				printf("%f quaterna", mvg->importi[i]);
			} else if(i==2 && mvg->importi[i]!=0){
				printf("%f terno", mvg->importi[i]);
			} else if(i==1 && mvg->importi[i]!=0) {
				printf("%f ambo", mvg->importi[i]);
			} else if(i==0 && mvg->importi[i]!=0) {
				printf("%f estratto", mvg->importi[i]);
			}
			if(i!=0 && mvg->importi[i-1]!=0)
				printf(" * ");
			i--;
		}
		
		printf("\r\n");
		righe_stampate++;
	}
	free(giocate_effettuate);
	return true;
}

bool vedi_estrazioni(int n, char *ruota){

	ssize_t ret = 0;
	int i = 0;
	char *elenco_ruote[NUMERORUOTE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};	
	int numero_ruota = ottieni_numero_indice(ruota);
	int len = sizeof(struct MessageVediEstrazione);
	int len1 = sizeof(struct MessageVediEstrazioneResponse);
	char *buff_mve = malloc(len);
	struct MessageVediEstrazione *mve = malloc(len); //struttura utilizzata per salvare il numero delle ultime estrazioni, il numero della ruota e il session_id per inviarla al server
	struct MessageVediEstrazioneResponse *mver;
	char *numeri_estratti = malloc(len1);
	//memset(numeri_estratti, 0xFC, len1);
	mve->ultime_estrazioni = n;
	mve->numero_ruota = numero_ruota;
	strcpy(mve->session_id, session_id);

	buff_mve = serializza_MessageVediEstrazione(mve);

	ret = send(socket_id, buff_mve, len, 0);
	if (ret <= 0)
		return -1;
	free(mve);
	free(buff_mve);
	while(1) {
		int s = 0;
		
		ret = recv(socket_id, numeri_estratti, len1, 0);
		
		if(ret <= 0)
			return false;
		
		mver = deserializza_MessageVediEstrazioneResponse(numeri_estratti);
		
		if(mver->numeri[0] == 0){
			continue;
		}
		if(mver->numeri[0] == -3){
			return false;
		}
		
		if(mver->numeri[0] == -4){
			printf("Numero di ultime estrazioni passato troppo grande\r\n");
			return false;
		}
		

		if(mver->numeri[0] == -1){
			printf("Username non loggato\r\n");
			return false;
		}

		if(mver->numeri[0] == -2){
			printf("Session_ID non corretto\r\n");
			return false;
		}

		if(strncmp(ruota, "Default", strlen(ruota))==0) {
			int tot_numeri = 0;
			for(i=0; i<TOT_RUOTE; i++) {
				printf("%s ", elenco_ruote[i]);
				int s = 0;
				while(s<5) {
					printf("%d ", mver->numeri[tot_numeri]);
					tot_numeri++;
					s++;
				}
				printf("\r\n");
			}
		} else {
			printf("%s", ruota);
			while(s<5) {
				printf("%d ", mver->numeri[s]);
				s++;
			}
			printf("\r\n");
			
		}
		printf("\r\n");
		free(mver);
	}
	free(numeri_estratti);
	return true;
}

bool vedi_vincite(){
	size_t ret;
	char *elenco_ruote[NUMERORUOTE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};	
	int len = sizeof(struct MessageVediVincite);
	int i = 0;
	int numeri_giocati = 0; 
	int riga_stampa = 0; //variabile utilizzata per vedere se la riga stampata è la prima oppure no: nel caso fosse la prima non devo stampare la serie di *
	float vittoria_cinquina = 0;
	float vittoria_quaterna = 0;
	float vittoria_terno = 0;
	float vittoria_ambo = 0;
	float vittoria_estratto = 0;
	char *buffer_invio_opcode = malloc(len);
	char buffer_ricezione[len];
	struct MessageVediVincite *mvv = malloc(len); 
	struct MessageVediVincite *mvv_invio = malloc(len); //struttura utilizzata per inviare al server l'opcode e il session_id
	memset(mvv_invio, 0x00, len);
	mvv_invio->opcode = MESSAGEVEDIVINCITEOPCODE;
	strcpy(mvv_invio->session_id, session_id);  
	
	buffer_invio_opcode = serializza_MessageVediVincite(mvv_invio);
	ret = send(socket_id, buffer_invio_opcode, len, 0);
	if (ret <= 0)
		return -1;
	
	free(buffer_invio_opcode);
	free(mvv_invio);

	while(1){
		ret = recv(socket_id, buffer_ricezione, len, 0);
		//printf("Il valore della ret è: %d\r\n", ret);
		if(ret == 1)
			continue;
		if(ret <= 0)
			return false;
		
		if(buffer_ricezione[0] == -1){
			break;
		}
		if(buffer_ricezione[0] == -3){
			printf("Username non loggato\r\n");			
			return false;
		}
		if(buffer_ricezione[0] == -2){
			printf("Session ID non corretto\r\n");			
			return false;
		}
		if(buffer_ricezione[0] == 'E') {
			if(riga_stampa!=0)
				printf("***********************************************************\r\n");
			printf("%s\r", buffer_ricezione);
		} else {
			
			mvv = deserializza_MessageVediVincite(&buffer_ricezione[0]);
			printf("%s ", elenco_ruote[mvv->indice_ruota]);
			while(i<MAX_NUMERI){
				if(mvv->numeri[i]==0)
					break;
				printf("%d ", mvv->numeri[i]);
				i++;
				numeri_giocati++;
			}
			i=0;
			printf(">> ");
			if(numeri_giocati >=5){
				if(mvv->tipo_vittoria=='C'){
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_cinquina += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='Q'){
					printf("Cinquina 0 ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_quaterna += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='T'){
					printf("Cinquina 0  Quaterna 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_terno += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='A'){
					printf("Cinquina 0  Quaterna 0  Terno 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_ambo += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='E'){
					printf("Cinquina 0  Quaterna 0  Terno 0  Ambo 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_estratto += mvv->importo_vinto;
				}
			} else if(numeri_giocati == 4){
				if(mvv->tipo_vittoria=='Q'){
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_quaterna += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='T'){
					printf("Quaterna 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_terno += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='A'){
					printf("Quaterna 0  Terno 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_ambo += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='E'){
					printf("Quaterna 0  Terno 0  Ambo 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_estratto += mvv->importo_vinto;
				}
			} else if(numeri_giocati == 3){
				if(mvv->tipo_vittoria=='T'){
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_terno += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='A'){
					printf("Terno 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_ambo += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='E'){
					printf("Terno 0  Ambo 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_estratto += mvv->importo_vinto;
				}
			} else if(numeri_giocati == 2){
				if(mvv->tipo_vittoria=='A'){
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_ambo += mvv->importo_vinto;
				} else if(mvv->tipo_vittoria=='E'){
					printf("Ambo 0  ");
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_estratto += mvv->importo_vinto;
				}
			} else if(numeri_giocati == 1){
				if(mvv->tipo_vittoria=='E'){
					printf("%s ", ottieni_tipo_vittoria(mvv->tipo_vittoria));
					printf("%f\r\n", mvv->importo_vinto);
					vittoria_estratto += mvv->importo_vinto;
				}
			} 
		}
		riga_stampa++;
	}
	
	printf("***********************************************************\r\n");
	printf("\r\n");
	printf("Vincita su ESTRATTO: %f\r\n", vittoria_estratto);
	printf("Vincita su AMBO: %f\r\n", vittoria_ambo);
	printf("Vincita su TERNO: %f\r\n", vittoria_terno);
	printf("Vincita su QUATERNA: %f\r\n", vittoria_quaterna);
	printf("Vincita su CINQUINA: %f\r\n", vittoria_cinquina);
	return true;
}

char* ottieni_tipo_vittoria(char tipo_vittoria){
	if(tipo_vittoria == 'E')
		return "Estratto";
	else if(tipo_vittoria == 'A')
		return "Ambo";
	else if(tipo_vittoria == 'T')
		return "Terno";
	else if(tipo_vittoria == 'Q')
		return "Quaterna";
	else if(tipo_vittoria == 'C')
		return "Cinquina";
	return NULL;
}

void esci(){
	
	int ret = close(socket_id);
	if(ret == -1)
		printf("Errore sulla chiusura del socket\r\n");
	else 
		printf("Chiusura avvenuta correttamente\r\n");
}

