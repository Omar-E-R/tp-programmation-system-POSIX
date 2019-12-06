/**-------------------------------------------------------------------------
  TP1 - Squelette code exercice 1-V1
  Compilation : gcc tp1_exo1-v1_base.c -o exo1b -Wall
--------------------------------------------------------------------------**/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#define NB_FILS_MAX  10
#define NB_FOIS      10


/*------------------------------------------------------------------------
 Fonction d'affichage d'une erreur selon la cause de l'echec d'une primitive
 Arret du processus avec retour valant codeRetour
  ------------------------------------------------------------------------*/
void erreur (char *msg, int codeRetour) {
  perror(msg);
  exit(codeRetour);
}

/*------------------------------------------------------------------------
 Code execute par chaque processus fils
  ------------------------------------------------------------------------*/
void fils (char* fichier, char caractereRechercher) {
  /* ouverture de fichier en mode lecture*/
  int fd=open(fichier, O_RDONLY);
  if(fd==-1){
    perror("Impossible d'ouvrir le fichier:");
    exit(1);
  }
  /*calcul de nb d'occu */
  char caractLu;
  int nbOcc=0;
  while (read(fd, &caractLu, sizeof(char))>0){
    if (caractLu==caractereRechercher)
      nbOcc++;
  }
  close(fd);

  /* ecrire le nb d'occu dans un fichier portant PID du processus comme nom*/
  char filename[10];
  sprintf(filename, "%d.txt", getpid());

  fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR);

  if (fd==-1){
    perror("Impossible de cree un fichier");
    exit(1);
  }

  write(fd, &nbOcc, sizeof(nbOcc) );

  close(fd);

  exit(0);

}

/*------------------------------------------------------------------------*/


int main(int argc, char *argv[]) {

  int i, nbFils;
  char caractRechercher;
  pid_t pids[NB_FILS_MAX];


  if (argc <= 2) {
    printf("Usage : %s fich1.txt fich2.txt[..]\n", argv[0]);
    exit(1);
  }

  nbFils = argc - 1;
  if (nbFils > NB_FILS_MAX)
    nbFils = NB_FILS_MAX;

  printf("Rechercher le nb d'occ de quel caractere?\n");
  scanf("%c", &caractRechercher);


  /* Creation des fils */
  for (i = 0; i < nbFils; i++) {
     switch (pids[i] = fork()) {
       case - 1 : erreur("Echec fork", 1);
      /* executer par le fils*/
       case 0 : fils(argv[i+1], caractRechercher);
      /*executer par le main*/
       default : break;
     }
  }

  /*------------------executer par le main----------------------*/

  /* Attendre la fin des fils (eventuellement)  */
  while (wait(NULL) != -1);

  /* lecture des resultat depuis les fichiers crees par les processus fils*/
  int res=0;
  for (int i = 0; i < nbFils; i++){
    char filename[10];
    sprintf(filename, "%d.txt", pids[i]);
    int fd=open(filename, O_RDONLY), valProc;
    read(fd, &valProc, sizeof(int) );
    remove(filename);
    close(fd);
    res+= valProc;
  }

  printf("RESULTAT=%d\n", res);

  return 0;
}
