#Reactor Sample Code

This repository contains a sample implementation of a Reactor pattern in C, using the provided Reactor structure. The Reactor pattern is
a design pattern that allows for an event-driven, non-blocking I/O architecture.
In addition , this program implementing a chat that supports an unlimited number of customers, using this reactor structure. 

The Reactor structure represents the core component of the Reactor pattern implementation. It includes the following members:

    FdNode* fds: A pointer to the array of file descriptor nodes.
    int num_of_fds: The current number of file descriptors in the reactor.
    pthread_t thread: A thread used for running the reactor event loop.
    bool in_action: A flag indicating whether the reactor is currently active.
    handler_t handlers[MAX_FDS]: An array of event callback function pointers corresponding to each file descriptor.
    struct pollfd* poll_fds: An array of struct pollfd objects used for polling I/O events.

#Compilation and Execution

To compile and run the sample code, perform the following steps:
1. Set the library path:

```bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
```
Execute the compiled binary:
```bash
./react_server

#usage

Once the server is running, clients can connect to it using a TCP connection. For example, you can use Telnet:

```bash
telnet localhost 9034
```
Clients can send messages to the server, which will be echoed back to them.