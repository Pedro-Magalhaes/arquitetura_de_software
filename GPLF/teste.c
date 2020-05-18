#include <stdio.h>
#include <math.h>
#include <pthread.h>

void* function(void* x)
{
    double *y = x;

    *y = sqrt(*y);

    return x;
}

int main(void)
{
    pthread_t thread;
    double arg = 123;
    void *retVal = NULL;

    pthread_create(&thread, NULL, function, &arg);

    pthread_join(thread, &retVal);
    printf("Sqrt of our argument using arg   : %f\n", arg);

    if (retVal != NULL)
    {
        printf("Sqrt of our argument using retVal: %f\n", *((double *)retVal));
    }

    return 0;
}