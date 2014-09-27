#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

const int NUM_MESSAGES = 20;
const int INIT_WAIT = 5;
const int END_WAIT = 10;

void *TX_task(void *txDelay)
{
    int msgNum = 0;
    int *transDelay = (int*)txDelay;
    struct timespec *delay;
    delay->tv_sec = (time_t)(*transDelay/1000);
    delay->tv_nsec = (*transDelay%1000) * 1000000;
    sleep(INIT_WAIT);

    while( msgNum < NUM_MESSAGES )
    {
        nanosleep(delay, NULL);
        printf("I am TX transmitting message number %ld\n", ++msgNum);
    }

    sleep(END_WAIT);
    printf("I am in TX\n");

    return NULL;
}

void *RX_task(void *param)
{
    int wait = 0;
    int i = 0;
    sleep(5);
    while( wait < 200 )
    {
        wait++;
        printf("In RX %ld\n", i++);
    }
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
    
    printf("Transmit IP: %s\n", ipTx);
    printf("Receive IP: %s\n", ipRx);
    printf("Transmit Port: %ld\n", portTx);
    printf("Receive Port: %ld\n", portRx);
    printf("Transport Rate: %ld\n", rateTx);

    pthread_t thread0, thread1;

    if( pthread_create(&thread0, NULL, TX_task, &rateTx))
    {
        perror("Couldn't create thread!");
        return 1;
    }

    if( pthread_create(&thread1, NULL, RX_task, NULL))
    {
        perror("Couldn't create thread!");
        return 1;
    }

    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    
    printf("I am out of TX\n");
    return 0;
}
