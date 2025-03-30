
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
        int ppid;
        read(fd[0], &ppid, sizeof(ppid)); // Read from the pipe
        printf("Child says: Parent PID is %d\n", ppid);
        close(fd[0]); // Close the read end of the pipe
        
        exit(0);
    } else {
        // Parent process
        close(fd[0]); // Close the read end of the pipe

        printf("Parent says: My PID is %d\n", getpid());

        pid_t parent_pid = getpid();
    
        write(fd[1], &parent_pid, sizeof(parent_pid)); // Write to the pipe
    
        close(fd[1]); // Close the write end of the pipe

        wait(NULL); // Wait for child to terminate
    }

    return 0;
}