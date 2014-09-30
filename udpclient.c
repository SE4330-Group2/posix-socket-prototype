/*
* udpclient.c
*
* OS: FreeBSD
* compiler: cc
*      To compile: cc -o udpclient -g udpclient.c
*
* Protocol Independant (working for both IPV4 and IPV6)
*
* udpclient assumes udpserver.c is running first.
* It sends 'count' packets to the server which in turn writes
* them back to the client; i.e., the server provides an echo service.
*
* To run:
*      1. first run udpserver on some udp port that is not used.
*      (you can make sure ports are not used with %netstat -a | grep udp)
*
*      % udpserver 10000&
*
*      2. then run the client
*
*      %udpclient localhost 10000 512
*      %udpclient mchugh.cs.pdx.edu 10000 512
*      %udpclient 131.252.215.20 10000 512
*      %udpclient 2001:468:1f04:2f0:250:daff:fe83:d48c 10000 512
*
* syntax:
*      % udpclient remote_host udp_port number_of_packets_to_echo
*
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

extern int errno;

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
  /*initilize addrinfo structure*/ 
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

int main (int argc, char **argv){

  int sockfd, rc;
  socklen_t salen;
  struct sockaddr *sa;
  struct addrinfo *res, *ressave;
  char *host, *port, *count;
  int i;
  int fromlen;

  if(argc != 4)
  {
    printf("usage: tcpclient <hostname/IPaddress> <portnumber> <count> \n");
    return 0;
  }

  host=argv[1];
  port=argv[2];
  count=atoi(argv[3]);

  res = createAddressInfo(host, port);

  ressave=res;

  sockfd=createSocket(res);

  sa=malloc(res->ai_addrlen);
  memcpy(sa, res->ai_addr, res->ai_addrlen);
  salen=res->ai_addrlen;

  freeaddrinfo(ressave);

  for ( i = 0; i < count; i++) {

    printf("I am TX and I am going to send a %d\n", i);

    if( rc = sendto(sockfd,&i,sizeof(i),0,sa, salen) < 0 )
    {
      if (errno == ENOBUFS)
        continue;
      perror("sending datagram");
      return -1;
    }

  }/*end of for */ 
  close(sockfd);
  return 0;
}
