#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "shared.h"

void error(char *msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  
  struct sockaddr_un svaddr, claddr;
  int sfd, j;
  size_t msgLen;
  ssize_t numBytes;
  char resp[BUF_SIZE];

  /* Create client socket; bind to unique pathname (based on PID) */
 
  sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sfd == -1) {
    std::cout << "Error on socket" << std::endl;
    exit (0);
  }
  
  memset(&claddr, 0, sizeof(struct sockaddr_un));
  claddr.sun_family = AF_UNIX;
  snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long) getpid());

  if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1) {
    std::cout << "Error on bind" << std::endl;
    exit(0);
  }
  
  /* Construct address of server */
  
  memset(&svaddr, 0, sizeof(struct sockaddr_un));
  svaddr.sun_family = AF_UNIX;
  strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);
  
  /* Send messages to server; echo responses on stdout */
  std::vector messages = {
    "hi",
    "hello",
    "hello world"
  };

  int counter = 0;
  while (true) {
    counter++;
    for (j = 0; j < messages.size(); j++) {
      std::string message = messages[j] + std::string("_") + std::to_string(counter);
      msgLen = message.size();       /* May be longer than BUF_SIZE */
      if (sendto(sfd, message.c_str(), msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen) {
        std::cout << "fatal error on sendto" << std::endl;
      }
      
      numBytes = recvfrom(sfd, resp, BUF_SIZE, MSG_WAITALL, NULL, NULL);
      /* Or equivalently: numBytes = recv(sfd, resp, BUF_SIZE, 0);
       or: numBytes = read(sfd, resp, BUF_SIZE); */
      if (numBytes == -1) {
        std::cout << "Error on recvfrom" << std::endl;
      }
      printf("Response %d: %.*s\n", j, (int) numBytes, resp);
      std::this_thread::sleep_for(std::chrono::milliseconds(300));

    }
  }
  
  remove(claddr.sun_path);            /* Remove client socket pathname */
  
  return EXIT_SUCCESS;
  
  
}
