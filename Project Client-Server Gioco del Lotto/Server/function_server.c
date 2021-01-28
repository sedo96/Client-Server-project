#include "function_server.h"

void* client_handler(struct thread_arg *ta) {

	
	int new_client_socket_id = ta->ns;
	int len = 256;
	char buffer[len];
	ssize_t ret;
	bool boolret;
	char *session_id; //variabile dove salvo il session_id del client
	char username_loggato[17]; //variabile dove salvo l'username che sta dialogando con il server
	memset(username_loggato, 0x00, 17);
	char *usr = username_loggato;
	int contatore_tentativi_login = estrai_tentativi_ip(ta->addr_IP);

	
	struct MessageLoginResponse *mlr_ip_bloccato = malloc(sizeof(struct MessageLoginResponse));

	
	while(1) {
		ret = recv(new_client_socket_id, buffer, len, 0);
		if(ret <= 0){
			ret = close(new_client_socket_id);
			if(ret == -1)
				printf("Errore nella chiusura del socket\r\n");
			else
				printf("Connessione con %s chiusa correttamente\r\n", username_loggato);
			pthread_exit(NULL);
			free(session_id);
			return NULL;
		}
			
		switch(buffer[0]) {
			case MESSAGESIGNUPOPCODE: 
				signup_handler(&buffer[0], new_client_socket_id);
				break;

			case MESSAGELOGINOPCODE:
				
				if(contatore_tentativi_login == 3 && user_isBlocked(ta->addr_IP)) {
					
					ret = invia_errori_client(new_client_socket_id, mlr_ip_bloccato, contatore_tentativi_login);
					if(ret<=0)
						return NULL;
					break;
				} else {
					
					boolret = login_handler(&buffer[0], new_client_socket_id, &usr, &session_id);
				}
				if(!boolret){
					
					contatore_tentativi_login = aggiorna_file_IP_list(contatore_tentativi_login, ta->addr_IP);
					
					ret = invia_errori_client(new_client_socket_id, mlr_ip_bloccato, contatore_tentativi_login); //mandare messaggio aggiornandolo di quanti tentativi ha rimasti
					if(ret<=0)
						return NULL;
					
					if(contatore_tentativi_login == 3){
						
						ret = invia_errori_client(new_client_socket_id, &*mlr_ip_bloccato, contatore_tentativi_login); //l'ip si è bloccato
						if(ret<=0)
							return NULL;
					}
				}
				break;

			case MESSAGEGIOCATAOPCODE:
				boolret = invia_giocata_handler(&buffer[0], new_client_socket_id, username_loggato, &session_id);
				if(!boolret)
					return NULL;
				break;

			case MESSAGEVEDIGIOCATAOPCODE:
				boolret = vedi_giocate_handler(&buffer[0], new_client_socket_id, username_loggato, &session_id);
				if(!boolret)
					return NULL;
				break;
			case MESSAGEVEDIESTRAZIONEOPCODE:
				boolret = vedi_estrazione_handler(&buffer[0], new_client_socket_id, username_loggato, &session_id);
				if(!boolret)
					return NULL;
				break;
			case MESSAGEVEDIVINCITEOPCODE:
				boolret = vedi_vincite_handler(&buffer[0], new_client_socket_id, username_loggato, &session_id);
				if(!boolret)
					return NULL;
				break;
		}
		
	}
	return NULL;
}

int invia_errori_client(int socket, struct MessageLoginResponse *mlr, int numero_tentativi){
	int ret = 0;
	char *buffer = malloc(sizeof(struct MessageLoginResponse));
	memset(mlr->username, 0x00, 17);
	memset(mlr->sessionID, 0x00, 10);
	mlr->sessionID[0] = (uint8_t)numero_tentativi; //salvo il numero dei tentativi da passare al client nel primo char del session_ID della struttura
	
	buffer = serializza_messageLoginResponse(mlr);
	
	ret = send(socket, buffer, sizeof(struct MessageLoginResponse), 0);
	if(ret <= 0)
		return -1;
	free(buffer);
	return ret;
}

