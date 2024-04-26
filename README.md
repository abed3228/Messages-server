# Messages-server

This Chat Application is a networked server-client system built in C for text-based communication.
It utilizes TCP/IP sockets and POSIX threads to handle multiple clients simultaneously in a robust and efficient manner.
The application provides a simple interface for real-time messaging.

## Features

- **Server-Client Architecture**: Manages communication between multiple clients via a central server.
- **Threaded Communication**: Uses separate threads for sending and receiving messages to ensure smooth, non-blocking operations.
- **Signal Handling**: Implements proper signal handling for graceful shutdown and cleanup.
- **Dynamic Client Management**: Supports dynamic connecting and disconnecting of clients without server interruption.

## Getting Started

Follow these instructions to get the project up and running on your local machine for development and testing purposes.

### Prerequisites

- GCC Compiler
- Make (for easy compilation via Makefile)
- POSIX Threads library
- A Unix-like environment (Linux, MacOS)

### Installation

1. **Clone the repository**

```bash
git clone https://github.com/yourusername/chat-application.git
cd  Messages-server
```

2. ** Compile the Server and Client**

```bash
make all
```

Or compile manually:

```bash
gcc -o server src/server/server_main.c src/server/server.c -lpthread
gcc -o client src/client/client_main.c src/client/client.c -lpthread
```

3. ** Start the Server or client **

server

```bash
./server -p 8080
```

client

```bash
./client -m client -i server_ip_address -p 8080
```

Replace server_ip_address with your server's actual IP address if running on different machines.

### Usage

To use the chat application, start the server first, then connect with one or more clients. Users can type messages into the client console, which will be sent to all other connected clients except the sender.
