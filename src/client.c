#include "client.h"

#include <arpa/inet.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network.h"

#define MAX_MSG_SIZE 512
#define STANDARD_PORT 8111
#define BUFFER_SIZE 1024

static ClientArgs* clientArgs;

typedef enum { CONNECT, LOGIN, QUIT, SEND, INVALID } COMMANDS;

COMMANDS parseCmd(char* mqMsg) {
  debugPrint("%s received: %s", clientArgs->name, mqMsg);

  if (strcmp("QUIT", mqMsg) == 0) {
    return QUIT;
  }
  if (strcmp("CONNECT", mqMsg) == 0) {
    return CONNECT;
  }
  if (strcmp("LOGIN", mqMsg) == 0) {
    return LOGIN;
  }
  return INVALID;
}

void reportSuccess() {
  char mq_buffer[MAX_MSG_SIZE];
  snprintf(mq_buffer, MAX_MSG_SIZE, "OK");
  if (mq_send(clientArgs->mqSendReport, mq_buffer, strlen(mq_buffer) + 1, 1) ==
      -1) {
    errorPrint("mq_send failed");
  }
}

void reportFailure() {
  char mq_buffer[MAX_MSG_SIZE];
  snprintf(mq_buffer, MAX_MSG_SIZE, "FAILED");
  if (mq_send(clientArgs->mqSendReport, mq_buffer, strlen(mq_buffer) + 1, 1) ==
      -1) {
    errorPrint("mq_send failed");
  }
}

void handleQuit() {
  debugPrint("Client: %s shutsdown.", clientArgs->name);

  if (mq_close(clientArgs->mqReceiveCMDS) == -1) {
    errorPrint("coudn't close mqeueu.");
    exit(EXIT_FAILURE);
  }
  reportSuccess();
  pthread_exit(NULL);
}

bool handleConnect(int16_t port) {
  debugPrint("Client: %s connecting to 127.0.0.1:%d", clientArgs->name, port);
  struct sockaddr_in server_addr;
  // char buffer[BUFFER_SIZE] = {0};

  clientArgs->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientArgs->socket == -1) {
    errorPrint("Error creating socket");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(STANDARD_PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    errorPrint("Invalid address/ Address not supported");
    close(clientArgs->socket);
    exit(EXIT_FAILURE);
  }

  if (connect(clientArgs->socket, (struct sockaddr*)&server_addr,
              sizeof(server_addr)) == -1) {
    errorPrint("Error connecting to server");
    close(clientArgs->socket);
    exit(EXIT_FAILURE);
  }
  normalPrint("Successfully connected to the server.\n");
  return true;
}

bool handleLogin() {
  bool checks = true;
  // build paket
  LGQ loginRequest = {.magic = htonl(MAGIC_NUMBER), .version = VERSION};
  strncpy(loginRequest.name, clientArgs->name, sizeof(loginRequest.name) - 1);
  paket_t paket = {.type = 0,
                   .length = htons(sizeof(loginRequest)),
                   .body = {.loginRequest = loginRequest}};

  // send it
  ssize_t bytes_sent =
      send(clientArgs->socket, (const char*)&paket, sizeof(paket), 0);
  if (bytes_sent == -1) {
    errorPrint("Error sending login request");
    close(clientArgs->socket);
    exit(EXIT_FAILURE);
  } else if (bytes_sent != sizeof(paket)) {
    errorPrint("Warning: Sent only %zd out of %zu bytes for login request.\n",
               bytes_sent, sizeof(paket));
  }

  // receive loginResponse
  char buffer[BUFFER_SIZE];
  ssize_t bytesReceived = recv(clientArgs->socket, &buffer, sizeof(Head), 0);
  Head receivedPaketHead;
  memcpy(&receivedPaketHead, buffer, sizeof(receivedPaketHead));
  receivedPaketHead.length = ntohs(receivedPaketHead.length);
  debugPrint("client received: %d and type: %d and we will need to receiv: %d",
             bytesReceived, receivedPaketHead.type, receivedPaketHead.length);

  if (receivedPaketHead.type == 1) {  // type 1: LRE
    LRE loginResponse;
    memset(buffer, 0, sizeof(buffer));
    bytesReceived =
        recv(clientArgs->socket, &buffer, receivedPaketHead.length, 0);
    memcpy(&loginResponse, buffer, sizeof(loginResponse));
    loginResponse.magic = ntohl(loginResponse.magic);
    debugPrint("client received body: %d and code: %d and magic: %x",
               bytesReceived, loginResponse.code, loginResponse.magic);

    // checks

    if (loginResponse.code != 0) {
      errorPrint("code is: %d but should be: 0", loginResponse.code);
      checks = false;
    }
    partSuccesPrint("code is correct");
    if (loginResponse.magic != 0xc001c001) {
      errorPrint("magic number is: %X but should be: %X", loginResponse.magic,
                 0xc001c001);
      checks = false;
    }
    partSuccesPrint("magic number is correct");
    if (strcmp(loginResponse.sName, "ChatServerG05") != 0) {
      errorPrint("server name is: %s but should be: %s", loginResponse.sName,
                 "ChatServerG05");
      checks = false;
    }
    partSuccesPrint("client-%s is logged into: %s", clientArgs->name,
                    loginResponse.sName);
  }
  if (checks) {
    testSuccesPrint("Login succesfull");
  }
  return checks;
}

void* client(void* arg) {
  clientArgs = (ClientArgs*)arg;
  debugPrint("starting client: %s", clientArgs->name);
  // establish mqueue
  mqd_t mqReceiveCMDS = mq_open(clientArgs->qName, O_RDONLY);
  if (mqReceiveCMDS == (mqd_t)-1) {
    errorPrint("client-%s: mqReceiveCMDS not openend.", clientArgs->name);
    exit(EXIT_FAILURE);
  }
  debugPrint("client %s opened Mqeueu", clientArgs->name);

  while (1) {
    unsigned int prio;
    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_read = mq_receive(mqReceiveCMDS, buffer, MAX_MSG_SIZE, &prio);
    if (bytes_read == -1) {
      errorPrint("receiv failed.");
      exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    bool checks = true;
    switch (parseCmd(buffer)) {
      case CONNECT:
        checks = handleConnect(STANDARD_PORT);
        break;
      case LOGIN:
        checks = handleLogin();
        break;
      case QUIT:
        handleQuit();
        break;
      case INVALID:
        errorPrint("Mq received an invalid command.");
        break;

      default:
        errorPrint("Hier hab ich mein Auto nicht geparkt.");
        break;
    }
    if (checks) reportSuccess();
  }
  if (mq_close(mqReceiveCMDS) == -1) {
    errorPrint("client coudn't close mqeueu.");
    exit(EXIT_FAILURE);
  }

  pthread_exit(NULL);
}