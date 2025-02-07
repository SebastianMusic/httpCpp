#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

struct clientMessage {
  char msg[100];
};

int main() {
  clientMessage msg;

  const char *httpHeader = "HTTP/1.1 200 OK\r\n"
                           "Content-Length: 6\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "\r\n";

  const char *httpContent = "halla\n";

  size_t httpHeaderLen = strlen(httpHeader);
  size_t httpContentLen = strlen(httpContent);
  size_t totalLen = httpHeaderLen + httpContentLen;

  memcpy(msg.msg, httpHeader,
         httpHeaderLen); // copies only specified memory no extra nullbyte
                         // padding like strncpy
  memcpy(msg.msg + httpHeaderLen, httpContent, httpContentLen);

  struct sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "server socket file descriptor is " << serverSocket << std::endl;
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) ==
      -1) {

    perror("bind failed");
    return 1;
  }

  if (listen(serverSocket, 128) < 0) {
    perror("listen failed");
  }

  int clientSocket = accept(serverSocket, 0, 0);

  write(1, &msg, totalLen);
  write(clientSocket, &msg, totalLen);

  return 0;
}
