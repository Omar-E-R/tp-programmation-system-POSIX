
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define NB_THREADS_MAX 10
#define NB_FOIS 10


void afficher(int rang, pthread_t thdId, int valeurRet)
{
	printf("Thread compagnon de rang %d, identifié par %lu, je mourrai en retournant %d\n", rang, thdId, valeurRet);
}

void afficher_principale(pthread_t idThreadPrincipale, pthread_t thdId, int valeurRet)
{
	printf("Thread principal %lu : valeur retournée par le thread %lu = %d\n", idThreadPrincipale, thdId, valeurRet);
}

void thdErreur(char *msg, int cause, int arret)
{
	printf("%s : %s \n", msg, strerror(cause));
	if (arret)
		pthread_exit(NULL);
}

void *thd_afficher(void *arg)
{

	int rang = *((int *)arg);
	pthread_t self_id = pthread_self();
	int *nbRand=malloc(sizeof (int));
	*nbRand=(rand() % 20);
	afficher(rang, self_id, *nbRand);
	pthread_exit((void *)nbRand);
}


int main(int argc, char *argv[])
{
	int i, nbThreads, etat;
	pthread_t idThreads[NB_THREADS_MAX], idThreadPrincipale= pthread_self();
	int rang[NB_THREADS_MAX];
	int somme=0;

	if (argc != 2)
	{
		printf("Usage : %s <Nb Threads>\n", argv[0]);
		exit(1);
	}
	nbThreads = atoi(argv[1]);
	if (nbThreads > NB_THREADS_MAX)
		nbThreads = NB_THREADS_MAX;


	/* Creation des threads */
	for (i = 0; i < nbThreads; i++)
	{
		rang[i] = i;
		if ((etat = pthread_create(&idThreads[i], NULL, thd_afficher, (void *)&(rang[i]))) != 0)
			thdErreur("Echec create", etat, 0);
	}
	/* Attendre la fin des threads  */
	for (i = 0; i < nbThreads; i++)
	{
		int *nbRet=NULL;
		if ((etat = pthread_join(idThreads[i], (void **)&nbRet)) != 0)
			thdErreur("Echec join", etat, 0);
		afficher_principale(idThreadPrincipale, idThreads[i], *nbRet);
		somme += *nbRet;
		free(nbRet);
	}

	printf("Thread principal %lu : somme des valeurs reçues = %d\n", idThreadPrincipale, somme);

	return 0;
}
