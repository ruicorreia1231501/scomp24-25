
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int fd[2];
    int num1, num2;

    if(pipe(fd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        close(fd[1]);

        int numbers[2];
        read(fd[0], numbers, sizeof(numbers));
        close(fd[0]); 
        int sum = numbers[0] + numbers[1];
        exit(sum);
        
    } else {
        // Parent process
        close(fd[0]); // Close the read end of the pipe

        printf("Enter two numbers (num1 num2): ");
        scanf("%d %d", &num1, &num2);
        int numbers[2] = {num1, num2};
        write(fd[1], numbers, sizeof(numbers)); 
        close(fd[1]); // Close the write end of the pipe   
        
        int status;
        waitpid(pid, &status, 0); // Wait for child to terminate

        if (WIFEXITED(status)) {
            int sum = WEXITSTATUS(status);
            printf("Sum received from child: %d\n", sum);
        }
    }

    return 0;
}