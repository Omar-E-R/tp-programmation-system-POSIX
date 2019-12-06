#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define NB_LIGNES_MAX 20
#define NB_COLONNES_MAX 20

/*----------VARIABLE GLOBALE PARTAGEE------------*/
int nbLignes, nbColonnes;
float matrice[NB_LIGNES_MAX][NB_COLONNES_MAX];

void thdErreur(char *msg, int cause, int arret){
	printf("%s : %s \n", msg, strerror(cause));
	if (arret)
		pthread_exit(NULL);
}

void afficher(pthread_t thdId, int ligne, float valeurRet)
{
	printf("Thread compagnon identifié par %lu, j'ai retourné la somme de la ligne %d qui vaut %f\n", thdId, ligne, valeurRet);
}

/*------------------FONCTION APPELEE DANS CHAQUE THREAD-------------------*/
void *thread_calculer(void *arg){
	int currLigne= *(int *)arg;
	float *sommePartielle=malloc(sizeof(float)), somme=0;

	for (int i = 0; i < nbColonnes; i++){
		somme+=matrice[currLigne][i];
	}

	*sommePartielle=somme;

	pthread_exit((void *)sommePartielle);
}

int main (int argc, char* argv[]){

	if( argc != 3){
		printf("Usage: %s NB_LIGNES NB_COLONNES\n", argv[0]);
		exit(1);
	}

	nbLignes=atoi(argv[1]);
	nbColonnes=atoi(argv[2]);

	if (nbLignes>NB_LIGNES_MAX || nbColonnes>NB_COLONNES_MAX){
		printf("\nnbLignes et nbColonnes doivent etre respct. <= %d et <= %d\n", NB_LIGNES_MAX, NB_COLONNES_MAX);
		exit(1);
	}

	/*--------------- SAISIE DE DONNEES------------------*/
	printf("\nSaisiez la matrice ligne par ligne element par element\n");
	float element;
	for (int i = 0; i < nbLignes; i++)
	{
		for (int j = 0; j < nbColonnes; j++)
		{
			scanf("%f", &element);
			matrice[i][j] = element;
		}

	}

	/*------------------------------------THREADS---------------------------------------------*/
	int etat;
	pthread_t idThreads[nbLignes];
	int arg[nbLignes];
	/*-------------CREATION DES THREADS-------------*/
	for (int i = 0; i < nbLignes; i++){
		arg[i]=i;
		if((etat=pthread_create(&idThreads[i], NULL, thread_calculer, (void *)&arg[i]))!=0){
			thdErreur("Echec pthread_create", etat, 0);
		}
	}

	float somme=0;

	/*-------------ATTENTE DE LA FIN D'EXEC DES THREADS ET RECUPERATION DE LA VALEURE RETOURNER-------------*/
	for (int i = 0; i < nbLignes; i++){
		float*valRetourner;

		if((etat=pthread_join(idThreads[i], (void**)&valRetourner ))!=0){
			thdErreur("Echec pthread_join", etat, 0);
		}
		afficher(idThreads[i], i, *valRetourner);

		somme+=*valRetourner;

		free(valRetourner);
	}

	/*---------AFFICHAGE DU RESULTAT FINALE--------*/
	printf("Thread principale %lu: Somme= %f",pthread_self(), somme);

	return 0;
}
