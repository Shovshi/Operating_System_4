#include "reactor.h"
#define MAX_EVENTS 10

Reactor *createReactor()
{
    Reactor *reactor = (Reactor *)malloc(sizeof(Reactor));
    if (reactor == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for reactor\n");
        return NULL;
    }

    reactor->fds = NULL;
    reactor->num_of_fds = 0;
    reactor->in_action = false;

    for (int i = 0; i < MAX_FDS; i++)
    {
        reactor->handlers[i] = NULL;
    }

    reactor->poll_fds = (struct pollfd *)malloc(MAX_FDS * sizeof(struct pollfd));
    if (reactor->poll_fds == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for poll_fds\n");
        free(reactor);
        return NULL;
    }

    return reactor;
}
static void *reactorThread(void *arg)
{
    Reactor *reactor = (Reactor *)arg;

    while (!reactor->in_action)
    {
        fd_set read_fds;
        int max_fd = 0;
        FD_ZERO(&read_fds);

        FdNode *current = reactor->fds;
        while (current != NULL)
        {
            FD_SET(current->fd, &read_fds);
            if (current->fd > max_fd)
                max_fd = current->fd;
            current = current->next;
        }

        int ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ready > 0)
        {
            current = reactor->fds;
            while (current != NULL)
            {
                if (FD_ISSET(current->fd, &read_fds))
                    current->handler(current->fd, reactor);
                current = current->next;
            }
        }
    }

    return NULL;
}
void stopReactor(Reactor *reactor)
{
    reactor->in_action = false;
}
void startReactor(Reactor *reactor)
{
    reactor->in_action = true;
    pthread_create(&reactor->thread, NULL, reactorThread, reactor);
}
void addFd(Reactor *reactor, int fd, handler_t handler)
{
    if (reactor->num_of_fds >= MAX_FDS)
    {
        fprintf(stderr, "Cannot add more file descriptors to the reactor\n");
        return;
    }

    // Create a new FdNode
    FdNode *newFdNode = (FdNode *)malloc(sizeof(FdNode));
    if (newFdNode == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for FdNode\n");
        return;
    }
    newFdNode->fd = fd;
    newFdNode->next = NULL;

    // Add the new FdNode to the linked list of fds
    if (reactor->fds == NULL)
    {
        reactor->fds = newFdNode;
    }
    else
    {
        FdNode *current = reactor->fds;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newFdNode;
    }

    // Save the handler for the given fd
    reactor->handlers[fd] = handler;

    // Update the pollfd structure for the given fd
    reactor->poll_fds[reactor->num_of_fds].fd = fd;
    reactor->poll_fds[reactor->num_of_fds].events = POLLIN;
    reactor->poll_fds[reactor->num_of_fds].revents = 0;

    reactor->num_of_fds++;
}


void waitFor(Reactor *reactor)
{
     reactor->in_action = true;

    while (reactor->in_action)
    {
        int ready = poll(reactor->poll_fds, reactor->num_of_fds, -1);
        if (ready == -1)
        {
            fprintf(stderr, "Failed to poll file descriptors\n");
            break;
        }

        for (int i = 0; i < reactor->num_of_fds; i++)
        {
            if (reactor->poll_fds[i].revents & POLLIN)
            {
                int fd = reactor->poll_fds[i].fd;
                handler_t handler = reactor->handlers[fd];
                if (handler != NULL)
                {
                    handler(fd, reactor);
                }
            }
        }
    }
}
void delFd(int fd, Reactor *reactor)
{
    // Find the index of the file descriptor in the poll_fds array
    int index = -1;
    for (int i = 0; i < reactor->num_of_fds; i++)
    {
        if (reactor->poll_fds[i].fd == fd)
        {
            index = i;
            break;
        }
    }

    // If the file descriptor is found, remove it
    if (index != -1)
    {
        // Shift the remaining elements in the array to fill the gap
        for (int i = index; i < reactor->num_of_fds - 1; i++)
        {
            reactor->poll_fds[i] = reactor->poll_fds[i + 1];
        }

        // Decrement the count of file descriptors
        reactor->num_of_fds--;
    }

    // Remove the corresponding handler from the handlers array
    reactor->handlers[fd] = NULL;

    printf("SUCCESS TO REMOVE %d\n", fd);
}