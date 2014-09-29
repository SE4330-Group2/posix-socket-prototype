#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct addrinfo * createAddressInfo(const char* address, const char* port)
{
    printf("Creating address info for %s:%s\n", address, port);
    
    struct addrinfo *addr;
    int err=getaddrinfo(address,port,NULL,&addr);
    if (err==0) {
        printf("Successfully created address info.\n");
        return addr;
    }
    
    printf("Error occurred in createAddressInfo with err=%ld.\n", err);

    return NULL;
}

int createSocket(struct addrinfo * addr)
{
    printf("Creating new socket with given address info\n");
    
    int sock = socket(addr->ai_family,addr->ai_socktype,addr->ai_protocol);
    if (sock==-1) {
        printf("Error occurred creating socket so now I am sad");
        return -1;
    }
    
    printf("Successfully created socket: %ld\n", sock);
    return sock;
}

int main(int argc, char **argv)
{
    char* pEnd;
    char str[15];
    
    const char* ipTx = argv[1];
    const char* ipRx = argv[2];
    const char* portTx = argv[3];
    const char* portRx = argv[4];
    const int rateTx = strtol(argv[5], &pEnd, 10);
    
    printf("Transmit IP: %s\n", ipTx);
    printf("Receive IP: %s\n", ipRx);
    printf("Transmit Port: %s\n", portTx);
    printf("Receive Port: %s\n", portRx);
    printf("Transport Rate: %ld\n", rateTx);
    
    struct addrinfo *addr = createAddressInfo(ipTx,portTx);
    
    int socket = createSocket(addr);
    
    int tmp = 3;
    int * buffer = &tmp;
    
    while(1)
    {
        printf("Sending\n");
        int success=sendto(socket,buffer,sizeof(buffer),0,addr->ai_addr,addr->ai_addrlen);
        if(success==-1) {
            printf("Error occurred sending so now I am sad");
            return -1;
        }
        sleep(1);
    }
    
    return 0;
}
