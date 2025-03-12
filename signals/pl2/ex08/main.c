#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define DB_RECORDS 10000
#define PROCESSES 10
#define CHUNK_SIZE (DB_RECORDS / PROCESSES)

pid_t children[PROCESSES];
volatile sig_atomic_t found = 0;

void signal_handler(int signo)
{
    found = 1;
    for (int i = 0; i < PROCESSES; i++)
    {
        if (children[i] > 0)
        {
            kill(children[i], SIGTERM);
            waitpid(children[i], NULL, 0);
        }
    }
}

typedef struct
{
    char name[50];
    char region[50];
    int phone;
} record_t;

record_t records[DB_RECORDS];

void populate_records()
{
    const char *names[] = {
        "Auto Repair Shop 1", "Auto Repair Shop 2", "Auto Repair Shop 3",
        "Auto Repair Shop 4", "Auto Repair Shop 5", "Auto Repair Shop 6",
        "Auto Repair Shop 7", "Auto Repair Shop 8", "Auto Repair Shop 9",
        "Auto Repair Shop 10"};

    const char *regions[] = {
        "Lisboa", "Porto", "Braga", "Coimbra", "Aveiro",
        "Faro", "Leiria", "Setubal", "Viseu", "Guarda"};

    for (int i = 0; i < DB_RECORDS; i++)
    {
        strncpy(records[i].name, names[i % 10], sizeof(records[i].name) - 1);
        records[i].name[sizeof(records[i].name) - 1] = '\0';

        strncpy(records[i].region, regions[i % 10], sizeof(records[i].region) - 1);
        records[i].region[sizeof(records[i].region) - 1] = '\0';

        records[i].phone = 220000000 + (rand() % 1000000);
    }
}

int main(void)
{
    srand(time(NULL));
    populate_records();

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = signal_handler;
    sigaction(SIGUSR1, &action, NULL);

    char region[50];
    printf("Enter a region: ");
    scanf("%s", region);

    for (int i = 0; i < PROCESSES; i++)
    {
        pid_t pid = fork();
        if (pid == 0) // Child
        {
            int start = i * CHUNK_SIZE;
            int end = start + CHUNK_SIZE;

            for (int j = start; j < end; j++)
            {
                if (found)
                    exit(0);

                if (strcmp(records[j].region, region) == 0)
                {
                    printf("Workshop: %s (Phone: %d)\n", records[j].name, records[j].phone);
                    kill(getppid(), SIGUSR1);
                    exit(0);
                }
            }
            exit(0);
        }
        else if (pid > 0) // Parent
        {
            children[i] = pid;
        }
    }

    for (int i = 0; i < PROCESSES; i++)
    {
        wait(NULL);
    }

    if (!found)
        printf("No workshops found in the region %s\n", region);

    return 0;
}