bool user_isBlocked(unsigned long addr_IP){
	char appoggio[256];
	unsigned long ip_file;
	time_t t = time(NULL);
	struct tm *ti;
	struct tm *p = localtime(&t);
	double secondi_differenza;
	FILE *indirizzi_ip = fopen("IP_List", "r");
	FILE *indirizzi_ip_temp = fopen("IP_List_temp", "w");
	if(!indirizzi_ip)
		return -1;
	while(fgets(appoggio, 256, indirizzi_ip) != NULL) {
		char *temp = strtok(appoggio, " ");
		ip_file = atol(temp);

		if(ip_file == addr_IP){
			char *temp1 = strtok(NULL, " ");
			temp1 = strtok(NULL, " ");
		
			ti = deserializza_tm(temp1);
			
			secondi_differenza = labs(difftime(mktime(ti), mktime(p)));
			//se il tempo passato tra il momento del blocco e il momento del nuovo login è > di 30 minuti, 1800 secondi, sblocco l'IP cancellando la riga del file IP_List
			if(secondi_differenza > 1800){ 
				while(fgets(appoggio, 256, indirizzi_ip) != NULL) {
					char *temp = strtok(appoggio, " ");
					ip_file = atol(temp);
					if(ip_file == addr_IP){
						continue;
					} else {
						fprintf(indirizzi_ip_temp, "%s", appoggio);
					}
					return false;
				}
			} else {
				fclose(indirizzi_ip);
				return true;
			}
		}
	}
	fclose(indirizzi_ip_temp);
	fclose(indirizzi_ip);
	remove("IP_List");
	rename("IP_List_temp", "IP_List");
	return true;
}

int estrai_tentativi_ip(unsigned long addr_IP){
	char appoggio[256];
	unsigned long ip_file;
	int numero_tentativi = 0;
	FILE *indirizzi_ip;
	indirizzi_ip = fopen("IP_List", "a+");
	if(!indirizzi_ip)
		return -1;
	while(fgets(appoggio, 256, indirizzi_ip) != NULL) {
		char *temp = strtok(appoggio, " ");
		
		ip_file = atol(temp);

		if(ip_file == addr_IP){
			numero_tentativi = atoi(strtok(NULL, " "));
			fclose(indirizzi_ip);
			return numero_tentativi;
		}
	}
	fprintf(indirizzi_ip, "%lu %d ", addr_IP, numero_tentativi);
	fclose(indirizzi_ip);
	return numero_tentativi;
}

char* serializza_tm(struct tm *t){
	char *buffer = malloc(128);
	strftime(buffer, 128, "%x-%T", t);

	return buffer;
}

struct tm* deserializza_tm(char *buffer){
	struct tm *t = malloc(sizeof(struct tm));
	strptime(buffer, "%x-%T", t);
	
	return t;
}

int aggiorna_file_IP_list(int numero_tentativi, unsigned long addr_IP){

	unsigned long ip_file;
	char appoggio[256];
	time_t t;
	time(&t);
  	char *buffer;
	FILE *indirizzi_ip = fopen("IP_List", "r");
	FILE *indirizzi_ip_temp = fopen("IP_List_temp", "w");
	if(!indirizzi_ip || !indirizzi_ip_temp){
		printf("Impossibile aprire IP_List\r\n");
		return -1;
	}
	
	numero_tentativi++;

	while(fgets(appoggio, 256, indirizzi_ip) != NULL) {
		char *temp = strtok(appoggio, " ");
		ip_file = atol(temp);
		if(ip_file == addr_IP){
			if(numero_tentativi == 3){
				struct tm *ti = localtime(&t);
				
				buffer = serializza_tm(ti);
				
				fprintf(indirizzi_ip_temp, "%lu %d %s", addr_IP, numero_tentativi, buffer);
		
			} else if(numero_tentativi < 3){
				fprintf(indirizzi_ip_temp, "%lu %d ", addr_IP, numero_tentativi);
			}
		} else {
			fprintf(indirizzi_ip_temp, "%s", appoggio);
		}
	}
	fclose(indirizzi_ip_temp);
	fclose(indirizzi_ip);
	remove("IP_List");
	rename("IP_List_temp", "IP_List");
	return numero_tentativi;
}



