/*
 * TP3 - Squelette exercice 3 - Rendez-vous entre N threads
 * Objectif de la synchronisation : que les threads s'attendent
 * pour passer un point de leur code
 *
 * Pour tester avec la temporisation par usleep, il faut definir
 * la constante lors de la compilation grace a l'option -D :
 * gcc tp23_exo3_base.c -oexo3b -DTEMPO -lpthread -Wall
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

int nbThreads, nbArriv;
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
    usleep(rand()%100);
#endif
}

//---------------------------------------------------------------------
// Pour simplifier l'utilisation des semaphores et avoir un code qui
// ressemble a ce qui a ete ecrit en TD, on peut implanter l'equivalent
// des operations P et V pour les reutiliser dans les solutions
// Ou reprendre ce qui a ete fait pour l'exercice 1
//
// Implanter l'operation P
void P(sem_t * sem)
{
  int etat;
  if ((etat = sem_wait(sem) != 0))
  {
    thdErreur(etat, "Error P function", NULL);
  }
}

// Implanter l'operation V
void V(sem_t * sem)
{
  int etat;
  if ((etat = sem_post(sem) != 0))
  {
    thdErreur(etat, "Error V function", NULL);
  }
}

//---------------------------------------------------------------------
// Synchronisation a completer pour realiser le RdV
//
void passerRdV (int num) {
  P(&sem[0]);
  nbArriv++;

  if(nbArriv<nbThreads){
    V(&sem[0]);
    P(&sem[1]);

  }else{
    for (int i = 1; i < nbThreads; i++){
      V(&sem[1]);
    }
    V(&sem[0]);
  }
  printf("Thread %d (%lu) : Je passe le point de RdV\n", num, pthread_self());
}

//---------------------------------------------------------------------
// Fonction executee par les threads
void *thd_rdv (void *arg) {
  int monNum = *(int *)arg;

  srand(pthread_self());

  printf("Thread %d (%lu) : J'effectue un traitement en parallele avec les autres\n",
		  monNum, pthread_self());
  attenteAleatoire();

  printf("Thread %d (%lu) : J'arrive au RdV\n", monNum, pthread_self());
  passerRdV(monNum);

  printf("Thread %d (%lu) : Je continue un traitement en parallele avec les autres\n",
		  monNum, pthread_self());
  attenteAleatoire();

  pthread_exit((void *)NULL);
}

#define NB_MAX_THREADS 30

//---------------------------------------------------------------------
int main(int argc, char *argv[]) {
  pthread_t idThds[NB_MAX_THREADS];
  int rangs[NB_MAX_THREADS];
  int i, etat;

  if (argc != 2) {
    printf("Usage : %s nombreDeThreads\n", argv[0]);
    exit(1);
  }

  nbThreads = atoi(argv[1]);
  if (nbThreads > NB_MAX_THREADS)
     nbThreads = NB_MAX_THREADS;

  // Initialiser le(s) semaphore(s) utilise(s)
  nbArriv=0;
  if((etat=sem_init(&sem[0], 0, 1))!=0){
    thdErreur(etat, "Init mutex ", NULL);
  }
  if((etat=sem_init(&sem[1], 0, 0))!=0){
    thdErreur(etat, "Init mutex ", NULL);
  }
  // Lancer les threads
  for (i = 0; i < nbThreads; i++) {
    rangs[i] = i;
    if ((etat = pthread_create(&idThds[i], NULL, thd_rdv, &rangs[i])) != 0)
      thdErreur(etat, "Creation thd ", NULL);
  }

  // Attendre leur fin
  for (i = 0; i < nbThreads; i++) {
    if ((etat = pthread_join(idThds[i], NULL)) != 0)
      thdErreur(etat, "Join thread 1", NULL);
  }

  // Detruire le(s) semaphore(s) utilise(s)
  if ((etat = sem_destroy(&sem[0])) != 0)
  {
    thdErreur(etat, "Init mutex ", NULL);
  }
  if ((etat = sem_destroy(&sem[1])) != 0)
  {
    thdErreur(etat, "Init mutex ", NULL);
  }

  printf ("\nFin de l'execution du thread principal \n");
  return 0;
}
