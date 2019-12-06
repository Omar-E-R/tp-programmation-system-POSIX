#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define NB_LIGNES_MAX 20
#define NB_COLONNES_MAX 20

struct ensembleArg{
	float* matrice;
	int nbLignes, nbColonnes, currLignes;

};

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
	struct ensembleArg *ensArg= (struct ensembleArg *)arg;

	float *sommePartielle=malloc(sizeof(float)), somme=0;

	int incremLigne=(ensArg->nbLignes)*(ensArg->currLignes);

	for (int i = 0; i < ensArg->nbColonnes; i++)
	{
		somme+=(ensArg->matrice[ incremLigne + i]);
	}

	*sommePartielle=somme;
	free(ensArg);
	pthread_exit((void *)sommePartielle);
}

int main (void){

	/*--------------- SAISIE DE DONNEES------------------*/

	int nbLignes, nbColonnes;
	float* matrice=malloc(NB_COLONNES_MAX*NB_LIGNES_MAX*sizeof(float));

	printf("Saisiez la taille de la matrice\n");
	printf("NB de LIGNES=");
	scanf("%d", &nbLignes);
	printf("\nNB de COLONNES=");
	scanf("%d", &nbColonnes);

	if (nbLignes>NB_LIGNES_MAX || nbColonnes>NB_COLONNES_MAX){
		printf("\nnbLignes et nbColonnes doivent etre respct. <= %d et <= %d\n", NB_LIGNES_MAX, NB_COLONNES_MAX);
		exit(1);
	}

	printf("\nSaisiez la matrice ligne par ligne element par element\n");
	float element;
	for (int i = 0; i < nbLignes; i++)
	{
		for (int j = 0; j < nbColonnes; j++)
		{
			scanf("%f", &element);
			matrice[ i*nbLignes + j] = element;
		}

	}




	/*-------------THREADS-------------*/
	int etat;
	pthread_t idThreads[nbLignes];

	/*-------------CREATION DES THREADS-------------*/
	for (int i = 0; i < nbLignes; i++){
		struct ensembleArg *arg=malloc(sizeof(struct ensembleArg));
		arg->nbLignes=nbLignes;
		arg->nbColonnes=nbColonnes;
		arg->currLignes=i;
		arg->matrice=matrice;

		if((etat=pthread_create(&idThreads[i], NULL, thread_calculer, (void *)arg))!=0){
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

	free(matrice);

	/*------AFFICHAGE DU RESULTAT FINALE--------*/
	printf("Thread principale %lu: Somme= %f",pthread_self(), somme);

	return 0;
}