bool signup_handler(char *buffer, int socket) {
	struct MessageSignUp *msu;
	char *msg_vuoto_ser;
	ssize_t ret;
	char username_app[MAXLENGTHUSERNAME + 16];
	strcpy(username_app, "./UserFiles/\0");
	msu = deserializza_messageSignUp(buffer);
	if(!msu)
		return false;
	strcat(username_app, msu->username);
	strcat(username_app, ".txt");

	FILE *f;
	
	f = fopen(username_app, "r");
	if(f==NULL) { //se l'username non esiste, creo il file e ci scrivo username e password
	
		FILE *temp;
		temp = fopen(username_app, "a");
		fprintf(temp,"Username:%s\r\n", msu->username);
		fprintf(temp,"Password:%s\r\n", msu->password);
		fclose(temp);
		free(msu);
		printf("Username e password registrati\r\n");
		
		ret = send(socket, buffer, MAXLENGTHUSERNAME+MAXLENGTHPASSWORD+1, 0);
		if(ret != 0)
			return false;
	} else {
		printf("Username già in uso! Provare con un altro username\r\n");	
		struct MessageSignUp msu_vuoto;
		memset(msu_vuoto.username, 0x00, MAXLENGTHUSERNAME);
		memset(msu_vuoto.password, 0x00, MAXLENGTHPASSWORD);
		msg_vuoto_ser = serializza_messageSignUp(&msu_vuoto);
		ret = send(socket, msg_vuoto_ser, MESSAGESIGNUPLENGTH, 0);
	}
	return true;
}

bool login_handler(char *buffer, int socket, char **username_loggato, char **session_id){

	struct MessageLogin *mlg;
	
	char username_app[MAXLENGTHUSERNAME + 16];
	char *mlg_vuoto_ser;
	ssize_t ret;
	

	char buffer_file_app[32];
	char password_app[MAXLENGTHPASSWORD];
	strcpy(username_app, "./UserFiles/\0");
	mlg = deserializza_messageLogin(buffer);
	if(!mlg)
		return false;
	strcat(username_app, mlg->username);
	strcat(username_app, ".txt");

	

	FILE *f;
	f = fopen(username_app, "r");

	
	if(f==NULL) {
		printf("Username non registrato\r\n");
		return false;
	} 
	
	do { //questo do mi serve per prelevare la password e controllare che sia uguale a quella inserita nel login
		fgets(buffer_file_app, 32, f);
	}while(strncmp(buffer_file_app, "Password", 8) != 0);

	strncpy(password_app, &buffer_file_app[9], MAXLENGTHPASSWORD);

	if(strcmp(password_app, mlg->password)!=0) {
		printf("Password non valida\r\n");
		return false; //password non valida, il client si è bruciato un tentativo
	} 
		
	if(strcmp(mlg->username, *username_loggato)==0) {
		printf("Utente già loggato\r\n");
		struct MessageLoginResponse mlg_vuoto;
		memset(mlg_vuoto.username, '9', MAXLENGTHUSERNAME);
		memset(mlg_vuoto.sessionID, 0x00, SESSIONIDLENGTH);
		mlg_vuoto_ser = serializza_messageLoginResponse(&mlg_vuoto);
		if(!mlg_vuoto_ser)
			return false;
		ret = send(socket, mlg_vuoto_ser, MESSAGELOGINRESPONSELENGTH, 0);
		if(ret <= 0)
			return false;
		
		free(mlg_vuoto_ser);
		return true;
	

	} else {
		strcpy(*username_loggato, mlg->username);
		printf("[%s]Credenziali valide\r\n", *username_loggato);
		*session_id = genera_session_id(10);
		f = fopen(username_app, "a");
		fprintf(f,"Session_ID:%s\r\n", *session_id);
		fclose(f);

		struct MessageLoginResponse mlgr;
		strncpy(mlgr.username, *username_loggato, MAXLENGTHUSERNAME);
		strncpy(mlgr.sessionID, *session_id, SESSIONIDLENGTH);
		buffer = serializza_messageLoginResponse(&mlgr);
		ret = send(socket, buffer, MAXLENGTHUSERNAME+SESSIONIDLENGTH+1, 0);
		if(ret <= 0)
			return false;
		free(buffer);
	}	
	

	return true;
}

