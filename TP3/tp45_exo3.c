#define _GNU_SOURCE
/**-------------------------------------------------------------------------
  TP5 - Squelette code exercice 3
  Compilation : gcc tp45_exo3_base.c -o tp45_exo3 -Wall
--------------------------------------------------------------------------**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

#define LG_MAX 80
#define TEMPS_MAX 10
int nbMessages, tempsAttente, num;
int tube[2];
time_t t;
/*-----------------------------
 * Envoyer un message a chaque fois que le delai est ecoule
 * ----------------------------*/
void envoyerMessage (int sigRecu) {
  char monMsg[50];

  /* Constituer le message */
  sprintf(monMsg, "%s %d %s %s", "Message numero", num, " envoye a ", (time(&t), ctime(&t)));

  /* L'envoyer */
  int msgSize=strlen(monMsg);
  if(write(tube[1], monMsg, msgSize)!=msgSize){
    perror("Failed to write msg to child process");
    exit(EXIT_FAILURE);
  }
  num++;
  /* Armer la periode suivante */
  alarm(tempsAttente);
}

/*-----------------------------*/
void fils (void) {
  char leMsg[LG_MAX];

 /* Orienter le tube */
  if(close(tube[1])==-1){
    perror("Failed to close Write-pipe");
    exit(EXIT_FAILURE);
  }
 /* Boucler pour lire les messages en les affichant */
  while (read (tube[0], leMsg, LG_MAX) > 0)
    printf("Fils - Message de mon pere: %s\n", leMsg);

 close(tube[0]);
 exit(0);
}

/*-----------------------------*/
int main (int argc, char *argv[]) {


  /* Verifier les parametres */
  if (argc != 3) {
    printf("Usage : %s <Periode en secondes> <Nb d'envois>\n", argv[0]);
    exit(1);
  }

  /* Recuperer les parametres */
  tempsAttente = atoi(argv[1]);
  if (tempsAttente > TEMPS_MAX){
    tempsAttente = TEMPS_MAX;
  }
  nbMessages = atoi(argv[2]);
  if (nbMessages > LG_MAX){
      tempsAttente = LG_MAX;
    }
  /* Creer le tube de communication */
  if(pipe(tube)==-1){
    perror("Failed to create a pipe");
    exit(EXIT_FAILURE);
  }

  num=0;

  /* Creer le fils qui herite du tube */
  switch(fork()) {
    case -1  : perror("Echec fork ");
               exit(3);
    case 0   : fils();
    default  : break;
  }

  /* Orienter le tube */
  if(close(tube[0])==-1){
    perror("Failed to close Read-pipe");
    exit(EXIT_FAILURE);
  }

  /* Se proteger contre le signal SIGALRM */
  struct sigaction action;
  action.sa_handler=envoyerMessage;
  sigemptyset(&action.sa_mask);
  action.sa_flags=0;

  if (sigaction(SIGALRM, &action, NULL) < 0){
    perror("Echec sigaction");
    exit(EXIT_FAILURE);
  }

  /* Armer le traitement periodique */
  alarm(tempsAttente);

  /* Continuer son traitement : Par exemple, boucler sans rien faire */
  while(num < nbMessages);
  /* Le pere doit se terminer en dernier */
  close(tube[1]);
  while(wait(NULL)==-1);
  printf("Pere : Je me termine en dernier a %s",(time(&t), ctime(&t)));
  return 0;
}
