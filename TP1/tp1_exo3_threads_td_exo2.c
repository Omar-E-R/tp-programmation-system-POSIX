#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>



#define NB_THREADS_MAX 20
#define ALPHABET_TAILLE 26

/*---------------VARIABLE GLOBALE PARTAGEE--------------------*/
int nbFichier;




/*-----------------------------------*/
void thdErreur(char *msg, int cause, int arret){
	printf("%s : %s \n", msg, strerror(cause));
	if (arret)
		pthread_exit(NULL);
}

void afficher(int* tab)
{
	for (int i = 0; i < ALPHABET_TAILLE; i++){
		printf("'%c': %d\n", i+97, tab[i]);
	}

}
void ajouterVal(int *tabInit, int *tabArg)
{
	for (int i = 0; i < ALPHABET_TAILLE; i++)
	{
		tabInit[i] += tabArg[i];
	}
}


/*------------------FONCTION APPELEE DANS CHAQUE THREAD-------------------*/
void *thread_calculer(void *arg){
	/*creation de tableau de 26 case ou les indices des case representent les codes ascii des lettres
	ex: 'a' ou 'A' sont represente par l'element d'indice 0*/
	int *tabAlpha=(int*)calloc(ALPHABET_TAILLE,sizeof(int));//allocation de memoire initialise a 0
	char* nomFichier=(char*)arg;

	/* ouverture de fichier en mode lecture*/
	int fd = open(nomFichier, O_RDONLY);
	if (fd == -1){
		perror("Impossible d'ouvrir le fichier:");
		exit(1);
	}
	/*calcul de nb d'occu */
	char lettre;
	while (read(fd, &lettre, sizeof(char)) > 0){
		if (lettre >=97 && lettre<=122){//lettre miniscule
			tabAlpha[(int) (lettre)-97]++;
		}
		if (lettre >= 65 && lettre <= 90){//lettre majuscule
			tabAlpha[(int)(lettre)-65]++;
		}
	}
	close(fd);
	pthread_exit((void *)tabAlpha);
}




int main (int argc, char* argv[]){
	if( argc < 2){
		printf("Usage: %s nom_fichier [...]\n", argv[0]);
		exit(1);
	}
	nbFichier=argc - 1;
	if (nbFichier>NB_THREADS_MAX){
		printf("\non peut traiter un nb maximum de fichier=%d\n", NB_THREADS_MAX);
		exit(1);
	}


	/*------------------------------------THREADS---------------------------------------------*/
	int etat;
	pthread_t idThreads[NB_THREADS_MAX];
	/*-------------CREATION DES THREADS-------------*/
	for (int i = 0; i < nbFichier; i++){

		if((etat=pthread_create(&idThreads[i], NULL, thread_calculer, (void *)argv[i+1]))!=0){//on donne comme arguments les noms des fichiers
			thdErreur("Echec pthread_create", etat, 0);
		}
	}

	int *tabResFinal=(int*)calloc(ALPHABET_TAILLE, sizeof(int));

	/*-------------ATTENTE DE LA FIN D'EXEC DES THREADS ET RECUPERATION DE LA VALEURE RETOURNER-------------*/
	for (int i = 0; i < nbFichier; i++){
		int* tabRet=NULL;
		if((etat=pthread_join(idThreads[i], (void**)&tabRet ))!=0){
			thdErreur("Echec pthread_join", etat, 0);
		}

		ajouterVal(tabResFinal, tabRet);

		free(tabRet);
	}

	/*---------AFFICHAGE DU RESULTAT FINALE--------*/
	afficher(tabResFinal);

	return 0;
}
