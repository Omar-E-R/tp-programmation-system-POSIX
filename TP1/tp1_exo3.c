	
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



#define NB_THREADS_MAX 10
#define NB_FOIS 10

/* variables globale */
int valeurInit;
pthread_t idThreadPrincipale;



void afficher(int rang, pthread_t thdId, int valeurRet)
{
	printf("Thread compagnon de rang %d, identifié par %lu, j'ai ajouté %d à la valeur qui vaut à présent %d\n", rang, thdId, valeurRet, valeurInit);

}

void afficher2( pthread_t thdId, int valeurRet)
{
	printf("Thread principal %lu : valeur retournée par le thread %lu = %d\n", idThreadPrincipale, thdId, valeurRet);

}

void thdErreur(char *msg, int cause, int arret)
{
	printf("%s : %s \n", msg, strerror(cause));
	if (arret)
		pthread_exit(NULL);
}

void *thd_incrementer(void *arg)
{

	int rang = *((int *)arg);
	int *nbRand=malloc(sizeof (int));

	pthread_t self_id = pthread_self();

	*nbRand=(rand() % 10);

	valeurInit+=*nbRand;

	afficher(rang, self_id, *nbRand);

	pthread_exit((void *)nbRand);
}


int main(int argc, char *argv[])
{
	time_t t=time(NULL);
	srand(t);

	int i, nbThreads, etat;
	pthread_t idThreads[NB_THREADS_MAX];
	idThreadPrincipale= pthread_self();
	int rang[NB_THREADS_MAX];

	if (argc != 2)
	{
		printf("Usage : %s <Nb Threads>\n", argv[0]);
		exit(1);
	}
	nbThreads = atoi(argv[1]);
	if (nbThreads > NB_THREADS_MAX)
		nbThreads = NB_THREADS_MAX;

	valeurInit=rand()%20;
	printf("Thread principal %lu : J’initialise la valeur à %d\n",idThreadPrincipale,valeurInit);

	/* Creation des threads */
	for (i = 0; i < nbThreads; i++)
	{
		rang[i] = i;
		if ((etat = pthread_create(&idThreads[i], NULL, thd_incrementer, (void *)&(rang[i]))) != 0)
			thdErreur("Echec create", etat, 0);

	}
	/* Attendre la fin des threads  */
	for (i = 0; i < nbThreads; i++)
	{
		int *nbRet=NULL;
		if ((etat = pthread_join(idThreads[i], (void **)&nbRet)) != 0)
			thdErreur("Echec join", etat, 0);

		afficher2(idThreads[i], *nbRet);

		free(nbRet);
	}

	printf("Thread principal %lu : La valeur vaut = %d\n", idThreadPrincipale, valeurInit);

	return 0;
}
