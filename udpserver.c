/*
 * udpserver.c
 *
 * This is an example TCP/IP UDP socket server.
 * It will read packets sent to 'portno' and write them back
 * to the sender.  It has no reliability functions.
 * It will loop forever and must be killed in order to make
 * it terminate.
 *
 * Protocol independant( working for both IPV4 and IPV6)
 * syntax:
 *      % udpserver portno &
 *
 * Start the server first and then start the udpclient.c app to
 * send packets to it.  Use the debug switch with udpclient.c to
 * see if any of the packets disappear.  They won't disappear on
 * localhost, but they might go away if they are crossing a busy 
 * router.
 */
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>

#define MAXBUF	10 * 1024

int createSocket(struct addrinfo * addr)
{
    printf("Creating new socket with given address info\n");
    
    int sock = socket(addr->ai_family,addr->ai_socktype,addr->ai_protocol);
    if (sock<0) {
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

    if((n = getaddrinfo(address, port, &hints, &res)) !=0) {
        printf("udpserver error for %s: %s",port,gai_strerror(n));
		return NULL;
    }
    printf("Successfully created address info.\n");

    return res;
}

int main(int argc, char **argv)
{
  int sockfd;
  ssize_t m;
  socklen_t addrlen, len;
  struct sockaddr *cliaddr;
  char* port;
  struct addrinfo *res, *ressave;
  int current = 0;

 if(argc== 2){
   port=argv[1];
 }
 else
 {  printf("usage: udpserver <portnumber>\n");
    return 0;
 }  

 
 res = createAddressInfo(NULL, port);

 ressave=res;

 sockfd=createSocket(res);

 bind(sockfd, res->ai_addr, res->ai_addrlen);
       
 if(&addrlen)
   addrlen=res->ai_addrlen;
 
 freeaddrinfo(ressave);

 cliaddr=malloc(addrlen);

 len=addrlen;

 while ( 1 ) {     /* do forever */
	int rc;
        int foo = 1;
        int *foo2 = &foo;
	if ((rc=recvfrom(sockfd, foo2, MAXBUF, 0, cliaddr, &len)) < 0 ) {
		printf("server error: errno %d\n",errno);
		perror("reading datagram");
		return -1;
	}
        printf("I am RX and I got a %d\n", *foo2);
}

/* can't get here, but just in case: close sockets
*/
close(sockfd);
return 0;

} 
