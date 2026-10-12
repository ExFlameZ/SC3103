//************************************************************
//   server_challenge.c   SC3103 Exercise #4 - Challenge
//   Runs on the RPi. Serves clients with one thread each, but
//   a counting semaphore limits it to at most 3 threads
//   (3 clients) at the same time.
//
//   Compile: gcc -o server_challenge server_challenge.c -lpthread
//   Run:     ./server_challenge 12345
//************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_THREADS 3

sem_t slots;                 // counts free thread slots (starts at 3)

void error(char *m)
{
    perror(m);
    exit(1);
}

/* Thread function: serves ONE client, then gives its slot back */
void *process(void *arg)
{
    int sock = (int)(long)arg;
    char buffer[256];
    int n, num, freeslots;

    memset(buffer, 0, sizeof(buffer));
    n = read(sock, buffer, 255);
    if (n > 0) {
        num = atoi(buffer);
        printf("[thread %lu] received %d, replying %d\n",
               (unsigned long)pthread_self(), num, num * 5);
        fflush(stdout);

        sleep(2);            // simulate work so you can SEE 3 threads at once

        snprintf(buffer, sizeof(buffer), "%d x 5 = %d", num, num * 5);
        write(sock, buffer, strlen(buffer) + 1);
    }

    close(sock);

    sem_post(&slots);        // CONSUMER side: release one slot
    sem_getvalue(&slots, &freeslots);
    printf("[thread %lu] done, free slots = %d\n",
           (unsigned long)pthread_self(), freeslots);
    fflush(stdout);
    return NULL;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, port, opt = 1, freeslots;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t tid;

    if (argc < 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    /* Semaphore: 0 = shared between threads (not processes),
       initial value 3 = three free slots */
    sem_init(&slots, 0, MAX_THREADS);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR binding to socket");

    listen(sockfd, 5);
    printf("Server listening on port %d (max %d threads)...\n", port, MAX_THREADS);
    fflush(stdout);

    while (1) {
        sem_wait(&slots);    // PRODUCER side: take a slot; blocks if all 3 busy

        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            sem_post(&slots);           // give the slot back
            continue;
        }

        if (pthread_create(&tid, NULL, process, (void *)(long)newsockfd) != 0) {
            perror("ERROR creating thread");
            close(newsockfd);
            sem_post(&slots);
            continue;
        }
        pthread_detach(tid);

        sem_getvalue(&slots, &freeslots);
        printf("New client accepted, free slots = %d\n", freeslots);
        fflush(stdout);
    }

    sem_destroy(&slots);
    close(sockfd);
    return 0;
}
