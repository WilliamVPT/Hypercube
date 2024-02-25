#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "token.c"

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

    // Lancement des processus et du token
    for (int i = 0; i < pow(2, n); i++) {
        Token(&sommets[i]);
    }

    int pause = 0; // Flag pour indiquer l'état de pause du processus principal
    printf("Entrez s pour suspendre, r pour reprendre et q pour arrêter\n");
    char touche;
    while (1) {
        touche = getchar();
        getchar(); // Vider le buffer d'entrée
        switch (touche) {
            case 's':
                // Suspendre le processus principal
                if (!pause) {
                    printf("Processus suspendu. Appuyez sur 'r' pour reprendre.\n");
                    pause = 1;
                }
                break;
            case 'r':
                // Reprendre le processus principal
                if (pause) {
                    printf("Processus repris.\n");
                    pause = 0;
                }
                break;
            case 'q':
                // Quitter le programme
                printf("Arrêt du programme.\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                // Touche non reconnue
                printf("Touche non reconnue : %c\n", touche);
                break;
        }
    }

    for(int i = 0; i<pow(2, n); i++){
        free(sommets[i].adj);
    }

    free(sommets);

    
    return EXIT_SUCCESS;
}
