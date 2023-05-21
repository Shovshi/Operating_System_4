#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

typedef void (*handler_t)(int);
void* createReactor();
void stopReactor(void * this);
void startReactor(void * this);
void addFd (void * this,int fd, handler_t handler);
void WaitFor(void * this);


