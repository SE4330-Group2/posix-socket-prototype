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


const int NUM_MESSAGES = 20;
const int INIT_WAIT = 5;
const int END_WAIT = 10;

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
}

int createSocket(struct addrinfo * addr)
{
  int sock = socket(addr->ai_family,addr->ai_socktype,addr->ai_protocol);
  if (sock<0)
  {
    perror("Error creating socket");
    return -1;
  }
  
  return sock;
}

struct addrinfo * createAddressInfo(const char* address, const char* port)
{
  int n;
  struct addrinfo hints, *res;
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags=AI_PASSIVE;
  hints.ai_family= AF_UNSPEC;
  hints.ai_socktype= SOCK_DGRAM;
  hints.ai_protocol=IPPROTO_UDP;

  if((n = getaddrinfo(address, port, &hints, &res)) !=0)
  {
    perror("Error creating address");
    return NULL;
  }
 
  return res;
}

void *TX_task(void *a)
{
  int socket;
  socklen_t salen;
  struct sockaddr *sa;
  struct addrinfo *res, *ressave;
  struct timespec delay;
  arguments * args = a;
  delay.tv_sec = (time_t)(args->rateTx/1000);
  delay.tv_nsec = (args->rateTx%1000) * 1000000;

  ressave = res = createAddressInfo(args->ipTx, args->portTx);
  socket=createSocket(res);

  sleep(INIT_WAIT);
  sa=malloc(res->ai_addrlen);
  memcpy(sa, res->ai_addr, res->ai_addrlen);
  salen=res->ai_addrlen;

  freeaddrinfo(ressave);

  int i;
  for ( i = 0; i < NUM_MESSAGES; i++)
  {
    printf("I am TX and I am going to send a %d\n", i);

    long message = htonl(i);
    if( sendto(socket,&message,sizeof(message),0,sa, salen) < 0 )
    {
      perror("Error sending datagram");
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
  struct timeval timeout;
  arguments * args = a;
  
  ressave = res = createAddressInfo(args->ipRx, args->portRx);
  socket=createSocket(res);

  timeout.tv_sec = 20;
  timeout.tv_usec = 0;

  if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
		 sizeof(timeout))< 0)
      perror("Set socket option failed\n");

  if (setsockopt (socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
		  sizeof(timeout)) < 0)
      perror("Set socket option failed\n");

  bind(socket, res->ai_addr, res->ai_addrlen);

  if(&addrlen)
    addrlen=res->ai_addrlen;

  freeaddrinfo(ressave);
  cliaddr=malloc(addrlen);
  len=addrlen;

  while ( 1 ) /* do forever */
  {     
    int received = 1;
    if (recvfrom(socket, &received, sizeof(received), 0, cliaddr, &len) < 0 ) {
      return NULL;
    }
    long message = ntohl(received);
    printf("I am RX and I got a %d\n", message);
  }
  close(socket);
  return NULL;
}

int main(int argc, char **argv)
{
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
    
    pthread_join(thread_tx, NULL);
    pthread_join(thread_rx, NULL);
    
    printf("a nice done message\n");

    return 0;
}