char* genera_session_id(int len) {
	char *session_id = malloc(len+1);
	 
	if(!session_id)
		return NULL;
	const char caratteri_alphanum[] = 
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	int i;
	
	srand((unsigned)time(NULL)); //inizializzazione algoritmo di numeri casuali
	for(i = 0; i < len; i++) {
		session_id[i] = caratteri_alphanum[rand()%(sizeof(caratteri_alphanum)-1)];
	}
	session_id[len] = '\0';
	return session_id;
}

bool invia_giocata_handler(char *buffer, int socket,char *username_loggato, char **session_id){
	
	int len = NUMERORUOTE+MAXNUMERI+MAXIMPORTI + SESSION_ID_LENGTH + 4;
	struct MessageGiocata *mlg;
	char username_app[MAXLENGTHUSERNAME + 16];
	size_t ret;

	if(!username_loggato[0]) {
		buffer[0] = -1;
		printf("Giocata effettuata con un username non loggato\r\n");
		ret = send(socket, buffer, len, 0);
		if(ret<=0)
			return false;	
		return true;
	}
	
	strcpy(username_app, "./UserFiles/\0");

	mlg = deserializza_messageGiocata(buffer);
	
	if(!mlg)
		return false;
	if(memcmp(*session_id, mlg->session_id, 10)){
		buffer[0] = -2;
		printf("Session_ID non corretto\r\n");
		ret = send(socket, buffer, len, 0);
		if(ret<=0)
			return false;	
		return true;
	}
	strcat(username_app, username_loggato);
	strcat(username_app, ".txt");

	FILE *f;
	f = fopen(username_app, "a");
	fprintf(f, "Giocata: ");
	inserisci_giocata(f, mlg);

	FILE *t;
	t = fopen("Giocate_attive.txt", "a");
	fprintf(t, "%s ", username_loggato);
	inserisci_giocata(t, mlg);
	
	printf("[%s]Giocata effettuata correttamente\r\n", username_loggato);
	printf("%d\r\n", buffer[0]);
	ret = send(socket, buffer, len, 0);
	if(ret <= 0)
		return false;
	return true;
}

void inserisci_giocata(FILE *f, struct MessageGiocata *mlg) {
	
	int i;
	char *elenco_ruote[NUMERORUOTE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};
	for(i = 0; i < NUMERORUOTE; i++) {
		if(mlg->ruote[i] == '1') {
			fprintf(f, "%s ", elenco_ruote[i]);
		}
	}
	fprintf(f, "- ");
	
	for(i = 0; i < mlg->quantigiocati; i++) {
		fprintf(f, "%d ", mlg->numeri[i]);
	}

	fprintf(f, "- ");
	for(i = 0; i < mlg->quantiimporti; i++) {
		fprintf(f, "%f ", mlg->importi[i]);
	}

	fprintf(f, "/\r\n");
	fclose(f);
	return;
} 

