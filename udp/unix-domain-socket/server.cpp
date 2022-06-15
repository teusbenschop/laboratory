/* ud_ucase_sv.c
 
 A server that uses a UNIX domain datagram socket to receive datagrams,
 and then return them to the senders.
 
 See also ud_ucase_cl.c.
 */
#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <arpa/inet.h>

void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  
  struct sockaddr_un svaddr, claddr;
  int sfd, j;
  ssize_t numBytes;
  socklen_t len;
  char buf[BUF_SIZE];
  
  sfd = socket(AF_UNIX, SOCK_DGRAM, 0);       /* Create server socket */
  if (sfd == -1) {
    std::cout << "error on socket" << std::endl;
    exit(0);
  }
  
  /* Construct well-known address and bind server socket to it */
  
  /* For an explanation of the following check, see the erratum note for
   page 1168 at http://www.man7.org/tlpi/errata/. */
  
  if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) - 1) {
    std::cout << "Server socket path too long: " << SV_SOCK_PATH << std::endl;
    exit(0);
  }
  
  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
    std::cout << "Error on remove " << SV_SOCK_PATH << std::endl;
    exit(0);
  }
  
  memset(&svaddr, 0, sizeof(struct sockaddr_un));
  svaddr.sun_family = AF_UNIX;
  strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1) {
    std::cout << "Error on bind" << std::endl;
    exit(0);
  }
  
  /* Receive messages, convert to uppercase, and return to client */
  
  for (;;) {
    len = sizeof(struct sockaddr_un);
    numBytes = recvfrom(sfd, buf, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &claddr, &len);
    if (numBytes == -1) {
      std::cout << "Error on recvfrom" << std::endl;
      exit(0);
    }

    buf[numBytes] = '\0';
//    std::cout << "Received " << numBytes << " bytes from " << claddr.sun_path << std::endl;
    std::cout << buf << std::endl;
    
    if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) != numBytes) {
      std::cout << "fatal error on sendto" << std::endl;
    }
  }
  
  return EXIT_SUCCESS;
}
