#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "reactor.h"

int clientSockets[100];
#define BUFFER_SIZE 1024
// Function to handle client messages
void handle_client_message(int clientSocket, Reactor *reactor)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

    if (bytesRead <= 0)
    {
        // Client disconnected
        printf("Client disconnected\n");

        // Remove clientSocket from the reactor
        delFd(reactor, clientSocket);

        for (int i = 0; i < reactor->num_of_fds; i++)
        {
            if (reactor->poll_fds[i].fd == clientSocket)
            {
                // Remove clientSocket from the list
                memmove(reactor->poll_fds + i, reactor->poll_fds + i + 1, (reactor->num_of_fds - i - 1) * sizeof(struct pollfd));
                memmove(reactor->handlers + i, reactor->handlers + i + 1, (reactor->num_of_fds - i - 1) * sizeof(handler_t));
                reactor->num_of_fds--;
                break;
            }
        }

        close(clientSocket);
        return;
    }
    else
    {
        buffer[bytesRead] = '\0';
        printf("Received data from socket %d: %s\n", clientSocket, buffer);
        // Broadcast the received message to all connected clients
        for (int i = 0; i < reactor->num_of_fds; i++)
        {
            if (clientSockets[i] != clientSocket)
            {
                send(clientSockets[i], buffer, strlen(buffer), 0);
            }
        }
    }
}
void handle_connection(int fd, Reactor *reactor)
{
    printf("New connection on socket %d\n", fd);

    // Accept the new client connection
    struct sockaddr_storage clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(fd, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket < 0)
    {
        perror("Error accepting connection");
        return;
    }

    // Add the new client socket to the reactor
    if (reactor->num_of_fds < MAX_FDS)
    {
        reactor->poll_fds[reactor->num_of_fds].fd = clientSocket;
        reactor->poll_fds[reactor->num_of_fds].events = POLLIN;
        reactor->handlers[reactor->num_of_fds] = handle_client_message;
        reactor->num_of_fds++;
    }
    else
    {
        printf("Maximum number of file descriptors reached\n");
        // handle error or gracefully close the connection
        close(clientSocket);
    }
}

int main()
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    
    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(1);
    }
    
    // Set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9034);
    
    // Bind the server socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(1);
    }
    
    // Listen for incoming connections
    if (listen(serverSocket, 10) == -1) {
        perror("listen");
        exit(1);
    }
    
    // Create the Reactor
    Reactor* reactor = createReactor();
    
    // Add serverSocket to the reactor for handling client connections
    addFd(reactor, serverSocket, (handler_t)handle_connection);
    
    // Start the reactor
    startReactor(reactor);

    while (1)
    {
        // Wait for events in the reactor
        waitFor(reactor);
    }
    
    // Cleanup and stop the reactor
    stopReactor(reactor);
    
    // Close the server socket
    close(serverSocket);
    
    return 0;
}