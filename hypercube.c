#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "token.c"

int pause_execution = 0;
pid_t token_pid;
pid_t main_pid;

void appel_SIGCONT(int sig) {
    printf("Signal SIGCONT reçu. Processus repris. Appuyez sur 'q' pour arrêter.\n");
    if (token_pid > 0) {
        kill(token_pid, SIGCONT);
        pause_execution = 0;
    }
}

void appel_SIGQUIT(int sig) {
    printf("Arrêt du programme.\n");
    if (token_pid > 0) {
        kill(token_pid, SIGKILL); // Terminer le processus token
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    signal(SIGCONT, appel_SIGCONT);
    signal(SIGQUIT, appel_SIGQUIT);

    main_pid = getpid(); // Obtenir le PID du processus principal

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dimension de l'hypercube>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int n = atoi(argv[1]);
    sommet *sommets = (sommet *) malloc(pow(2, n) * sizeof(sommet));
    for(int i = 0; i < pow(2, n); i++){
        init_sommet(&sommets[i], i, n);
    }

    // Création de la structure de l'hypercube
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < pow(n, 2); j++) {
            int etiq = j ^ (1 << i);
            if (etiq >= 0 && etiq < pow(2, n)) {
                add_adj(&sommets[j], sommets[etiq].pipefd[1]);
            }
        }
    }


    // Lancement des processus et du token
    token_pid = fork();
    if (token_pid == 0) {
        Token(&sommets[0], sommets, pow(2, n));
    }

    printf("Entrez s pour suspendre, r pour reprendre et q pour arrêter\n");

    fd_set fds;
    struct timeval tv;
    int stdin_fd = fileno(stdin);
    char touche;

    while (1) {
        FD_ZERO(&fds);
        FD_SET(stdin_fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int ret = select(stdin_fd + 1, &fds, NULL, NULL, &tv);

        if (ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (ret > 0 && FD_ISSET(stdin_fd, &fds)) {
            touche = getchar();
            getchar(); // Vider le buffer d'entrée

            switch (touche) {
                case 's':
                    // Suspendre les processus fils
                    for(int i = 0; i<pow(2, n); ++i){
                        if(getpid() != main_pid && getpid() != sommets[i].pid) // Ne pas suspendre le processus principal
                            kill(sommets[i].pid, SIGSTOP);
                    }
                    break;
                case 'r':
                    // Reprendre le processus principal
                    appel_SIGCONT(SIGCONT);
                    break;
                case 'q':
                    // Quitter le programme
                    appel_SIGQUIT(SIGQUIT);
                    break;
                default:
                    // Touche non reconnue
                    printf("Touche non reconnue : %c\n", touche);
                    break;
            }
        }

        // Continuer à exécuter d'autres tâches ici si nécessaire
    }

    for(int i = 0; i < pow(2, n); i++){
        free(sommets[i].adj);
    }

    free(sommets);

    return EXIT_SUCCESS;
}
