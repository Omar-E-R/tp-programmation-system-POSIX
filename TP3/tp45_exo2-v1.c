/**-------------------------------------------------------------------------
  TP4 - Squelette code exercice 2-V1
  Compilation : gcc ELRIFAI_Omar_tp45_exo2-v1.c afficher.c -o tp45_exo2-v1 -Wall
--------------------------------------------------------------------------**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "afficher.h"


/*------------------------------------------------------------------------
 Fonction d'affichage d'une erreur selon la cause de l'echec d'une primitive
 Arret du processus avec retour valant codeRetour
  ------------------------------------------------------------------------*/
void erreur (char *msg, int codeRetour) {
  perror(msg);
  exit(codeRetour);
}

/*------------------------------------------------------------------------
 Traitement associé à SIGINT
  ------------------------------------------------------------------------*/
void traiterSIGINT (int sig) {
  if(sig == SIGINT){
    printf(">> Ctrl-C/SIGINT recu par %d\n", getpid());
  }
}


/*------------------------------------------------------------------------
 Code execute par chaque processus fils
  ------------------------------------------------------------------------*/
void fils () {
  while(1){//boucler indefiniment
    afficher();
  }
  exit(0);
}

/*------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
  pid_t pidFils;
  // Se protéger contre Ctrl-C (SIGINT)
  struct sigaction action;
  action.sa_handler= traiterSIGINT; //la fonction qui va traiter le signal
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if(sigaction(SIGINT, &action, NULL) != 0){
    erreur("erreur sigaction", -1);
  }
  printf("Processus (pere) de pid %d : protege contre SIGINT\n", getpid());
  sigsuspend(&action.sa_mask);//On attend le signal qui va etre traiter par action

  /* Créer son fils */
  switch (pidFils=fork()) {
    case - 1 : erreur("Echec fork", 1);

    case 0 : fils();

    /* default : break; */
  }

  printf("Processus (pere) de pid %d : j'ai cree un fils de pid %d\n", getpid(), pidFils);

  /* Attendre éventuellement la fin de son fils */
  while(wait(NULL)==-1);

  return 0;
}
