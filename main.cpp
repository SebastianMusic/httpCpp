#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
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

  return 0;
}
