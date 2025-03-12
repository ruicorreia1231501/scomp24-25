#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void handler(int signo)
{
    char msg[] = "Division by zero!\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    exit(0);
}

int main(void)
{
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));

    act.sa_handler = handler;

    sigaction(SIGFPE, &act, NULL);

    int a, b;
    while (1)
    {
        scanf("%d %d", &a, &b);
        printf("%d\n", a / b);
    }
}