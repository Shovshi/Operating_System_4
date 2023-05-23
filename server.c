#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "reactor.h"

#define MAX_BUFFER_SIZE 1024

void handle_receive(int fd, Reactor *reactor)
{
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead = recv(fd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        printf("Received data from socket %d: %s\n", fd, buffer);
    }
    else if (bytesRead == 0)
    {
        printf("Connection closed on socket %d\n", fd);
        delFd(fd, reactor);
        close(fd);
    }
    else
    {
        perror("Error receiving data");
    }
}

void handle_connection(int fd, Reactor *reactor)
{
    printf("New connection on socket %d\n", fd);

    // Accept the new client connection
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(fd, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket < 0)
    {
        perror("Error accepting connection");
        return;
    }

    // Add the new client socket to the reactor
    addFd(reactor, clientSocket, handle_receive);
}

int main()
{
    int serverSocket;
    struct sockaddr_in serverAddress;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Error creating server socket");
        return 1;
    }

    // Set server address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(9034);

    // Bind server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Error binding server socket");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0)
    {
        perror("Error listening for connections");
        return 1;
    }

    // Create reactor
    Reactor *reactor = createReactor();
    if (reactor == NULL)
    {
        return 1;
    }

    // Add server socket to the reactor
    addFd(reactor, serverSocket, handle_connection);

    printf("Server is running on 127.0.0.1:9034\n");

    // Start the reactor
    startReactor(reactor);

    while (1)
    {
        // Wait for events in the reactor
        waitFor(reactor);
    }

    // Stop the reactor
    stopReactor(reactor);

    // Wait for the reactor thread to finish
    waitFor(reactor);

    // Cleanup
    close(serverSocket);
    free(reactor);

    return 0;
}