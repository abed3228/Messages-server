#include "client.h"

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void parse_arguments(int argc, char **argv, char **ip, int *port)
{
  int option;
  while ((option = getopt(argc, argv, "i:p:")) != -1)
  {
    switch (option)
    {
    case 'i':
      *ip = optarg;
      break;
    case 'p':
      *port = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s -i ip_address -p port\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }
}

int connect_to_server(const char *ip, int port)
{
  struct sockaddr_in server_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Socket creation failed");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Connection to server failed");
    return -1;
  }
  return 0;
}

void setup_signals()
{
  struct sigaction sa;
  sa.sa_handler = catch_ctrl_c_and_exit;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
}

void catch_ctrl_c_and_exit(int sig)
{
  flag = 1;
}

void *send_msg_handler(void *arg)
{
  char message[LENGTH], buffer[LENGTH + 32];
  while (!flag)
  {
    fgets(message, LENGTH, stdin);
    str_trim_lf(message, LENGTH);

    sprintf(buffer, "%s: %s\n", name, message);
    if (send(sockfd, buffer, strlen(buffer), 0) == -1)
    {
      printf("Error send\n");
    }

    memset(message, 0, LENGTH);
    memset(buffer, 0, sizeof(buffer));
  }
  catch_ctrl_c_and_exit(0);
  return NULL;
}

void *recv_msg_handler(void *arg)
{
  char message[LENGTH];
  while (!flag)
  {
    int receive = recv(sockfd, message, LENGTH, 0);
    if (receive > 0)
    {
      printf("%s", message);
    }
    else if (receive == 0 || receive == -1)
    {
      break;
    }
    memset(message, 0, sizeof(message));
  }
  return NULL;
}

int client_main(char *ip, int port)
{

  // Connect to server
  if (connect_to_server(ip, port) != 0)
  {
    return EXIT_FAILURE;
  }

  // Get user name
  printf("Please enter your name: ");
  fgets(name, sizeof(name), stdin);
  str_trim_lf(name, strlen(name));

  // Send the name to the server
  if (send(sockfd, name, strlen(name), 0) == -1)
  {
    perror("Send name error");
    exit(1);
  }
  printf("=== WELCOME TO THE CHAT ROOM ===\n");

  // Thread initialization
  pthread_t send_msg_thread, recv_msg_thread;
  if (pthread_create(&send_msg_thread, NULL, send_msg_handler, NULL) != 0)
  {
    perror("pthread_create() error");
    exit(1);
  }
  if (pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0)
  {
    perror("pthread_create() error");
    exit(1);
  }

  // Handle program termination
  setup_signals();
  while (!flag)
  {
    sleep(1);
  }

  // Cleanup
  close(sockfd);
  return EXIT_SUCCESS;
}