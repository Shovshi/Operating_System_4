#include "hashmap.h"

// Initialize the hashmap
void initHashmap(Hashmap* hashmap) 
{
    for (int i = 0; i < HASH_SIZE; i++) {
        hashmap->clients[i].fd = -1;
        hashmap->clients[i].function = NULL;
    }
}

// Insert an entry into the hashmap
void insert(Hashmap* hashmap, int fd, void (*function)(void)) 
{
    int index = fd % HASH_SIZE;
    hashmap->clients[index].fd = fd;
    hashmap->clients[index].function = function;
}

// Get the function pointer associated with an fd from the hashmap
void (*get(Hashmap* hashmap, int fd))(void) 
{
    int index = fd % HASH_SIZE;
    return hashmap->clients[index].function;
}