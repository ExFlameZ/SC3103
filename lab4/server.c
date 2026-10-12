//************************************************************
//   server.c   SC3103 Exercise #4 - Part 3(ii)
//   Runs on the RPi. Waits for clients; for each client it
//   spawns a thread that reads a number, multiplies it by 5,
//   and sends the result back.
//
//   Compile: gcc -o server server.c -lpthread
//   Run:     ./server 12345
//************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>

sem_t slots;

void error(char *m)
{
    perror(m);
    exit(1);
}

/* Thread function: serves ONE client on its own socket */
void *process(void *arg)
{
    int sock = (int)(long)arg;          // recover the socket fd passed in
    char buffer[256];
    int n, num;

    memset(buffer, 0, sizeof(buffer));
    n = read(sock, buffer, 255);        // receive the number (as text)
    if (n < 0) {
        perror("ERROR reading from socket");
        close(sock);
	sem_post(&slots);
        return NULL;
    }
    sleep(2);
    num = atoi(buffer);                 // text -> integer
    printf("Received %d from client, replying %d\n", num, num * 5);

    snprintf(buffer, sizeof(buffer), "%d x 5 = %d", num, num * 5);
    n = write(sock, buffer, strlen(buffer) + 1);   // +1 sends the '\0' too
    if (n < 0)
        perror("ERROR writing to socket");

    close(sock);             
    sem_post(&slots);
           // done with this client
    return NULL;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, port, opt = 1;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t tid;
    sem_init(&slots, 0, 3);

    if (argc < 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    /* 1. Create a TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* Lets you restart the server immediately without
       "Address already in use" errors */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* 2. Fill in this server's address and bind the socket to it */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;            // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;    // accept on any local IP
    serv_addr.sin_port = htons(port);          // host -> network byte order

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR binding to socket");

    /* 3. Tell the OS we're ready to accept connections */
    listen(sockfd, 5);
    printf("Server listening on port %d...\n", port);

    /* 4. Accept clients forever; one thread per client */
    while (1) {
        sem_wait(&slots);
	clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
	    sem_post(&slots);
            continue;
        }

        if (pthread_create(&tid, NULL, process, (void *)(long)newsockfd) != 0) {
            perror("ERROR creating thread");
            close(newsockfd);
	    sem_post(&slots);
            continue;
        }
        pthread_detach(tid);   // thread cleans itself up when it ends
	int freeslots;
	sem_getvalue(&slots, &freeslots);
	printf("New client accepted, free slots = %d\n", freeslots);
	fflush(stdout);
    }

    close(sockfd);
    return 0;
}
