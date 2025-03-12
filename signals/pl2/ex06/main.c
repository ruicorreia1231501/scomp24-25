#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    char cmd[32];
    int time;
} command_t;

void alarm_handler(int signo)
{
    write(STDOUT_FILENO, "Command exceeded maximum execution time!\n", 41);
    exit(0);
}

int main(void)
{

    command_t commands[] = {{"ls", 5}, {"pwd", 1}, {"sleep 10", 2}, {"pwd", 1}};
    struct sigaction act;
    int num_commands = sizeof(commands) / sizeof(command_t);

    memset(&act, 0, sizeof(struct sigaction));

    act.sa_handler = alarm_handler;
    sigaction(SIGALRM, &act, NULL);

    for (int i = 0; i < num_commands; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            char *cmd = strtok(commands[i].cmd, " ");
            char *arg = strtok(NULL, " ");

            if (arg != NULL)
            {
                execlp(cmd, cmd, arg, NULL);
            }
            else
            {
                execlp(cmd, cmd, NULL);
            }
            exit(0);
        }
        else if (pid > 0)
        {
            // Parent process
            alarm(commands[i].time);
            int status;
            waitpid(pid, &status, 0);
        }
        else
        {
            write(STDOUT_FILENO, "Error forking process!\n", 24);
            exit(1);
        }
    }

    return 0;
}