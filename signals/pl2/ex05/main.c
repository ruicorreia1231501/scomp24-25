#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    pid_t vpid[argc - 1];

    for (int i = 0; i < argc; i++)
    { 
        if (vpid[i] == 0)
        {
            vpid[i] = fork();
            if(vpid[i] == 0){
                execlp(argv[i + 1], argv[i + 1], NULL);
            }
        }else{
            kill(vpid[i], SIGCONT);
        }
        sleep(5);
        kill(vpid[i], SIGSTOP);
    }

    return 0;
}