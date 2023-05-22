#include "hashmap.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_FDS 100

// Reactor structure
typedef struct Reactor
{
    Hashmap hashmap;
    int num_of_fds;
    pthread_t thread;
    bool in_action;
    handler_t handlers[MAX_FDS];
    struct pollfd* fds;

} Reactor;

typedef void (*handler_t)(int);
void* createReactor();
void stopReactor(void * this);
void startReactor(void * this);
void addFd (void * this,int fd, handler_t handler);
void WaitFor(void * this);
int handle_client_message(int clientSocket);
void delFd(Reactor* reactor, int fd);
void freeReactor(void *this);


