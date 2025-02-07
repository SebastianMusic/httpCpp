#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

void sendHtml(int fd, std::string path) {
  std::string httpHeader = "HTTP/1.1 200 OK\r\n"
                           "Content-Length: CONTENTLENGTHREPLACE\r\n"
                           "Content-Type: text/html\r\n"
                           "Connection: close\r\n"
                           "\r\n";
  // calculate the size of the html file
  char sendBuffer[1000];
  struct stat statBuffer{};

  int htmlFd =
      open(path.c_str(), O_RDONLY, 0); // convert std::string to c style string
  if (htmlFd < 0) {
    perror("open failed in sendhtml");
    std::cout << path.c_str();
  }
  if (fstat(htmlFd, &statBuffer) < 0) {
    perror("fstat failed in sendHtml()");
  }
  size_t fileSize = statBuffer.st_size;
  std::string fileSizeString = std::to_string(fileSize);

  size_t pos = httpHeader.find("CONTENTLENGTHREPLACE");

  if (pos != std::string::npos) {
    httpHeader.replace(pos, std::strlen("CONTENTLENGTHREPLACE"),
                       fileSizeString);
  }
  size_t httpHeaderLen = httpHeader.length();
  memcpy(sendBuffer, httpHeader.c_str(), httpHeaderLen);

  if (read(htmlFd, sendBuffer + httpHeaderLen, fileSize) < 0) {
    perror("sendHtml failed in read");
  }
  if (write(fd, sendBuffer, httpHeaderLen + fileSize) < 0) {
    perror("sendHtml failed in write");

    std::cout << sendBuffer;
  }
}

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

  sendHtml(clientSocket, "/home/sebastian/kode/cpp/index.html\0");
  return 0;
}
