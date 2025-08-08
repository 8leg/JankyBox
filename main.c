#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3000 // just setting a constant value

int Socket(int domain, int type, int protocol) {
  // socket wrapper functiion
  int res = socket(domain, type, protocol);
  if (res == -1) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  return res;
}

int main(int argc, char const *argv[]) {
  int server_fd, new_socket; // server file descriptor and fuckass new socket
  ssize_t valread;           // some POS
  struct sockaddr_in
      address; // serious business. Making struct of type sockaddr_in named
               // address. I hate this syntax tho. It will be then cast into
               // sockaddr struct requred by bind
  int opt = 1;
  socklen_t addrlen = sizeof(address); // lenght of struct
  char buffer[1024] = {0};             // buffer

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) <
      0) { // if it is -1 - throwing error
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    // this func changes option of socket. This one sets it to reuse its address
    // to true passing our file descriptor as destined socket an then we pass
    // opt. opt just means true here SOL_SOCKET just means that we change
    // options on socket level
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  /*
 The <sys/socket.h> header defines the following macros, with distinct
  integral values:

  AF_UNIX
      UNIX domain sockets
  AF_UNSPEC
      Unspecified
  AF_INET
      Internet domain sockets
*/
  address.sin_addr.s_addr = INADDR_ANY;
  /*
  The <netinet/in.h> header shall define the following macros for use as
    destination addresses for connect(), sendmsg(), and sendto():

    INADDR_ANY
        IPv4 local host address.
    INADDR_BROADCAST
        IPv4 broadcast address.
  */
  address.sin_port = htons(
      PORT); // https://stackoverflow.com/questions/19207745/htons-function-in-socket-programing

  /* code */

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    // this shit binds "unnamed" socket to port number. That middle part is
    // where we convert our structure "address" of type sockaddr_in
    // into another type - sockaddr
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    // just fucking listen. The N parametr tells how many messages can be in
    // queue before socket stops accepting packages
    perror("listen");
    exit(EXIT_FAILURE);
  }
  while (true) {
    if ((new_socket =
             accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
      // accept makes it so when you listen to things with listen and soeone
      // tries to connect it will then create new fd (file descriptor) to be an
      // ID of that connection so you can manage many connections each with
      // different shit
      perror("accept");
      exit(EXIT_FAILURE);
    }

    ssize_t nread;
    char buf[256];
    nread = read(new_socket, buf, 256);
    // because in UNIX everything is a file, here we threat our connection -
    // socket, whatever - as a pipe, a file, a stream. So now we use generic
    // functions we use to read files to read from the stream that comes from
    // our connection
    if (nread == -1) {
      perror("read failed");
      exit(EXIT_FAILURE);
    }
    if (nread == 0) {
      printf("END OF FILE occured\n");
    }
    write(STDOUT_FILENO, buf, nread); // write out to STDOUT i suppose?
    buf[3] = '1';
    write(new_socket, buf,
          nread); // write out to our socket, meaning we just sent it all back.
                  // Echo servers, huh?

    // closing the connected socket
    close(new_socket);
  }

  // closing the listening socket
  close(server_fd);
  return 0;
}