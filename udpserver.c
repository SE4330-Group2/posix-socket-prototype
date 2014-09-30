#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>

int createSocket(struct addrinfo * addr)
{
  printf("Creating new socket with given address info\n");

  int sock = socket(addr->ai_family,addr->ai_socktype,addr->ai_protocol);
  if (sock<0)
  {
    printf("Error occurred creating socket so now I am sad");
    return -1;
  }

  printf("Successfully created socket: %ld\n", sock);
  return sock;
}

struct addrinfo * createAddressInfo(const char* address, const char* port)
{
  printf("Creating address info for %s:%s\n", address, port);

  int n;
  struct addrinfo hints, *res;
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags=AI_PASSIVE;
  hints.ai_family= AF_UNSPEC;
  hints.ai_socktype= SOCK_DGRAM;
  hints.ai_protocol=IPPROTO_UDP;

  if((n = getaddrinfo(address, port, &hints, &res)) !=0)
  {
    printf("Error creating address for %s: %s",port,gai_strerror(n));
    return NULL;
  }
  printf("Successfully created address info.\n");

  return res;
}

int main(int argc, char **argv)
{
  int socket;
  socklen_t addrlen, len;
  struct sockaddr *cliaddr;
  struct addrinfo *res, *ressave;

  res = createAddressInfo(NULL, args->portRx);

  ressave=res;

  socket=createSocket(res);

  bind(socket, res->ai_addr, res->ai_addrlen);

  if(&addrlen)
    addrlen=res->ai_addrlen;

  freeaddrinfo(ressave);

  cliaddr=malloc(addrlen);

  len=addrlen;

  while ( 1 ) /* do forever */
  {     
    int rc;
    int foo = 1;
    int *foo2 = &foo;
    if ((rc=recvfrom(socket, foo2, sizeof(foo), 0, cliaddr, &len)) < 0 ) {
      printf("server error: errno %d\n",errno);
      perror("reading datagram");
      return -1;
    }
    printf("I am RX and I got a %d\n", *foo2);
  }
  close(socket);
  return 0;
} 
