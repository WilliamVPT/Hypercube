#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct sommet{
    pid_t pid;
    int* etiq;
    int pipefd[2];
    int nb_adj;
    int* adj;
}sommet;

int main(int argc, char* argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dimension de l'hypercube>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int n = atoi(argv[1]); // Convertir l'argument en entier
    int pipefd[2], process[(int)(pow(2, n))];
    pid_t pid;

    for(int i = 0; i < pow(2, n); i++){
        if(pid=fork() == 0){
            process[i] = pid;
            printf("Processus n°%d , pid = %d\n", i, getpid());
            exit(EXIT_SUCCESS);
        }
    }
    printf("\n");
    for(int i = 0; i < pow(2, n); i++) {
        printf("processus n°%d, pid = %d\n", i, process[i]);    
    }

    // Attendre que tous les processus fils se terminent
    for(int i = 0; i < pow(2, n); i++) {
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
