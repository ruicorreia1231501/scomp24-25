#include <signal.h>
#include <unistd.h>
#include <string.h>

void handler(int signo)
{
    char msg[] = "I got SIGSEGV\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    exit(0);
}

int main(void)
{
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));

    act.sa_handler = handler;

    sigaction(SIGSEGV, &act, NULL);

    int a;
    a = *(int *)0;

    return 0;
}