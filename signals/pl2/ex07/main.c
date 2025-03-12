#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAX_PROCESSES 50
#define CHECKPOINT 25

int success = 0;
pid_t children[MAX_PROCESSES];

int simulate1()
{
    usleep((rand() % 1500 + 500) * 1000); // Random sleep between 500ms and 2000ms
    return (rand() % 2) ? 1 : 0;          // 20% success rate
}

void simulate2()
{
    usleep((rand() % 2000 + 1000) * 1000); // Random sleep between 1000ms and 3000ms
    printf("Process %d is running simulate2()\n", getpid());
}

void signal_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        successful_searches++;
    }
}

void continue_execution(int signo)
{
    simulate2();
    exit(0);
}

int main(void)
{
    srand(time(NULL));

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = signal_handler;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        { // Child process
            struct sigaction child_act;

            memset(&child_act, 0, sizeof(struct sigaction));

            child_act.sa_handler = continue_execution;

            sigaction(SIGUSR1, &child_act, NULL);

            if (simulate1() == 1)
            {
                kill(getppid(), SIGUSR1);
            }
            else
            {
                kill(getppid(), SIGUSR2);
            }

            pause();
        }
        else
        { // Parent process
            children[i] = pid;
        }
    }

    sleep(2);

    if (success == 0)
    {
        printf("Inefficient Algorithm! Terminating children...\n");
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            kill(children[i], SIGKILL);
        }
    }
    else
    {
        printf("At least one success! Signaling children to start simulate2()\n");
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            kill(children[i], SIGUSR1);
        }
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        wait(NULL);
    }

    return 0;
}
