#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "sommet.c"

#define BUFFER_SIZE 100

// Définir une structure pour stocker la valeur de pause_execution et un sémaphore pour protéger l'accès à la variable
typedef struct {
    int pause_execution;
    sem_t sem;
} shared_data;

shared_data *shared;

void Token(sommet *s, sommet *adjacents, int nb_adjacents) {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // processus fils
        close(s->pipefd[0]);
        char filename[10];
        sprintf(filename, "process_%d", s->etiq);
        int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        int Log = -1; // Initialiser à -1 pour indiquer que le descripteur de fichier n'est pas encore ouvert
        while (!shared->pause_execution) {
            if (Log == -1) {
                if ((Log = open("process_Log", O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1) {
                    perror("Erreur lors de l'ouverture du fichier de log");
                    exit(EXIT_FAILURE);
                }
            }
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(s->pipefd[0], &readfds);
            select(s->pipefd[0] + 1, &readfds, NULL, NULL, NULL);
            if (FD_ISSET(s->pipefd[0], &readfds)) {
                char buffer[BUFFER_SIZE];
                read(s->pipefd[0], buffer, BUFFER_SIZE);
                usleep(500000);
                // affichage visite
                time_t rawtime;
                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
                char message[BUFFER_SIZE];
                sprintf(message, "%s: Token reçu à %d\n", timestamp, s->etiq);
                // Vérifier si le processus doit être en pause avant d'écrire dans le fichier de log
                
                write(Log, message, strlen(message)); // Écriture dans le fichier de log
                
               

                // Envoyer le token à un sommet adjacent au hasard
                int rand_index = rand() % nb_adjacents;
                sommet adj = adjacents[rand_index];

                if (Log != -1) {
                    close(Log); // Fermer le fichier de log si nécessaire
                    Log = -1; // Réinitialiser le descripteur de fichier
                }

                // Appel récursif de Token avec le sommet adjacent
                Token(&adj, adjacents, nb_adjacents);

            }
        }
        // Sortir du processus fils si nécessaire
        exit(EXIT_SUCCESS);
    } else {
        // Processus père
        close(s->pipefd[1]);
        wait(NULL); // Attendre la fin du processus fils
    }
}
