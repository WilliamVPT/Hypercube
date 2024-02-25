#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>


typedef struct sommet{
    pid_t pid;
    int etiq;//identité du sommet
    int pipefd[2];
    int nb_adj;//nombre de sommetd adjacents
    int* adj;//tab des adjacents
}sommet;

//intialise le type sommet
void init_sommet(sommet *s, int etiq, int n){
    s->etiq = etiq;
    s->nb_adj = 0;
    pipe(s->pipefd);
    s->adj = (int *)malloc(n * sizeof(int));//tableau de la taille max des processus qui seront créer
}

//ajout un sommet dans le tableau des adjacent d'un sommet
void add_adj(sommet *s, int adjacent) {
    s->adj[s->nb_adj++] = adjacent;
}