#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>

#define MAX_PROCESSES 16
#define BUFFER_SIZE 100

// Define the node structure
typedef struct node {
    int label;
    int adjacent[MAX_PROCESSES];
    int num_adjacent;
    int pipe_fds[2];
    pid_t pid;
} node;

// Initialize the node
void init_node(node *n, int label) {
    n->label = label;
    n->num_adjacent = 0;
    pipe(n->pipe_fds);
}

// Add a node to the list of adjacent nodes
void add_adjacent(node *n, int adjacent) {
    n->adjacent[n->num_adjacent++] = adjacent;
}

// Create a new process for a node
void create_process(node *n) {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // Child process
        close(n->pipe_fds[0]);
        char filename[10];
        sprintf(filename, "%d", n->label);
        int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        while (1) {
            // Wait for the token
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(n->pipe_fds[0], &readfds);
            select(n->pipe_fds[0] + 1, &readfds, NULL, NULL, NULL);
            if (FD_ISSET(n->pipe_fds[0], &readfds)) {
                char buffer[BUFFER_SIZE];
                read(n->pipe_fds[0], buffer, BUFFER_SIZE);
                // Log the visit
                time_t rawtime;
                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
                char message[BUFFER_SIZE];
                sprintf(message, "%s: Token visited\n", timestamp);
                write(fd, message, strlen(message));
                // Pass the token to a random adjacent node
                int adjacent = n->adjacent[rand() % n->num_adjacent];
                write(adjacent, buffer, BUFFER_SIZE);
            }
        }
    } else {
        // Parent process
        close(n->pipe_fds[1]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s n\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    node nodes[MAX_PROCESSES];
    for (int i = 0; i < MAX_PROCESSES; i++) {
        init_node(&nodes[i], i);
    }
    // Create the hypercube structure
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < MAX_PROCESSES; j++) {
            int label = j ^ (1 << i);
            if (label >= 0 && label < MAX_PROCESSES) {
                add_adjacent(&nodes[j], nodes[label].pipe_fds[1]);
            }
        }
    }
    // Create processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        create_process(&nodes[i]);
    }
    // Parent process waits for signal to suspend or resume processes
    while (1) {
        int c;
        printf("Enter s to suspend, r to resume, or q to quit: ");
        scanf(" %c", &c);
        if (c == 'q') {
            break;
        }
        if (c == 's') {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                kill(nodes[i].pid, SIGSTOP);
            }
        } else if (c == 'r') {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                kill(nodes[i].pid, SIGCONT);
            }
        }
    }
    return 0;
}