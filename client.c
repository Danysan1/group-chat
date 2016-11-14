#include "chat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char **argv){
	struct hostent *host;
	struct sockaddr_in clientaddr, servaddr;
	int sd, nread, port;
	fd_set maskFissa, maskPassata;
	struct chat_message data;
	char nickname[20];
	char const scrivi[]="Scrivi un messaggio: ",
				lettura[]="Lettura messaggio\n",
				ricezione[]="Ricezione messaggio\n";
	
	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=3){
		printf("Error:%s serverAddress serverPort\n", argv[0]);
		exit(1);
	}

	/* INIZIALIZZAZIONE INDIRIZZO SERVER--------------------- */
	memset((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	host = gethostbyname (argv[1]);
	if (host == NULL){
		printf("%s not found in /etc/hosts\n", argv[1]);
		exit(2);
	}

	nread = 0;
	while (argv[2][nread] != '\0'){
		if ((argv[2][nread] < '0') || (argv[2][nread] > '9')){
			printf("Secondo argomento non intero\n");
			exit(2);
		}
		nread++;
	}
	port = atoi(argv[2]);
	if (port < 1024 || port > 65535)
	{printf("Porta scorretta...");exit(2);}

	servaddr.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
	servaddr.sin_port = htons(port);

	/* INIZIALIZZAZIONE INDIRIZZO CLIENT--------------------- */
	memset((char *)&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;  
	clientaddr.sin_port = 0;
	
	printf("Client avviato\n");

	/* CREAZIONE SOCKET ---------------------------- */
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0) {perror("apertura socket"); exit(3);}
	printf("Creata la socket sd=%d\n", sd);
	
	/* BIND SOCKET, a una porta scelta dal sistema --------------- */
	if(bind(sd,(struct sockaddr *) &clientaddr, sizeof(clientaddr))<0)
	{perror("bind socket "); exit(1);}
	printf("Client: bind socket ok, alla porta %i\n", clientaddr.sin_port);

	/* CREAZIONE MASCHERE */
	FD_ZERO(&maskFissa);
	FD_SET(0, &maskFissa);
	FD_SET(sd, &maskFissa);
	maskPassata = maskFissa;
	
	/* CORPO DEL CLIENT: */
	data.op = +1;
	printf("Nickname (max 20 caratteri): ");
	
	if(fgets(nickname, 20, stdin) == 0){
		puts("EOF o errore, esco");
		exit(1);
	}
	
	write(1, scrivi, sizeof(scrivi));

	while (select(sd+1, &maskPassata, 0, 0, 0) > 0){
		socklen_t len;
		
		if(FD_ISSET(0, &maskPassata)){
			write(1, lettura, sizeof(lettura));
			
			if(fgets(data.messaggio, 1000, stdin) == 0)
				break; /* EOF inserito, esci */
			else { /* Invia messaggio*/
				data.op = 0;
				strncpy(data.nickname, nickname, MAX_NICKNAME_LENGTH);
				
				if (sendto(sd, &data, sizeof(data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) <= 0){
					perror("sendto");
					continue;
				}
			}
		}
	
		if (FD_ISSET(sd, &maskPassata)){
			write(1, ricezione, sizeof(ricezione));
			
			if (recvfrom(sd, &data, sizeof(data), 0, (struct sockaddr *)&servaddr, &len)<0){
				perror("recvfrom");
				continue;
			}
			
			printf("%s scrive:\n%s\n",data.nickname,data.messaggio);
		}
		
		maskPassata = maskFissa;
	} /* while*/

	printf("\nClient: termino...\n");
	shutdown(sd,0);
	shutdown(sd,1);
	
	data.op = -1;
	data.messaggio[0] = '\0';
	if (sendto(sd, &data, sizeof(data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) <= 0)
	  perror("sendto");
	close(sd);
	exit(0);
}
