#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Include the respective headers for client and server functionality
#include "src/client/client.h"
#include "src/server/server.h"

// Default configurations
#define DEFAULT_MODE "client"  // Default mode if none specified
#define DEFAULT_IP "127.0.0.1" // Default IP if none specified
#define DEFAULT_PORT 8080      // Default port if none specified

void usage(const char *exeName)
{
    printf("Usage: %s [-m mode] [-i ip] [-p port]\n", exeName);
    printf("   -m   mode: 'client' or 'server' (default: client)\n");
    printf("   -i   IP address to connect or bind to (default: %s)\n", DEFAULT_IP);
    printf("   -p   port to connect or listen on (default: %d)\n", DEFAULT_PORT);
}

int main(int argc, char **argv)
{
    int opt;
    char *mode = DEFAULT_MODE;
    char *ip = DEFAULT_IP;
    int port = DEFAULT_PORT;

    // Parse command line arguments
    while ((opt = getopt(argc, argv, "m:i:p:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            mode = optarg;
            break;
        case 'i':
            ip = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Decide based on the mode chosen
    if (strcmp(mode, "server") == 0)
    {
        printf("Starting server at IP %s on port %d\n", ip, port);
        return server_main(port);
    }
    else if (strcmp(mode, "client") == 0)
    {
        printf("Starting client connecting to IP %s on port %d\n", ip, port);
        return client_main(ip, port);
    }
    else
    {
        fprintf(stderr, "Invalid mode '%s'. Use 'client' or 'server'.\n", mode);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
