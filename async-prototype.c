#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>


const int NUM_MESSAGES = 5; //20
const int INIT_WAIT = 1; //5
const int END_WAIT = 2; //10

typedef struct arguments
{
    char* ipTx;
    char* ipRx;
    char* portTx; 
    char* portRx;
    int rateTx;
} arguments;

void parseArgs(int argc, char **argv, arguments * args)
{
  if(argc != 6)
  {
    printf("usage: posix-socket <tx hostname> <rx hostname> <tx port> <rx port> <tx rate>\n");
    return;
  }
  char* pEnd;
  
  args->ipTx = argv[1];
  args->ipRx = argv[2];
  args->portTx = argv[3];
  args->portRx = argv[4];
  args->rateTx = strtol(argv[5], &pEnd, 10);

  printf("ipTx: %s\n", args->ipTx);
  printf("ipRx: %s\n", args->ipRx);
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

void *TX_task(void *args)
{
  int socket;
  socklen_t salen;
  struct sockaddr *sa;
  struct addrinfo *res, *ressave;
  int count = 20;
  struct timespec delay;
  delay.tv_sec = (time_t)(args->rateTx/1000);
  delay.tv_nsec = (args->rateTx%1000) * 1000000;
  sleep(INIT_WAIT);

  ressave = res = createAddressInfo(args->ipTx, args->portTx);

  socket=createSocket(res);

  sa=malloc(res->ai_addrlen);
  memcpy(sa, res->ai_addr, res->ai_addrlen);
  salen=res->ai_addrlen;

  freeaddrinfo(ressave);

  int i;
  for ( i = 0; i < count; i++)
  {
    printf("I am TX and I am going to send a %d\n", i);

    if( sendto(socket,&i,sizeof(i),0,sa, salen) < 0 )
    {
      if (errno == ENOBUFS)
        continue;
      perror("sending datagram");
      return NULL;
    }
	
	nanosleep(&delay, NULL);
  }
  close(socket);
  sleep(END_WAIT);
  return NULL;
}

void *RX_task(void *a)
{
  int socket;
  socklen_t addrlen, len;
  struct sockaddr *cliaddr;
  struct addrinfo *res, *ressave;

  arguments * args = a;
  
  ressave = res = createAddressInfo(args->ipRx, args->portRx);

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
      return NULL;
    }
    printf("I am RX and I got a %d\n", *foo2);
  }
  close(socket);
  return NULL;
}

int main(int argc, char **argv)
{
    char* pEnd;
    char str[15];
    char* ipTx = argv[1];
    char* ipRx = argv[2];
    int portTx = strtol(argv[3], &pEnd, 10);
    int portRx = strtol(argv[4], &pEnd, 10);
    int rateTx = strtol(argv[5], &pEnd, 10);
    
    int* foo = &rateTx;
    
    printf("Transmit IP: %s\n", ipTx);
    printf("Receive IP: %s\n", ipRx);
    printf("Transmit Port: %ld\n", portTx);
    printf("Receive Port: %ld\n", portRx);
    printf("Transport Rate: %ld\n", rateTx);

    pthread_t thread_tx, thread_rx;
    
    arguments args;	
    parseArgs(argc, argv, &args);

    if( pthread_create(&thread_tx, NULL, TX_task, &args))
    {
        perror("Couldn't create thread!");
        return 1;
    }

    if( pthread_create(&thread_rx, NULL, RX_task, &args))
    {
        perror("Couldn't create thread!");
        return 1;
    }
    
    printf("I am waiting for TX\n");

    pthread_join(thread_tx, NULL);
    
    printf("I am out of TX\n");

    pthread_join(thread_rx, NULL);
    
    printf("I am out of RX\n");
    
    printf("a nice done message\n");
    

    return 0;
}
