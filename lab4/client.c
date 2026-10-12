//************************************************************
//   client.c   SC3103 Exercise #4 - Part 3(i)
//   Runs on the Ubuntu PC. Asks the user for a number, sends
//   it to the server on the RPi, and prints the reply.
//
//   Compile: gcc -o client client.c
//   Run:     ./client 192.168.3.1 12345
//************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *m)
{
    perror(m);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, port, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage: %s [hostname] [port]\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[2]);

    /* 1. Create a TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* 2. Look up the server's address (works for names or IPs) */
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    /* 3. Fill in the server's address and port */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    /* 4. Connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /* 5. Get a number from the user and send it */
    printf("Please enter a number: ");
    fgets(buffer, 255, stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");

    /* 6. Read the server's reply and display it */
    memset(buffer, 0, sizeof(buffer));
    n = read(sockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Server replied: %s\n", buffer);

    close(sockfd);
    return 0;
}
