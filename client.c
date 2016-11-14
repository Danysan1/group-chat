#define maxNicknameDim 20
#define maxMessageDim 1000

struct msg{
  int op; //0:messaggio, +1: iscrizione, -1:disiscrizione
  char nickname[maxNicknameDim], messaggio[maxMessageDim];
};

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
	struct msg;
	char nickname[20];

	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=3){
		printf("Error:%s serverAddress serverPort\n", argv[0]);
		exit(1);
	}

	/* INIZIALIZZAZIONE INDIRIZZO SERVER--------------------- */
	memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
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
	memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
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
	FD_SET(1, &maskFissa);
	FD_SET(sd, &maskFissa);
	maskPassata = maskFissa;
	
	/* CORPO DEL CLIENT: */
	msg.op = +1;
	printf("Nickname (max 20 caratteri): ");
	
	if(fgets(nickname, 20, stdin) <= 0){
		puts("EOF o errore, esco");
		exit(1);
	}
	
	printf("Messaggio: ");

	while (select(sd+1, &maskPassata, 0, 0, 0) > 1){
		if(!FD_ISSET(1, maskPassata) && fgets(msg.messaggio, 1000, stdin) > 0){
			
			
		}
	
		if (!FD_ISSET(sd, maskPassata)){
			printf("%s scrive:\n%s\n\n",msg.nickname,msg.messaggio);
			
			if (recvfrom(sd, &specialChar, sizeof(specialChar), 0, (struct sockaddr *)&cliaddr, &len)<0)
			{perror("recvfrom"); continue;}
			
			/*Resetto la struttura*/
			msg.op = 0;
			msg.nickname = nickname;
		}
		
		maskPassata = maskFissa;
	} /* while*/

	printf("\nClient: termino...\n");
	shutdown(sd,0);
	shutdown(sd,1);
	
	msg.op = -1;
	msg.messaggio = '\n';
	if (sendto(sd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len)<0)
	  perror("sendto")
	close(sd);
	exit(0);
}
