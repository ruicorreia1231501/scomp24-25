#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define ARRAY_SIZE 100000
#define NUM_CHILDREN 5
#define CHUNK_SIZE (ARRAY_SIZE / NUM_CHILDREN)

int array[ARRAY_SIZE];
int target;

void populate_array()
{
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = i + 1; // Populate array with unique values
    }
    write(1, "Finished array population\n", 26);
}

void search_array(int start, int end, int child_num)
{
    for (int i = start; i < end; i++)
    {
        if (array[i] == target)
        {
            printf("Child %d found the number at position %d\n", child_num, i);
            exit(child_num);
        }
    }
    exit(0);
}

int main(void)
{
    populate_array();
    write(1, "Enter a number to search for: ", 30);
    scanf("%d", &target);

    pid_t children[NUM_CHILDREN];
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            int start = i * CHUNK_SIZE;
            int end = start + CHUNK_SIZE;
            search_array(start, end, i + 1);
        }
        else if (pid > 0)
        {
            // Parent process
            children[i] = pid;
        }
        else
        {
            write(1, "Error forking child process\n", 29);
            exit(1);
        }
    }

    int status;
    int found = 0;
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid_t pid = wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            printf("Parent: Child %d (PID %d) found the number\n", WEXITSTATUS(status), pid);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Parent: Number not found in the array\n");
    }

    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        if (children[i] != 0)
        {
            kill(children[i], SIGTERM);
        }
    }

    return 0;
}