/**-------------------------------------------------------------------------
  TP4 - Squelette code exercice 2-V2
  Compilation : gcc ELRIFAI_Omar_tp45_exo2-v2.c -o tp45_exo2-v2 -Wall
--------------------------------------------------------------------------**/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
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

/*------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

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

  printf("Processus (pere) de pid %d : Je vais executer boucle\n", getpid());

  /* Remplacer son code par celui de l'exécutable boucler */
  execlp("./boucle", "./boucle", NULL);
  erreur("erreur execlp", -1);
  return 0;
}
