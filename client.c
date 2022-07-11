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

#define LENGTH 2048
#define SIM_LENGTH 10 
#define IP_ADDRESS "127.0.0.1" 
#define PORT 1337 
#define SIZE 100
#define SIZE_USERNAME 50

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];


void str_trim_lf (char* arr, int length);

void catch_ctrl_c_and_exit();
void send_msg_handler();
void recv_msg_handler();



int main(int argc, char **argv){

	struct sockaddr_in server_addr;

	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&server_addr, sizeof(server_addr)); 			        // Clearing the struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
  server_addr.sin_port = htons(PORT);
	

  // Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// Send name
  printf("Please enter your name: ");
  fgets(name, 32, stdin);
  str_trim_lf(name, strlen(name));
  int retVal = send(sockfd, name, strlen(name), 0);
  if (retVal <= 0){
    puts("Error with sending\n");
    return EXIT_FAILURE;
  }
	printf("=== WELCOME TO THE %s ===\n",name);

	pthread_t send_msg_thread;
  if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
    return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	while (1){
		if(flag){
			printf("\nGood Bye\n");
			break;
    }
	}

	close(sockfd);

	return EXIT_SUCCESS;
}




void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void catch_ctrl_c_and_exit() {
    flag = 1;
}

void send_msg_handler() {
  char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

  while(1) {
    fgets(message, LENGTH, stdin);
    str_trim_lf(message, LENGTH);

    if (strcmp(message, "exit") == 0) {
			break;
    } else {
      sprintf(buffer, "%s: %s\n", name, message);
      send(sockfd, buffer, strlen(buffer), 0);
    }

		bzero(message, LENGTH);
    bzero(buffer, LENGTH + 32);
  }
  catch_ctrl_c_and_exit();
  pthread_cancel(pthread_self());
}

void recv_msg_handler() {
	char message[LENGTH] = {};
  while (1) {
		int receive = recv(sockfd, message, LENGTH, 0);
    if (receive > 0) {
      printf("%s", message);
    } 
    else if (receive == 0) {
			break;
    } 
    else {
			// -1
		}
		memset(message, 0, sizeof(message));
  }
  pthread_cancel(pthread_self());
}
