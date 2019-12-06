/*
 * TP2 - Squelette exercice 2 - Synchronisation pour restreindre
 * les affichages possibles
 * Ressource partagee = ecran
 * Objectif de la synchronisation : que les threads ne puissent
 * afficher que ACDB ou ADCB a chaque "cycle"
 *
 * Pour tester avec la temporisation par usleep, il faut definir
 * la constante lors de la compilation grace a l'option -D :
 * gcc tp23_exo2_base.c -oexo2b -DTEMPO -lpthread
 * Rappel : Votre synchronisation doit marcher avec ou sans cette
 * temporisation
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem[2];
//---------------------------------------------------------------------
// Afficher un message d'erreur en fonction du code erreur obtenu
// codeErr = code de retour de la fonction testee
// codeArret = adresse de l'information retournee lors du pthread_exit
// Mettre NULL si cette information n'a pas d'importance
void thdErreur(int codeErr, char *msgErr, void *codeArret) {
  fprintf(stderr, "%s: %d soit %s \n", msgErr, codeErr, strerror(codeErr));
  pthread_exit(codeArret);
}

//---------------------------------------------------------------------
// Fonction pour perdre un peu de temps
void attenteAleatoire (void) {
#ifdef TEMPO
    sleep(rand()%3);
#endif
}

//---------------------------------------------------------------------
// Pour simplifier l'utilisation des semaphores et avoir un code qui
// ressemble a ce qui a ete ecrit en TD, on peut implanter l'equivalent
// des operations P et V pour les reutiliser dans les solutions
// Ou reprendre ce qui a ete fait pour l'exercice 1
//
// Implanter l'operation P
void P ( sem_t *sem ) {
  int etat;
  if ((etat=sem_wait(sem))!=0)
  {
    thdErreur(etat, "error P function", NULL);
  }

  /* En cas d'erreur, un message est affiche et l'execution avortee */
}

// Implanter l'operation V
void V(sem_t *sem)
{
  int etat;
  if ((etat = sem_post(sem)) != 0)
  {
    thdErreur(etat, "error V function", NULL);
  }
  /* En cas d'erreur, un message est affiche et l'execution avortee */
}


//---------------------------------------------------------------------
// Synchronisation a completer dans chacun des 3 threads
// en utilisant les operations P et V ecrites
//

//---------------------------------------------------------------------
// Fonction executee par le thread 1 : afficher A puis B
void *thd_1 (void *arg) {

  srand(pthread_self());

  for (;;) {
      attenteAleatoire();
      printf("A---\n");
      V(&sem[1]);
      V(&sem[2]);
      P(&sem[0]);
      P(&sem[0]);
      attenteAleatoire();
      printf("B\n");
      attenteAleatoire();
  }
  pthread_exit((void *)NULL);
}

//---------------------------------------------------------------------
// Fonction executee par le thread 2 : afficher C
void *thd_2 (void *arg) {

  srand(pthread_self());

  for (;;) {
      P(&sem[1]);
      attenteAleatoire();
      printf("C\n");
      attenteAleatoire();
      V(&sem[0]);
  }
  pthread_exit((void *)NULL);
}

//---------------------------------------------------------------------
// Fonction executee par le thread 3 : afficher D
void *thd_3 (void *arg) {

  srand(pthread_self());

  for (;;) {
      P(&sem[2]);
      attenteAleatoire();
      printf("D\n");
      attenteAleatoire();
      V(&sem[0]);
  }
  pthread_exit((void *)NULL);
}

#define NB_THDS 3

//---------------------------------------------------------------------
int main(void) {
  pthread_t idThds[NB_THDS];
  int etat;

  // Initialiser le(s) semaphore(s) utilise(s)
  for (int i = 0; i < NB_THDS; i++)
  {
    if((etat=sem_init(&sem[i], 0, 0))!=0){
      thdErreur(etat, "Error init sem", NULL);
    }
  }

  printf("J'ai augmenter le temps de sleep. Faites ctrl + c pour arreter le programme\n");

  // Lancer les threads
  if ((etat = pthread_create(&idThds[0], NULL, thd_1, NULL)) != 0)
      thdErreur(etat, "Creation thd 1", NULL);
  if ((etat = pthread_create(&idThds[1], NULL, thd_2, NULL)) != 0)
      thdErreur(etat, "Creation thd 2", NULL);
  if ((etat = pthread_create(&idThds[2], NULL, thd_3, NULL)) != 0)
      thdErreur(etat, "Creation thd 3", NULL);

  // Attendre leur fin
  if ((etat = pthread_join(idThds[0], NULL)) != 0)
    thdErreur(etat, "Join thread 1", NULL);
  if ((etat = pthread_join(idThds[1], NULL)) != 0)
    thdErreur(etat, "Join thread 2", NULL);
  if ((etat = pthread_join(idThds[2], NULL)) != 0)
    thdErreur(etat, "Join thread 3", NULL);

  // Detruire le(s) semaphore(s) utilise(s)
  for (int i = 0; i < NB_THDS; i++)
  {
    if ((etat = sem_destroy(&sem[i])) != 0)
    {
      thdErreur(etat, "Error dest sem", NULL);
    }
  }


  printf ("\nFin de l'execution du thread principal \n");
  return 0;
}
