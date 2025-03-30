
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 100


struct data {
    int number;
    char name[BUFFER_SIZE];
};

int main() {
    pid_t pid;
    int fd[2];

    if(pipe(fd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        close(fd[1]); // Close the write end of the pipe

        struct data d;

        read(fd[0], &d, sizeof(d));
        printf("Child process received:\n");
        printf("Number: %d\n", d.number);
        printf("Name: %s\n", d.name);
        close(fd[0]); // Close the read end of the pipe

        exit(0);
        
    } else {
        // Parent process
        close(fd[0]); // Close the read end of the pipe

        struct data d;

        printf("Enter a number: ");
        scanf("%d", &d.number);

        printf("Enter a name: ");
        scanf("%s", d.name);

        write(fd[1], &d, sizeof(d));

        close(fd[1]);

        wait(NULL);
    }

    return 0;
}