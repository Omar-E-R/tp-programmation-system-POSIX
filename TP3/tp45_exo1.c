/**-------------------------------------------------------------------------
 * Omar EL RIFAI
  TP4 - code exercice 1
  Compilation : gcc tp45_exo1.c -o exo1 -Wall
--------------------------------------------------------------------------**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>


#define NB_FILS_MAX 10
#define NB_TOURS_MAX 10
#define IN_OUT 2
#define VAL_RAND 254

typedef struct
{
	pid_t pid;
	int val_conserv;
	int rang;
} data_t;
int nbFils, nbTours;

/*------------------------------------------------------------------------
 * Affichage de l'identite de l'appelant
  ------------------------------------------------------------------------*/
void afficher(data_t* data){
	pid_t currentChildPid = getpid();//le pid du processus qui  en  est  émetteur
	int next_rang = 1 + (data->rang % nbFils); //On stock le rang du processus prochain dans next_rang pour l'affichage
	if(data->rang==1){
		printf("Processus de pid %d, n°%d dans l’anneau : j’envoie au n°%d l’info [%d – %d – %d]\n", currentChildPid, data->rang, next_rang, currentChildPid, data->val_conserv, data->rang);
	}else{
		printf("Processus de pid %d, n°%d dans l’anneau : j'ai reçu de %d, j’envoie au n°%d l’info [%d – %d – %d]\n", currentChildPid, data->rang, data->pid, next_rang, currentChildPid, data->val_conserv, data->rang);
	}

	data->pid = currentChildPid; //On stock le pid du processus (fils) qui  en  est  émetteur pour le donner au prochain processus fils

	usleep(rand()%1000);
}

/*------------------------------------------------------------------------
 Fonction d'affichage d'une erreur selon la cause de l'echec d'une primitive
 Arret du processus avec retour valant codeRetour
  ------------------------------------------------------------------------*/
void erreur(char *msg, int codeRetour){
	perror(msg);
	exit(codeRetour);
}

/*------------------------------------------------------------------------
Fonctions write securisee
------------------------------------------------------------------------*/
void write_s(int fd, data_t *variable){
	if (write(fd, (void *)variable, sizeof(*variable)) < sizeof(variable)){
		erreur("Probleme ecriture dans le tube", 99);
	}
}
/*------------------------------------------------------------------------
Fonctions read securisee
------------------------------------------------------------------------*/
void read_s(int fd, data_t* variable){
	double break_out=0.0;
	while (read(fd, (void *)variable, sizeof(*variable)) < sizeof(variable)){
		break_out+=0.0000001;
		if(break_out > 100000){
			fprintf(stderr,"Probleme de lecteur dans le tube (boucle infini)\n");
			exit(98);
		}

	}
}
/*------------------------------------------------------------------------
Fonctions close securisee
------------------------------------------------------------------------*/
void close_s(int fd){
	if (close(fd) == -1)
	{
		erreur("Probleme fermeture tube redacteur", 97);
	}
}

int main(int argc, char *argv[]){

	if (argc != 3){
		printf("Usage : %s <Nb Fils> <Nb Tours>\n", argv[0]);
		exit(1);
	}
	nbFils = atoi(argv[1]);
	if (nbFils > NB_FILS_MAX)
		nbFils = NB_FILS_MAX;

	nbTours = atoi(argv[2]);
	if (nbTours > NB_TOURS_MAX)
		nbTours = NB_TOURS_MAX;

	int tube[NB_FILS_MAX][IN_OUT];
	/* Creation des Tubes */
	for (int i = 0; i < nbFils; i++){
		if (pipe(tube[i]) == -1)
			erreur("Probleme creation tube", 99 - i);
	}

	/* Creation des Processus Fils */
	for (int i = 0; i < nbFils; i++){
		switch (fork()){
			case -1:{
				erreur("Echec fork", 99 - i);
			}
			case 0:{//executer par chacun des processus fils
				data_t data;

				close_s(tube[i][1]);//fermeture du tube proprietaire en mode de write

				close_s(tube[(i + 1) % nbFils][0]);

				while (nbTours >= 0){
					if (nbTours == 0 && i > 0){//si le dernier tour est fait et ce n'est pas le processus numero 1
						printf("Processus de pid %d, je me termine (aussi)\n", data.pid);
						exit(0);

					}
					read_s(tube[i][0], &data);
					if (nbTours == 0){ //si le dernier tour est fait, le processus numero 1 se termine et arrete le cycle
						printf("Processus de pid %d, l’information m’est revenue de %d, je peux me terminer\n", getpid(), data.pid);
						exit(0);
					}
					data.rang = 1 + (data.rang % nbFils);//On incremente cette valeur recu de l'emetteur

					afficher(&data); //Affichage de l'identite de l'appelant

					write_s(tube[(i + 1) % nbFils][1], &data);

					nbTours--;//on decremente le nb de tour restant a faire
				}
				exit(0);//ca ne serait jamais executer, mais c'est juste pour eviter des ""bugs" eventuellement
			}
			default:
				break;
		}
	}
	data_t dat;//la structure de donnees de base initialis22 avec la valeur immutable
	dat.rang=0;
	dat.pid=0;
	dat.val_conserv=VAL_RAND;//valeur entiere immutable
	/*Fermer tous les tubes de 1 a NbFils d'ecriture et lecture
	  Car seul le premier tube (0) qui est utilise en ecriture*/
	close_s(tube[0][0]);
	for(int i=1; i<nbFils; i++){
		close_s(tube[i][0]);
		close_s(tube[i][1]);
	}
	write_s(tube[0][1], &dat);
	close_s(tube[0][1]);//fermer le seul tube utilisé, maintenant tous sont fermes

	/* Attendre la fin des fils (eventuellement)  */
	while (wait(NULL) != -1)
		;

	return 0;
}
