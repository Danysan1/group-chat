#include"chat.h"

/*
* Proxy si occupa di inoltrare i messaggi a tutti quelli
* che sono registrati. si utilizza il protocollo udp con scambio di datagrammi
* la struttura msg contiene un nickname che identifica chi l'ha mandato, il messaggio ed una serie di operazioni (reg/dereg)
* ./proxy #PORTA
*/

int dimensione = 0;
 
void main(int arg, char **argv) {
	int sock,porta_locale,num;
    socklen_t len;
	struct sockaddr_in sockadd_localhost;
	struct sockaddr_in cliaddr;
	struct sockaddr_in registrati[DIM];
	struct chat_message ric;
	
	if(arg!=2){
		printf("Error: %s port\n", argv[0]);
		exit(1);
	}
	
	num=0;
	while( argv[1][num]!= '\0' ){
		if( (argv[1][num] < '0') || (argv[1][num] > '9') ){
			printf("Argomento non intero\n");
			exit(2);
		}
		num++;
	} 	
	porta_locale = atoi(argv[1]);
	if (porta_locale < 1024 || porta_locale > 65535){
		printf("Error: %s port\n", argv[0]);
		printf("1024 <= port <= 65535\n");
		exit(2);  	
	}
	
	if( ( sock =socket(AF_INET,SOCK_DGRAM,0) ) < 0) {
		perror("errore in apertura socket");
		exit(1);
	}
		  
	sockadd_localhost.sin_family = AF_INET;
	sockadd_localhost.sin_addr.s_addr = INADDR_ANY;
	sockadd_localhost.sin_port = htons(porta_locale);

	if(  bind(sock,(struct sockaddr*) &sockadd_localhost,sizeof(sockadd_localhost)) < 0) {
		perror("errore in bind della sock");
		exit(1);
	}

	printf("Server proxy correttamente avviato (porta %d)\n", porta_locale);

	len=sizeof(struct sockaddr_in);
	while ( recvfrom(sock, &ric, sizeof(struct chat_message), 0,(struct sockaddr *)&cliaddr, &len)>0 )
		inoltro(ric,sock,registrati,cliaddr);
		
	close(sock);
	printf("Server proxy correttamente termina (porta %d)\n", porta_locale);
}

void inoltro(struct chat_message ms, int sock, struct sockaddr_in *registrati,struct sockaddr_in mittente) {
	int i,pos;
    socklen_t len;
	
	//registrazione
	if( ms.op == 1) {
		printf("procedo a registrazione\n");
		registrati[dimensione] =  mittente;
		dimensione++;
	}

	//de-registrazione
	pos=-1;
	if( ms.op == -1) {
		printf("procedo a de-registrazione\n");
		for(i=0; i<dimensione; i++)
			if( registrati[i].sin_addr.s_addr == mittente.sin_addr.s_addr 
				&& registrati[i].sin_port ==  mittente.sin_port)
				pos = i;
			
		if( pos >= 0) {	
			registrati[pos] = registrati[dimensione-1];
			dimensione--;
		}
	}
	
	//inoltro messaggi
	len =sizeof(struct sockaddr_in);
	printf("Inoltro messaggio di  %s\n",ms.nickname);
	for(i=0; i<dimensione; i++){
		//evito di inoltrare anche al mittente
		if( registrati[i].sin_addr.s_addr != mittente.sin_addr.s_addr || registrati[i].sin_port !=  mittente.sin_port)
			if ( sendto(sock, &ms, sizeof(struct chat_message), 0,(struct sockaddr *)&(registrati[i]), len)<0)
				chiusura(sock);
	}
}

void chiusura(int sock_udp) {
	close(sock_udp);
	exit(1);
}













