#pragma once
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/select.h>

#define MAX_FDS 100
typedef struct Reactor Reactor;
typedef void (*handler_t)(int clientSocket, Reactor* reactor);

typedef struct FdNode
{
    int fd;
    handler_t handler;
    struct FdNode *next;
} FdNode;

typedef struct Reactor
{
    FdNode *fds;
    int num_of_fds;
    pthread_t thread;
    bool in_action;
    handler_t handlers[MAX_FDS];
    struct pollfd* poll_fds;
} Reactor;

Reactor *createReactor();
void stopReactor(Reactor *reactor);
void startReactor(Reactor *reactor);
void addFd(Reactor *reactor, int fd, handler_t handler);
void waitFor(Reactor *reactor);
void delFd(int fd, Reactor *reactor);