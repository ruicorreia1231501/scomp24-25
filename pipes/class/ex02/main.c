
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
        close(fd[1]);
 
        int sum =  0;
        int num;

        do{
            read(fd[0], &num, sizeof(num));

            if (num > 0 && num < 100 && sizeof(sum) != BUFFER_SIZE) {
                sum += num;
                printf("\nChild received: %d, current sum: %d\n", num, sum);
            }
        } while (num > 0);

        close(fd[0]);
        
        exit(sum);
        
    } else {
        // Parent process
        close(fd[0]);

        int read = 0;
        do
        {
            printf("Enter a number (0 to exit): \n");
            scanf("%d", &read);
            write(fd[1], &read, sizeof(read));
        } while (read);

        write(fd[1], &read, sizeof(read));
        close(fd[1]); 

        int status;
        waitpid(pid, &status, 0); // Wait for child to terminate

        if (WIFEXITED(status)) {
            int sum = WEXITSTATUS(status);
            printf("Sum received from child: %d\n", sum);
        }
    }

    return 0;
}