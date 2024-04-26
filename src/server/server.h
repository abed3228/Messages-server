#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>   //for threading , link with lpthread
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

#include "../utility/utility.h"

#define LIST_USERNAME 50
#define SIZE_USERNAME 50
#define MAX_CLIENTS 100
#define BUFFER_SZ 2048
#define PORT 1337

/* Client structure */
typedef struct
{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[32];
} client_t;

/**
 * @brief Updates the count of connected clients.
 * @param num The number to adjust the client count by.
 * This function is typically called to increment or decrement the count of active clients.
 */
void update_cli_count(int num);

/**
 * @brief Adds a client to the queue of connected clients.
 * @param cl A pointer to the client_t structure representing the client to be added.
 * This function enqueues a new client into the server's list of managed clients.
 */
void queue_add(client_t *cl);

/**
 * @brief Removes a client from the queue based on their unique identifier.
 * @param uid The unique identifier of the client to remove.
 * This function dequeues a client from the server's list of managed clients using their unique ID.
 */
void queue_remove(int uid);

/**
 * @brief Sends a message to all connected clients except the sender.
 * @param s The message to send.
 * @param uid The unique identifier of the sending client.
 * This function broadcasts a message to all clients, excluding the client identified by uid.
 */
void send_message(char *s, int uid);

/**
 * @brief Handles all communication for a connected client.
 * @param arg A void pointer, typically used to pass a pointer to a client_t structure.
 * @return Always returns NULL. Thread termination is handled through signals and client disconnection.
 * This function manages all interactions with a specific client, such as receiving messages and handling disconnections.
 */
void *handle_client(void *arg);

/**
 * @brief The main function for the server program.
 * @param port The port number on which the server will listen for incoming connections.
 * @return Returns EXIT_SUCCESS on successful execution and EXIT_FAILURE on errors encountered during initialization.
 * This function sets up the server socket, binds, listens, and accepts client connections, spawning a new thread for each client.
 */
int server_main(int port);

#endif