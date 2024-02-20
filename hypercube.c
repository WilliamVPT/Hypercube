#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
    s->adj = (int *)malloc(pow(2,n) * sizeof(int));//tableau de la taille max des processus qui seront créer
}

//ajout un sommet dans le tableau des adjacent d'un sommet
void add_adj(sommet *s, int adjacent) {
    s->adj[s->nb_adj++] = adjacent;
}


int main(int argc, char* argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dimension de l'hypercube>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int n = atoi(argv[1]); // Convertir l'argument en entier
    sommet *sommets = (sommet *) malloc(pow(2,n)*sizeof(sommet));
    for(int i = 0; i < pow(2, n); i++){
        init_sommet(&sommets[i], i, n);
    }

    //création structure de l'hypercube
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < pow(n, 2); j++) {
            int etiq = j ^ (1 << i);
            if (etiq >= 0 && etiq < pow(2, n)) {
                add_adj(&sommets[j], sommets[etiq].pipefd[1]);
            }
        }
    }
    /* //affichage des sommets et des adjacents
    for(int i = 0; i<pow(2, n); i++){
        printf("Sommet : %d\n", sommets[i].etiq);
        int j = 0;
        while(j<=pow(2, n) && sommets[i].adj[j]){
            printf("Sommet adj : %d\n", sommets[i].adj[j]);
            j++;
        }
    }*/

    free(sommets);

    for(int i = 0; i<pow(2, n); i++){
        free(sommets[i].adj);
    }
    
    

    return EXIT_SUCCESS;
}