bool vedi_giocate_handler(char *buffer, int socket, char *username_loggato, char **session_id){

	int ret;

	char *mvg_buff = NULL;
	char mvg_endbuff[2];

	memset(mvg_endbuff, 0x00, 2);

	struct MessageVediGiocata mvg;
	struct MessageVediGiocata *mvg_client = malloc(sizeof(struct MessageVediGiocata));
	FILE *f;
	int i = 0;
	char appoggio[256];

	mvg_client = deserializza_MessageVediGiocata(buffer);
	if(!username_loggato[0]) {
		buffer[0] = -1;
		printf("Username non loggato\r\n");
		ret = send(socket, buffer, sizeof(struct MessageVediGiocata), 0);
		if(ret<=0)
			return false;
		return true;
	}
	
	if(memcmp(*session_id, mvg_client->session_id, 10)){
		buffer[0] = -2;
		printf("Session_ID non corretto\r\n");
		ret = send(socket, buffer, sizeof(struct MessageVediGiocata), 0);
		if(ret<=0)
			return false;
		return true;
	}
	
	
	if(mvg_client->numeri[0]==0) {
		f = fopen("Giocate_estratte.txt", "r");
	} else { 
		f = fopen("Giocate_attive.txt", "r");
	}
	if(f==NULL)
		return false;
		
	
	free(mvg_client);
	

	while(fgets(appoggio, 256, f) != NULL) {
		
		char *prova;
		prova = strtok(appoggio, " ");

		if(strcmp(prova, username_loggato) == 0) {
			char *temp;

			for(i=0; i<TOT_RUOTE; i++)
				mvg.ruote[i] = '0';
			while((temp = strtok(NULL, " "))) {		
				if(strncmp(temp,"-",1)==0){
					break;
				}
				
				int indice = ottieni_numero_indice(temp);
				mvg.ruote[indice] = '1';
			}
			i=0;
			memset(mvg.numeri, 0x00, TOT_NUMERI*sizeof(int));
			while((temp = strtok(NULL, " "))) {
				if(strncmp(temp,"-",1)==0){
					break;
				}
				mvg.numeri[i] = atoi(temp);
				i++;
			}
			i=0;
			memset(mvg.importi, 0x00, TOT_IMPORTI*sizeof(float));
			while((temp = strtok(NULL, " "))) {
				if(strncmp(temp,"/",1)==0){
					break;
				}
				mvg.importi[i] = atof(temp);
				i++;
			}
			mvg_buff = serializza_MessageVediGiocata(&mvg);
			
			ret = send(socket, mvg_buff, sizeof(struct MessageVediGiocata), 0);
			free(mvg_buff);
			if(ret!=sizeof(struct MessageVediGiocata)) {
				printf("Numero di byte inviati errati\r\n");
				return false;
			}
		}
	}
	fclose(f);
	
	ret = send(socket, mvg_endbuff, 2, 0);

	
	return true;
}

