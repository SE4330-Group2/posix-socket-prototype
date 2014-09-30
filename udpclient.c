#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

typedef struct arguments
{
    char* ipTx;
    char* ipRx;
    char* portTx; 
    char* portRx;
    int rateTx;
} arguments;


extern int errno;

struct arguments * parseArgs(int argc, char **argv)
{
  if(argc != 6)
  {
    printf("usage: posix-socket <tx hostname> <rx hostname> <tx port> <rx port> <tx rate>\n");
    return NULL;
  }
  char* pEnd;
  
  arguments args = {
	.ipTx = argv[1],
	.ipRx = argv[2],
	.portTx = argv[3],
	.portRx = argv[4],
	.rateTx = strtol(argv[5], &pEnd, 10)
  };
  
  return &args;
}

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

int main (int argc, char **argv)
{
  int socket;
  socklen_t salen;
  struct sockaddr *sa;
  struct addrinfo *res, *ressave;
  int count = 20;

  arguments * args = parseArgs(argc, argv);

  res = createAddressInfo(args->ipTx, args->portTx);

  ressave=res;

  socket=createSocket(res);

  sa=malloc(res->ai_addrlen);
  memcpy(sa, res->ai_addr, res->ai_addrlen);
  salen=res->ai_addrlen;

  freeaddrinfo(ressave);

  for ( int i = 0; i < count; i++)
  {
    printf("I am TX and I am going to send a %d\n", i);

    if( sendto(socket,&i,sizeof(i),0,sa, salen) < 0 )
    {
      if (errno == ENOBUFS)
        continue;
      perror("sending datagram");
      return -1;
    }
  }
  close(socket);
  return 0;
}
