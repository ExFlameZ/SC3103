#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int g_var = 0;

void *inc_gv()
{
	int i,j;
	for (i=0;i<10;i++)
	  {g_var++;
	  for (j=0; j<5000000; j++);
	  printf("%d\n",g_var);
	  fflush(stdout);
	}
}

int main()
{
    pthread_t TA, TB;
    int TAret, TBret;

    /* Create two threads, both running inc_gv() */
    TAret = pthread_create(&TA, NULL, inc_gv, NULL);
    TBret = pthread_create(&TB, NULL, inc_gv, NULL);

    /* Wait for both threads to finish */
    pthread_join(TA, NULL);
    pthread_join(TB, NULL);

    printf("pthread2 completed \n");
    return 0;
}
