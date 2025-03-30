#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
#define NUM_CHILDREN 5
#define CHUNK_SIZE (ARRAY_SIZE / NUM_CHILDREN)

int vec1[ARRAY_SIZE];
int vec2[ARRAY_SIZE];
int result[ARRAY_SIZE];

void populate_arrays() {
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        vec1[i] = rand() % 100;  // Random numbers (0-99)
        vec2[i] = rand() % 100;
    }
    printf("Finished array population\n");
}

int main() {
    populate_arrays();

    int pipes[NUM_CHILDREN][2];  // 5 pipes (one per child)
    pid_t children[NUM_CHILDREN];

    // Create pipes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            close(pipes[i][0]);  // Close unused read end

            int start = i * CHUNK_SIZE;
            int end = start + CHUNK_SIZE;

            for (int j = start; j < end; j++) {
                int sum = vec1[j] + vec2[j];
                write(pipes[i][1], &sum, sizeof(int));
            }

            close(pipes[i][1]);  // Close write end after sending data
            exit(0);
        }
        else if (pid > 0) {
            // Parent process
            children[i] = pid;
        }
        else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    
    for (int i = 0; i < NUM_CHILDREN; i++) {
        close(pipes[i][1]);

        int start = i * CHUNK_SIZE;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            read(pipes[i][0], &result[start + j], sizeof(int));
        }

        close(pipes[i][0]); 
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        waitpid(children[i], NULL, 0);
    }

    printf("Result array: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    return 0;
}
