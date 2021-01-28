#include "function_client.h"

int main(int argc, char* argv[])
{

	char scelta[256]; //buffer dove salvo il tipo di comando che scrivo su terminale
	char *username; //variabile dove salvo l'username nei comandi di signup
	char *password; //variabile dove salvo la password nei comandi di signup
	



	if(argc<3) {
		printf("Errore nel numero degli argomenti. Specificare IP server e porta server");
		return -1;	
	}

	
	port_number = atoi(argv[2]);
	

	if((port_number > 65535) || (port_number < 2000)) {
		printf("Inserisci un numero di porta compreso tra 2000 e 65535\r\n");
		perror("Errore: \r\n");
		return -1;
	}


	if((socket_id = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Errore nell'inizializzazione del socket!\r\n");
		return -1;		
	}
		
	memset(&add, 0, sizeof(add));
	add.sin_family = AF_INET;
	add.sin_port = htons(port_number);
	inet_pton(AF_INET, (const char*)argv[1], &add.sin_addr);


	
	if((connect(socket_id, (struct sockaddr*)&add, sizeof(add)))<0) {
		printf("Il socket locale non è in grado di inviare una richiesta di connessione ad un socket remoto\r\n");
		perror("Errore\r\n");
		return -1;
	}

	help("default");	
	

	while (1) {
		printf("Inserire un comando\r\n");
		fgets(scelta, 80, stdin);
			
		char *strtok_temp; //variabile d'appoggio che uso per salvare la prima parola del comando inserito su terminale
		strtok_temp = strtok(scelta, " ");	
		char tipo = 0x00; //variabile utilizzata per la funzione vedi_giocate, tipo 0 per le giocate estratte, tipo 1 per le giocate attive
		int n; //usata per la funzione vedi_estrazione, numero di estrazioni a partire dall'ultima che voglio vedere
		int i = 0; //variabile usata per i cicli
		char *elenco_ruote[NUMERORUOTE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};

		if((strcmp(scelta, "help")==0)) {
			strtok_temp = strtok(NULL, "");
			help(strtok_temp);

		} else if ((strcmp(scelta, "signup"))==0)	{
			username = strtok(NULL, " ");
			password = strtok(NULL, " ");
				
			//controllare che lo username non sia troppo lungo
			signup(username, password);
		
		} else if((strcmp(scelta, "login"))==0) {
			
			username = strtok(NULL, " ");
			password = strtok(NULL, " ");

			//controllare che lo username non sia troppo lungo

			login(username, password);

		} else if((strcmp(scelta, "invia_giocata")==0)) {
			
			scrivi_giocata();

		} else if((strcmp(scelta, "vedi_giocate")==0)) {
			strtok_temp = strtok(NULL, " ");

			if(strncmp(strtok_temp,"1",1)==0) {
				tipo = '1';
			} else if(strncmp(strtok_temp,"0", 1)==0) {
				tipo = '0';
			} else {
				printf("Tipo giocata non valido\r\n");
				return -1;
			}

			vedi_giocate(tipo);
					
		} else if(strcmp(scelta, "vedi_estrazione")==0) {
			strtok_temp = strtok(NULL, " ");
			n = atoi(strtok_temp);
			if(n==0) {
				printf("Nessun numero specificato\r\n");
				return -1;
			}
			char *strtok_temp1;
			strtok_temp1 = strtok(NULL, " ");
			if(strtok_temp1 == NULL) {
				strtok_temp1 = "Default";
			}
			while(i<TOT_RUOTE) {
				if(strncmp(strtok_temp1,elenco_ruote[i], strlen(elenco_ruote[i]))==0)
					break;
				i++;
			}
			if(i==11 && strncmp(strtok_temp1, "Default", 7)!=0) {
				printf("Ruota non valida\r\n");
				return -1;
			}
			vedi_estrazioni(n, strtok_temp1);
		} else if((strncmp(scelta, "vedi_vincite", strlen(scelta)-1)==0)){
			vedi_vincite();
		} else if((strncmp(scelta, "esci", strlen(scelta)-1)==0)){
			esci();
			return 0;
		}
	
	}
}


