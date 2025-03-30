#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
#define NUM_CHILDREN 5
#define CHUNK_SIZE (ARRAY_SIZE / NUM_CHILDREN)


int vec1[ARRAY_SIZE];
int vec2[ARRAY_SIZE];


void populate_array()
{
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        vec1[i] = rand() % 10;
        vec2[i] = rand() % 10;
    }
    write(1, "Finished array population\n", 26);
}

int do_sum(int start, int end)
{
    int partial_sum = 0;
    for (int i = start; i < end; i++)
    {
        partial_sum += vec1[i] + vec2[i];
    }
    return partial_sum;
}

int main(void)
{
    populate_array();

    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int total_sum = 0;

    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            close(fd[0]); 
            int start = i * CHUNK_SIZE;
            int end = start + CHUNK_SIZE;
            int partial_sum = do_sum(start, end);
            write(fd[1], &partial_sum, sizeof(partial_sum));
            close(fd[1]); 
            exit(0); 
        }
        else if (!(pid > 0))
        {
            write(1, "Error forking child process\n", 29);
            exit(1);
        }
    }

    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        int partial_sum;
        close(fd[1]); // Close the unused write end in the parent
        read(fd[0], &partial_sum, sizeof(partial_sum));
        total_sum += partial_sum;
    }

    printf("Final sum: %d\n", total_sum);

    close(fd[0]);


    return 0;
}