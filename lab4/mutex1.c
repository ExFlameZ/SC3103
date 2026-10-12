//************************************************************
//   pthread2.c   SC3103 Exercise #4 - Practice B
//   Two threads increment a shared global variable without
//   synchronisation, demonstrating a race condition.
//************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int g_var1 = 0;
pthread_mutex_t mutexA;                         // global variable (shared)

void *inc_gv(void *arg)
{
    int i, j;
    for (i = 0; i < 10; i++)
    {   pthread_mutex_lock(&mutexA);
        g_var1++;                          // increment the global variable
        for (j = 0; j < 5000000; j++);  // delay loop
        printf(" %d", g_var1);          // print the value
        fflush(stdout);
	pthread_mutex_unlock(&mutexA);
    }
    return NULL;
}

int main()
{
    pthread_t TA, TB;
    int TAret, TBret;
    pthread_mutex_init(&mutexA, NULL);

    /* Create two threads, both running inc_gv()*/
    TAret = pthread_create(&TA, NULL, inc_gv, NULL);
    TBret = pthread_create(&TB, NULL, inc_gv, NULL);

    /* Wait for both threads to finish */
    pthread_join(TA, NULL);
    pthread_join(TB, NULL);

    printf("pthread2 completed \n");
    return 0;
}
