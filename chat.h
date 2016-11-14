#define MAX_NICKNAME_LENGTH 20
#define MAX_MESSAGE_LENGTH 1000

struct chat_message {
  int op; //0:messaggio, +1: iscrizione, -1:disiscrizione
  char nickname[MAX_NICKNAME_LENGTH], messaggio[MAX_MESSAGE_LENGTH];
}; 
