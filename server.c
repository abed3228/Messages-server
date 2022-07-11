 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>


#define LIST_USERNAME 50
#define SIZE_USERNAME 50
#define MAX_CLIENTS 100
#define BUFFER_SZ 2048
#define PORT 1337 

int uid = 10;
unsigned int cli_count = 0;
char usernames[LIST_USERNAME][SIZE_USERNAME]; // username list in the chat

/* Client structure */
typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void update_cli_count(int num);
void str_trim_lf (char* arr, int length);
void queue_add(client_t *cl);												// Add clients to queue 
void queue_remove(int uid);													//Remove clients to queue
void send_message(char *s, int uid);								//Send message to all clients except sender
void *handle_client(void *arg);                    	// Handle all communication with the client



int main(int argc, char **argv){
	int option = 1;
	int listenfd = 0;
	int connfd = 0;

	struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  pthread_t tid;

	/* Socket settings */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);

	/* Bind */
  if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR: Socket binding failed\n");
    return EXIT_FAILURE;
  }

  /* Listen */
  if (listen(listenfd, MAX_CLIENTS) < 0) {
    perror("ERROR: Socket listening failed\n");
    return EXIT_FAILURE;
	}

	printf("=== Welcome to the chat server  ===\n");


	while (1){
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Check if max clients is reached */
		pthread_mutex_lock(&clients_mutex);
			if((cli_count + 1) == MAX_CLIENTS){
				printf("Max clients reached. Rejected: ");
				printf(":%d\n", cli_addr.sin_port);
				close(connfd);
				continue;
			}
		pthread_mutex_unlock(&clients_mutex);

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &handle_client, (void*)cli);
	}
	
	return EXIT_SUCCESS;
}



void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { 
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}


void queue_add(client_t *cl){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	update_cli_count(1);
	client_t *cli = (client_t *)arg;

	// Name
	if(recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) <  2 || strlen(name) >= 32-1){
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	}
	else{
		strcpy(cli->name, name);
		sprintf(buff_out, "%s has joined\n", cli->name);
		printf("%s", buff_out);
		send_message(buff_out, cli->uid);
	}

	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				send_message(buff_out, cli->uid);
				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s \n", buff_out);
			}
		} 
		else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			sprintf(buff_out, "%s has left\n", cli->name);
			printf("%s", buff_out);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		} 
		else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  /* Delete client from queue and yield thread */
	close(cli->sockfd);
  queue_remove(cli->uid);
  free(cli);
  update_cli_count(-1);
  pthread_detach(pthread_self());

	return NULL;
}
                 	
void update_cli_count(int num){
	pthread_mutex_lock(&clients_mutex);
	if (num > 0)
	{
		cli_count++;
	}
	else{
		cli_count--;
	}
	pthread_mutex_unlock(&clients_mutex);
}
