#include "gestore_estrazioni.h"

int ultima_estrazione[TOT_RUOTE][NUMERI_PER_RUOTA];
char *elenco_ruote[] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};
char username_giocata[11];
	

void* thread_estrazioni(int *periodo) {

  	time_t t = time(NULL);
  	struct tm tm = *localtime(&t);
	int p = *periodo;

	while(1) {
		FILE *f = fopen("Estrazioni.txt", "a");
		sleep(p);
		printf("Estrazione effettuata\r\n");
		int i, j, t;
		int casuale[4];
		t = 0;
		fprintf(f,"Estrazione del %d-%02d-%02d  ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
		fprintf(f,"Orario: %d:",tm.tm_hour);
		if(tm.tm_min<10)
			fprintf(f,"0%d\r\n",tm.tm_min);
		else 
			fprintf(f,"%d\r\n",tm.tm_min);
		srand(time(0));

		for(i=0; i < 11; i++) {
			for(j=0; j<5; j++) {
				if(j==0) {
					fprintf(f,"%s ", elenco_ruote[i]);
				} 
				casuale[j] = rand()%90+1;
				t = 0;
				bool trovato = false;
				while(t<j) {
					if(casuale[t] == casuale[j]) {
						j--;
						trovato = true;
						break;
					}
					t++;
				}
				if(trovato == true)
					continue;
				fprintf(f,"%d ", casuale[j]);
				ultima_estrazione[i][j] = casuale[j];
			}
			fprintf(f,"\r\n");
		}
		fclose(f);
		char riga_giocate_attive[256];
		
		int quante_giocate_attive = 0;
		
		FILE *p = fopen("Giocate_attive.txt", "r");
		FILE *h = fopen("Giocate_estratte.txt", "a");
		
		if(p) {
			if(tm.tm_min<10)
				fprintf(h,"%d-%d-%d  %d 0%d\r\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
			else 
				fprintf(h,"%d-%d-%d  %d %d\r\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
			while(fgets(riga_giocate_attive, 256, p) != NULL) {
				quante_giocate_attive++;
				fputs(riga_giocate_attive, h);
				calcola_vincite(riga_giocate_attive);
			}
			if(quante_giocate_attive == 0)
				printf("Nessuna giocata attiva\r\n");
		} else {
			printf("File non presente, nessuna giocata attiva\r\n");
			return NULL;
		}
		FILE *s = fopen("Giocate_attive.txt", "w");
		fclose(p);
		fclose(h);
		fclose(s);
	}
	
}

int trova_numero_indice(char *nome_ruota){
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

struct Schedina* deserializza_schedina(char *riga_g) {
	struct Schedina* sch = malloc(sizeof(struct Schedina));
	if(!sch)
		return NULL;
	
	char *temp, *temp1;
	int contatore = 0;
	temp = strtok(riga_g, " ");
	strcpy(username_giocata, temp);
	while((temp1 = strtok(NULL, " "))) {
		if(strcmp(temp1, "-") == 0) 
			break;

		int indice = trova_numero_indice(temp1);
		if(indice == -1)
			return NULL;
		sch->ruote[indice] = '1';
	}
	while((temp1 = strtok(NULL, " "))) {
		if(strcmp(temp1, "-") == 0) 
			break;
		
		sch->numeri[contatore] = atoi(temp1);
		contatore++;
	}
	contatore = 0;
	while((temp1 = strtok(NULL, " "))) {
		
		if(strcmp(temp1, "/") == 0) 
			break;
		
		sch->importi[contatore] = atof(temp1);
		contatore++;
	}
	return sch;
}

void calcola_vincite(char *riga_giocata) {
	struct Schedina* sch = malloc(sizeof(struct Schedina));
	memset(sch->ruote, '0', TOT_RUOTE);
	memset(sch->numeri, 0, NUMERI_GIOCATI*sizeof(int));
	memset(sch->importi, 0, MAX_IMPORTI*sizeof(float));
	sch = deserializza_schedina(riga_giocata);
	int quante_ruote_giocate = 0;
	int quanti_numeri_giocati = 0;
	int quanti_importi_giocati = 0;

	float vincita_estratto = 11.23;
	int vincita_ambo = 250;
	int vincita_terno = 4500;
	int vincita_quaterna = 120000;
	int vincita_cinquina = 6000000;
	int i = 0;

	time_t t = time(NULL);
  	struct tm tm = *localtime(&t);

	for(i=0; i<TOT_RUOTE; i++) {
		if(sch->ruote[i] == '1') {
			quante_ruote_giocate++;
		}
	}
	for(i=0; i<NUMERI_GIOCATI; i++) {
		
		if(sch->numeri[i] == 0 || sch->numeri[i] == -1) {
			break;
		}
		quanti_numeri_giocati++;
	}
	for(i=0; i<MAX_IMPORTI; i++) {
		if(sch->importi[i] == 0 && i!=0) {
			break;
		}
		quanti_importi_giocati++;
	}

	//verifico quanti numeri ho indovinato su ogni ruota;
	int numeri_indovinati_ruota[quanti_numeri_giocati];
	memset(numeri_indovinati_ruota, 0, sizeof(int)*quanti_numeri_giocati);
	int j=0;
	float vincita = 0; //variabile che mi tiene conto di quanto ho vinto con le ultime giocate nell'ultima estrazione
	int ambi = ambi_generabili(quanti_numeri_giocati); //ambi generabili in base al numero di numeri giocati
	int terni = terni_generabili(quanti_numeri_giocati); //terni generabili in base al numero di numeri giocati
	int quaterne = quaterne_generabili(quanti_numeri_giocati); //quaterne generabili in base al numero di numeri giocati
	int cinquine = cinquine_generabili(quanti_numeri_giocati); //cinquine generabili in base al numero di numeri giocati

	FILE *vincite = fopen("Vincite.txt", "a"); //apro il file vincite
	int prima_volta = 0; //se vincessi due volte su due ruote diverse, scrivo l'orario e la data dell'estrazione una sola volta
	
	int numeri_indovinati = 0;
	for(i=0; i<TOT_RUOTE; i++) {
		if(sch->ruote[i] == '1') {
			
			
			int t;
			for(j=0; j<quanti_numeri_giocati; j++) {
				
				t = 0;
				while(t<5) {
					
					if(ultima_estrazione[i][t] == sch->numeri[j]) {
						numeri_indovinati_ruota[numeri_indovinati] = sch->numeri[j];
						numeri_indovinati++;
					}
					t++;
				}
				
			}
			t = 0;
			if(numeri_indovinati != 0) { //preparo il file con le vincite con la ruota vincente e tutti i numeri giocati
							
				if(prima_volta==0) {
					prima_volta++;
					fprintf(vincite, "%s\r\n", username_giocata);	
					fprintf(vincite,"Estrazione del %d-%02d-%02d  ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
					fprintf(vincite,"Orario: %d:",tm.tm_hour);
					if(tm.tm_min<10)
						fprintf(vincite,"0%d\r\n",tm.tm_min);
					else 
						fprintf(vincite,"%d\r\n",tm.tm_min);
				}
				
				fprintf(vincite, "%s ", elenco_ruote[i]);
				while(t<quanti_numeri_giocati) {
					fprintf(vincite,"%d ",sch->numeri[t]);
					t++;
				}
				fprintf(vincite, ">>");
			}
			
			if(numeri_indovinati == 1) {
				vincita = (sch->importi[0]*vincita_estratto)/(quante_ruote_giocate*quanti_numeri_giocati);
				fprintf(vincite," Estratto %f /\r\n", vincita);
				//fprintf(vincite, "-\r\n");
			} else if(numeri_indovinati == 2) {
				vincita = (sch->importi[1]*vincita_ambo)/(quante_ruote_giocate*ambi);
				fprintf(vincite," Ambo %f /\r\n", vincita);
				//fprintf(vincite, "-\r\n");
			} else if(numeri_indovinati == 3){
				vincita = (((sch->importi[2]*vincita_terno)/(quante_ruote_giocate*terni))+((sch->importi[1]*vincita_ambo*ambi_generabili(3))/(quante_ruote_giocate*ambi))); //la vincita per un terno si trova facendo: moltiplicando l'importo giocato sul terno per la vincita del terno diviso il numero delle ruote giocate per il numero di terni generati in base ai numeri giocati sommato all'importo giocato sull'ambo per il numero della vincita per l'ambo per il numero di ambi generabili in quel terno che è vincente diviso il numero di ruote giocate e il numero di ambi generati in base al numero dei numeri totali giocati
				fprintf(vincite," Terno %f /\r\n", vincita);
				//fprintf(vincite, "-\r\n");
			} else if(numeri_indovinati == 4){
				vincita = (((sch->importi[3]*vincita_quaterna)/(quante_ruote_giocate*quaterne))+((sch->importi[2]*vincita_terno*terni_generabili(4))/(quante_ruote_giocate*terni))+((sch->importi[1]*vincita_ambo*ambi_generabili(4))/(quante_ruote_giocate*ambi)));
				fprintf(vincite," Quaterna %f /\r\n", vincita);
				//fprintf(vincite, "-\r\n");
			} else if(numeri_indovinati == 5){
				vincita = (((sch->importi[4]*vincita_cinquina)/(quante_ruote_giocate*cinquine))+((sch->importi[3]*vincita_quaterna*quaterne_generabili(5))/(quante_ruote_giocate*quaterne))+((sch->importi[2]*vincita_terno*terni_generabili(5))/(quante_ruote_giocate*terni))+((sch->importi[1]*vincita_ambo*ambi_generabili(5))/(quante_ruote_giocate*ambi)));
				fprintf(vincite," Cinquina %f /\r\n", vincita);
				//fprintf(vincite, "-\r\n");
			}
				
		}
	
	}
	if(numeri_indovinati!=0)
		fprintf(vincite, "-\r\n");
	fclose(vincite);
}

int ambi_generabili(int numeri_giocati) {
	if(numeri_giocati == 2){
		return 1;
	} else if(numeri_giocati == 3) {
		return 3;
	} else if(numeri_giocati == 4){
		return 6;
	} else if(numeri_giocati == 5){
		return 10;
	} else if(numeri_giocati == 6){
		return 15;
	} else if(numeri_giocati == 7){
		return 21;
	} else if(numeri_giocati == 8){
		return 28;
	} else if(numeri_giocati == 9){
		return 36;
	} else if(numeri_giocati == 10){
		return 45;
	}
	return 0;
}

int terni_generabili(int numeri_giocati) {
	if(numeri_giocati == 3){
		return 1;
	} else if(numeri_giocati == 4){
		return 4;
	} else if(numeri_giocati == 5){
		return 10;
	} else if(numeri_giocati == 6){
		return 20;
	} else if(numeri_giocati == 7){
		return 35;
	} else if(numeri_giocati == 8){
		return 56;
	} else if(numeri_giocati == 9){
		return 84;
	} else if(numeri_giocati == 10){
		return 120;
	}
	return 0;
}

int quaterne_generabili(int numeri_giocati) {
	if(numeri_giocati == 4){
		return 1;
	} else if(numeri_giocati == 5){
		return 5;
	} else if(numeri_giocati == 6){
		return 15;
	} else if(numeri_giocati == 7){
		return 35;
	} else if(numeri_giocati == 8){
		return 70;
	} else if(numeri_giocati == 9){
		return 126;
	} else if(numeri_giocati == 10){
		return 210;
	}
	return 0;
}

int cinquine_generabili(int numeri_giocati) {
	if(numeri_giocati == 5){
		return 1;
	} else if(numeri_giocati == 6){
		return 6;
	} else if(numeri_giocati == 7){
		return 21;
	} else if(numeri_giocati == 8){
		return 56;
	} else if(numeri_giocati == 9){
		return 126;
	} else if(numeri_giocati == 10){
		return 252;
	}
	return 0;
}

