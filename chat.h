#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h> 
#include<unistd.h>
#include<sys/wait.h> 
#include<sys/stat.h> 
#include<signal.h> 
#include<dirent.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <sys/time.h>
#include<time.h>

#define MAX_NICKNAME_LENGTH 20
#define MAX_MESSAGE_LENGTH 1000
#define DIM 100

struct chat_message {
  int op; /* 0:messaggio, +1: iscrizione, -1:disiscrizione */
  char nickname[MAX_NICKNAME_LENGTH], messaggio[MAX_MESSAGE_LENGTH];
}; 

void chiusura(int sock_udp);
void inoltro(struct chat_message ms, int sock, struct sockaddr_in *registrati,struct sockaddr_in mittente);

