#include "reactor.h"

void* createReactor() 
{
    printf("Creating reactor...\n");
    Reactor* reactor = (Reactor*)malloc(sizeof(Reactor));
    if (reactor == NULL) {
        fprintf(stderr, "Failed to allocate memory for Reactor\n");
        return NULL;
    }
    initHashmap(&reactor->hashmap);
    reactor->num_of_fds = 0;
    reactor->in_action = false;
    reactor->handlers[0] = NULL;

    return reactor;
}

void stopReactor(void * this)
{
     if (this == NULL) {
        fprintf(stderr, "Invalid Reactor pointer\n");
        return;
    }
}

void startReactor(void * this)
{
    Reactor* reactor = (Reactor*) this;
    // Create the thread
    if (pthread_create(&reactor->thread, NULL, handle_client_message, reactor) != 0) 
    {
        fprintf(stderr, "Failed to create reactor thread\n");
        free(reactor);
        return NULL;
    }
}

void addFd (void * this,int fd, handler_t handler)
{
    if (this == NULL) 
    {
        fprintf(stderr, "Invalid Reactor pointer\n");
        return;
    }

    Reactor* reactor = (Reactor*)this;

    if (reactor->num_of_fds >= MAX_FDS)
    {
        fprintf(stderr, "Reached the maximum number of file descriptors\n");
        return;
    }

    // Store the file descriptor and handler in the reactor
    reactor->fds =  malloc(10 * sizeof(struct pollfd)); // Allocate memory for 10 struct pollfd elements
    reactor->handlers[reactor->num_of_fds] = handler;
    
    // Update the hashmap entry
    Client* client = &reactor->hashmap.clients[fd];
    client->fd = fd;
    client->function = handler;

    reactor->num_of_fds++;
}

void WaitFor(void * this)
{
    Reactor *reactor = (Reactor *)this;
    if (!reactor->in_action)
        return;

    pthread_join(reactor->thread, NULL);
}

int handle_client_message(int clientSocket)
{
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        perror("recv");
        return -1;
    } else if (bytesRead == 0) {
        return 1;
    } else {
        buffer[bytesRead] = '\0';
        printf("Received message from client: %s\n", buffer);
    }
    return 0;
}

void delFd(Reactor* reactor, int fd) 
{
   // Remove the file descriptor from the hashmap
    // Update the handlers array and fds array accordingly
    // Decrement the num_of_fds counter

    // Find the index of the file descriptor in the handlers array
    int index = -1;
    for (int i = 0; i < reactor->num_of_fds; i++)
    {
        if (reactor->fds[i].fd == fd)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // File descriptor not found
        return;
    }

    // Shift the elements after the index by one position to the left
    for (int i = index; i < reactor->num_of_fds - 1; i++)
    {
        reactor->handlers[i] = reactor->handlers[i + 1];
        reactor->fds[i] = reactor->fds[i + 1];
    }

    // Clear the last element
    reactor->handlers[reactor->num_of_fds - 1] = NULL;
    reactor->fds[reactor->num_of_fds - 1].fd = -1;
    reactor->fds[reactor->num_of_fds - 1].events = 0;

    // Decrement the num_of_fds counter
    reactor->num_of_fds--;
}

void runReactor(void *this)
{
     if (this == NULL) {
        fprintf(stderr, "Invalid Reactor pointer\n");
        return;
    }
    Reactor* reactor = (Reactor*)this;
    reactor->in_action = true;

    while (reactor->in_action) {
        int readyCount = poll(reactor->fds, reactor->num_of_fds, 5000); //-1
        if (readyCount <= 0) {
            continue;
        }
        for (int i = 0; i < reactor->num_of_fds; i++)
        {
            if (reactor->fds[i].revents & POLLIN) {
                reactor->handlers[i](reactor->fds[i].fd);
            }
            else if (reactor->fds[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
                delFd(reactor, reactor->fds[i].fd);
            }
        }
    }
    printf("Reactor thread finished.\n");
    return reactor;
}

void freeReactor(void *this)
{
    Reactor *reactor = (Reactor*)(this);
    free(reactor->fds);
    // Free the reactor struct itself
    free(reactor);
}
   