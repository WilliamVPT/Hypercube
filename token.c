#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "sommet.c"

#define BUFFER_SIZE 100


void Token(sommet *s) { //crée les processus et fait se balader le token
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // processus fils
        close(s->pipefd[0]);
        char filename[10];
        sprintf(filename, "%d", s->etiq);
        int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        while (1) {
            // Wait for the token
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(s->pipefd[0], &readfds);
            select(s->pipefd[0] + 1, &readfds, NULL, NULL, NULL);
            if (FD_ISSET(s->pipefd[0], &readfds)) {
                char buffer[BUFFER_SIZE];
                read(s->pipefd[0], buffer, BUFFER_SIZE);
                // afffichage visite
                time_t rawtime;
                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
                char message[BUFFER_SIZE];
                sprintf(message, "%s: Token reçu\n", timestamp);
                write(fd, message, strlen(message));
                // envoie le token ailleurs aléatoirement
                int adj = s->adj[rand() % s->nb_adj];
                write(adj, buffer, BUFFER_SIZE);
            }
        }
    } else {
        // Processus père
        close(s->pipefd[1]);
    }
}