#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#define HASH_SIZE 128

// Structure for the hashmap entry
typedef struct Client 
{
    int fd;
    void (*function)(int);
} Client;

// Hashmap structure
typedef struct Hashmap
{
    Client clients[HASH_SIZE];
} Hashmap;

void initHashmap(Hashmap* hashmap);
void insert(Hashmap* hashmap, int fd, void (*function)(int));
void (*get(Hashmap* hashmap, int fd))(int);