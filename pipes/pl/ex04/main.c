#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define PIPE_BUF 4096

int down[2], up[2];

void toggle_case(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (islower(str[i]))
            str[i] = toupper(str[i]);
        else if (isupper(str[i]))
            str[i] = tolower(str[i]);
    }
}

void handle_signal(int sig)
{
    if (sig == SIGUSR1)
    {
        char buffer[PIPE_BUF];

        // Read message from Up pipe
        read(up[0], buffer, PIPE_BUF);
        toggle_case(buffer);

        // Send modified message back to client via Down pipe
        write(down[1], buffer, strlen(buffer) + 1);
    }
}

int main()
{
    char buffer[PIPE_BUF];

    if (pipe(up) == -1 || pipe(down) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child (Client)
        close(up[0]);  // Close unused read end
        close(down[1]); // Close unused write end

        printf("Enter a message: ");
        fflush(stdout);
        if (fgets(buffer, PIPE_BUF, stdin) == NULL)
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        write(up[1], buffer, strlen(buffer) + 1);
        close(up[1]); // Now close after writing

        kill(getppid(), SIGUSR1);

        read(down[0], buffer, PIPE_BUF);
        printf("Received from server: %s\n", buffer);

        close(down[0]); // Close read end after receiving
    }
    else
    {
        // Parent (Server)
        close(down[0]); // Close unused read end
        close(up[1]);   // Close unused write end

        signal(SIGUSR1, handle_signal);

        printf("Server waiting for signal...\n");

        // Keep the server running until child finishes
        pause(); 

        close(up[0]);   // Close read end after receiving
        close(down[1]); // Close write end after sending
    }

    return 0;
}
