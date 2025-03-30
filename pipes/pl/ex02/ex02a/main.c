
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

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

        int number;
        char name[BUFFER_SIZE];
        read(fd[0], &number, sizeof(number)); // Read number from pipe
        read(fd[0], name, sizeof(name)); // Read name from pipe

        close(fd[0]); 

        printf("Number received: %d\n", number);
        printf("Name received: %s\n", name);

        exit(0);
        
        
    
    } else {
        // Parent process
        close(fd[0]); // Close the read end of the pipe
        
        int number;
        char name[BUFFER_SIZE];

        printf("Enter a number: ");
        scanf("%d", &number);

        printf("Enter a name: ");
        scanf("%s", name);

        write(fd[1], &number, sizeof(number)); // Write number to pipe
        write(fd[1], name, sizeof(name)); // Write name to pipe

        close(fd[1]);

        wait(NULL);
    }

    return 0;
}