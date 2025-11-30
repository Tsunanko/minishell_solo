#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int g_signal = 0;

static void handler(int sig)
{
    g_signal = sig;
    write(1, "\n", 1);
}

void setup_signals_interactive(void)
{
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void setup_signals_child(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

