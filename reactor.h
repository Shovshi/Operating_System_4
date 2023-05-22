#include "hashmap.h"

// Reactor structure
typedef struct Reactor
{
    Hashmap hashmap;

} Reactor;

void initHashmap(Hashmap* hashmap);
typedef void (*handler_t)(int);
void* createReactor();
void stopReactor(void * this);
void startReactor(void * this);
void addFd (void * this,int fd, handler_t handler);
void WaitFor(void * this);


