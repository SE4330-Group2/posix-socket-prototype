posix-socket-prototype
======================

Datagrams and threads in Linux using POSIX standards

##Requirements

  1. Must be written in C
  2. Transmits and receives datagrams
  2. Payload of the datagram is an integer (4 bytes)
  2. Make 2 pthreads, one for the [server](#transmiter-task) and one for the [receiver](#reciever-task)
  2. App reads parameters from command line as follows:

    ```
a.out ipTx ipRx portTx portRx TxRateMs
    ```
  2. Main can join on the two threads and stop after they are done. Print out a nice done message
  2. Use [ntohl](http://linux.die.net/man/3/ntohl) and [htonl](http://linux.die.net/man/3/htonl) as discussed in class
  3. Use only POSIX functions listed in the FACE standard
  4. Use ports 7082 and 7083 for testing. IO ip address is 137.104.21.4

###Transmiter Task

  2. Wait (sleep) 5 seconds after start in TX task
  2. Send 20 messages (make a constant so I can ask you to change it if need be)
  3. Increment the int after each send
  2. Echoes the message it is going to send to standard output

    ```
I am TX and I am going to send a 9
    ```
    
  2. Wait 10 seconds after last message sent.

###Reciever Task
  2. Prints the message it receives (newline after it) to standard output
    
    ```
I am RX and I got a 13
    ```
    
  2. Stop after 20 seconds total (use timed version of [recv](http://linux.die.net/man/3/recv))






