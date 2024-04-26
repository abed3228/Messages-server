#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../utility/utility.h"

#define LENGTH 2048

/**
 * @brief Initializes signal handling for the application.
 * This function sets up handlers for SIGINT and other necessary signals.
 */
void setup_signals();

/**
 * @brief Handles SIGINT (Ctrl+C) to exit the application cleanly.
 * @param sig The signal number received.
 * This function performs necessary cleanup before terminating the application.
 */
void catch_ctrl_c_and_exit(int sig);

/**
 * @brief Thread function to handle sending messages to the server.
 * @param arg Typically unused in this context, can be used to pass data to the thread.
 * @return Always returns NULL. Thread termination is handled through signals.
 * This thread captures user input and sends it to the connected server.
 */
void *send_msg_handler(void *arg);

/**
 * @brief Thread function to handle receiving messages from the server.
 * @param arg Typically unused in this context, can be used to pass data to the thread.
 * @return Always returns NULL. Thread termination is handled through signals.
 * This thread listens for messages from the server and displays them to the user.
 */
void *recv_msg_handler(void *arg);

/**
 * @brief Establishes a connection to the server.
 * @param ip The IP address of the server as a string.
 * @param port The port number on which to connect to the server.
 * @return Returns 0 on success, -1 on failure.
 * This function initializes a socket connection to the specified server IP and port.
 */
int connect_to_server(const char *ip, int port);

/**
 * @brief Parses command line arguments to extract IP address and port information.
 * @param argc The number of arguments passed to the program.
 * @param argv The array of command-line arguments.
 * @param ip Pointer to a string where the IP address will be stored.
 * @param port Pointer to an integer where the port number will be stored.
 * This function processes command line arguments provided by the user to set network configuration.
 */
void parse_arguments(int argc, char **argv, char **ip, int *port);

/**
 * @brief The main function for the client program.
 * @param ip A string representing the IP address to connect to.
 * @param port An integer representing the port number to connect to.
 * @return Returns EXIT_SUCCESS on successful execution and EXIT_FAILURE on error.
 * This function initializes the client, connects to the server, and starts the message handling threads.
 */
int client_main(char *ip, int port);

#endif