int ottieni_numero_indice(char *nome_ruota){
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

bool vedi_estrazione_handler(char *buffer, int socket, char *username_loggato, char **session_id){
	
	
	ssize_t ret = 0;
	char *elenco_ruote[NUMERORUOTE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};
	int len = sizeof(struct MessageVediEstrazioneResponse);
	char *numeri_estratti; //buffer che invio al client con i numeri
	char *verifica_parametri; //buffer che invio al client in caso di errore, username_loggato o session_ID
	char *ruota = malloc(sizeof(char)*11);
	struct MessageVediEstrazione *mve;
	
	int j = 0;
	FILE *f;
	int n = 0; //numero totale righe nel file Estrazioni.txt

	struct MessageVediEstrazioneResponse *mver = malloc(sizeof(struct MessageVediEstrazioneResponse));
	struct MessageVediEstrazioneResponse *mver_vuoto = malloc(sizeof(struct MessageVediEstrazioneResponse));
	memset(mver->numeri, 0x00, TOTALE_NUMERI);
	memset(mver_vuoto->numeri, 0x00, TOTALE_NUMERI);

	mve = deserializza_MessageVediEstrazione(buffer);

	verifica_parametri = serializza_MessageVediEstrazioneResponse(mver_vuoto);
	
	if(!username_loggato[0]){
		printf("Username non loggato\r\n");
		verifica_parametri[0] = -1;
		ret = send(socket, verifica_parametri, sizeof(struct MessageVediEstrazioneResponse), 0);
		if(ret<=0)
			return false;
	
		free(verifica_parametri);
		free(mver_vuoto);
		return true;
	}

	if(memcmp(*session_id, mve->session_id, 10)){
		printf("Session_ID non corretto\r\n");
		verifica_parametri[0] = -2;
		ret = send(socket, verifica_parametri, sizeof(struct MessageVediEstrazioneResponse), 0);
		if(ret<=0)
			return false;
		free(verifica_parametri);
		free(mver_vuoto);
		return true;
	}
	
	if(mve->numero_ruota!=-1) {
		strcpy(ruota,elenco_ruote[mve->numero_ruota]);
	}

	f = fopen("Estrazioni.txt", "r"); //apro il file dove salvo le estrazioni

	if(f==NULL)
		return false;
	
	char appoggio[256];

	while(fgets(appoggio, 256, f) != NULL) {
			n++;
	}

	if(mve->ultime_estrazioni*12 > n){
		verifica_parametri[0] = -4;
		ret = send(socket, verifica_parametri, sizeof(struct MessageVediEstrazioneResponse), 0);
		if(ret<=0)
			return false;
		free(verifica_parametri);
		free(mver_vuoto);
		printf("Numero di ultime estrazioni passato troppo grande\r\n");
		return true;
	}

	rewind(f);
	int i = 0;
	while(fgets(appoggio, 256, f) != NULL) { //sposto il cursore del file alla riga desiderata
		if(n-(mve->ultime_estrazioni*12)<0) {
			
			break;
		}
		i++;
		if(n-(mve->ultime_estrazioni*12)==i) {
			
			break;
			
		}
	}
	
	
	while(j < (n-i+1)/12) {
		
		int t = 1;
		int quanti_numeri_tot = 0;
		fgets(appoggio, 256, f);
		while(t<=11) {
			
			fgets(appoggio, 256, f);
			char *temp = strtok(appoggio, " ");
			
			if(mve->numero_ruota ==-1) {
				int s = 0;
				while(s<5) {
					char *numero = strtok(NULL, " ");
					char number = atoi(numero);
					mver->numeri[quanti_numeri_tot] = number;
					
					quanti_numeri_tot++;
					s++;	
				}

			} else if(strncmp(temp, ruota, strlen(ruota))==0){
				int s = 0;
				while(s<5) {
					char *numero = strtok(NULL, " ");
					char number = atoi(numero);
					mver->numeri[s] = number;
					s++;	
				}
				
				
			} 
			t++;
		}
		
		numeri_estratti = serializza_MessageVediEstrazioneResponse(mver);
		ret = send(socket, numeri_estratti, len, 0);
		if(ret<=0)
			return false;
		free(numeri_estratti);
		j++;
	}

	memset(verifica_parametri, 0xFD, TOTALE_NUMERI);
	ret = send(socket, verifica_parametri, len, 0);
	if(ret<=0)
		return false;
	free(verifica_parametri);
	free(ruota);
	free(mve);
	free(mver);
	fclose(f);
	
	return true;
}

bool vedi_vincite_handler(char *buffer, int socket, char *username_loggato, char **session_id){
	ssize_t ret;
	int len = sizeof(struct MessageVediVincite);

	char app_righe_file[256]; //buffer di appoggio dove inserisco le righe prelevate dal file
	struct MessageVediVincite *mvv_client;
	struct MessageVediVincite *mvv;

	mvv_client = deserializza_MessageVediVincite(buffer);
	if(!username_loggato[0]){
		printf("Username non loggato\r\n");
		buffer[0] = -3;
		ret = send(socket, buffer, len, 0);
		if(ret<=0)
			return false;
		free(mvv_client);
		return true;
	}

	if(memcmp(*session_id, mvv_client->session_id, 10)){
		printf("Session_ID non corretto\r\n");
		buffer[0] = -2;
		ret = send(socket, buffer, len, 0);
		if(ret<=0)
			return false;
		free(mvv_client);
		return true;
	}
		
	char *buff_mvv;
	FILE *vincite = fopen("Vincite.txt", "r");
	if(vincite==NULL){
		printf("Errore nell'apertura del file\r\n");
		return false;
	}
	
	while(1) {
		
		fgets(app_righe_file, 256, vincite);
		if(strncmp(app_righe_file, username_loggato, strlen(username_loggato))==0){
			fgets(app_righe_file, 256, vincite); //estrazione
			ret = send(socket, app_righe_file, len, 0);
			if(ret<=0)
				return false;
			while(fgets(app_righe_file, 256, vincite) != NULL) {
				if(strncmp(app_righe_file, "-", 1)==0)
					break;
				
				mvv = copiaRiga_MessageVediVincite(app_righe_file);
				
				buff_mvv = serializza_MessageVediVincite(mvv);

				
				ret = send(socket, buff_mvv, len, 0);
				if(ret<=0)
					return false;
				free(mvv);
				free(buff_mvv);

			}
		
		} else {
			while(fgets(app_righe_file, 256, vincite) != NULL) {
				if(strncmp(app_righe_file, "-", 1)==0)
					break;
			}
		
		}
		if(feof(vincite))
			break;
		
	}
	
	memset(app_righe_file, -1, len);
	ret = send(socket, app_righe_file, len, 0);
	if(ret<=0)
		return false;

	printf("Prova uscita\r\n");
	fclose(vincite);
	
	return true;
}

