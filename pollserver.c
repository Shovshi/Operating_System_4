#include "reactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int clientSockets[MAX_CLIENTS];
int numClients = 0;

// Function to handle client messages
void handle_client_message(int clientSocket , Reactor *reactor)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytesRead <= 0) {
        // Client disconnected
        printf("Client disconnected\n");
        
        // Remove clientSocket from the reactor
        delFd(reactor, clientSocket);
        
        for (int i = 0; i < numClients; i++) {
            if (clientSockets[i] == clientSocket) {
                // Remove clientSocket from the list
                memmove(clientSockets + i, clientSockets + i + 1, (numClients - i - 1) * sizeof(int));
                numClients--;
                break;
            }
        }
        
        close(clientSocket);
        return;
    }
    
    buffer[bytesRead] = '\0';
    printf("Received message from client: %s\n", buffer);
    
    // Broadcast the received message to all connected clients
    for (int i = 0; i < numClients; i++) {
        if (clientSockets[i] != clientSocket) {
            send(clientSockets[i], buffer, strlen(buffer), 0);
        }
    }
}

// Function to handle client connections
void handle_connection(int serverSocket , Reactor *reactor)
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    
    if (clientSocket == -1) {
        perror("accept");
        return;
    }
    
    if (numClients < MAX_CLIENTS) {
        clientSockets[numClients++] = clientSocket;
        printf("New client connected: %s\n", inet_ntoa(clientAddr.sin_addr));
        
        // Add clientSocket to the reactor for handling client messages
        addFd(reactor, clientSocket, (handler_t)handle_client_message);
    } else {
        printf("Maximum number of clients reached. Connection rejected.\n");
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
         WaitFor(reactor);
    }
    
    // Cleanup and stop the reactor
    stopReactor(reactor);
    
    // Close the server socket
    close(serverSocket);
    
    return 0;
}
   
