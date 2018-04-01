#include<iostream>
#include<pthread.h>
#include<unistd.h>

using namespace std;

void * threadFunction(void * value)
{
    int * x = (int *)value;
    while (*x < 5)
    {
        usleep(1000);
        (*x)++;
    }
    return x;
}

int main()
{
    int x = 0, y = 0;
    pthread_t thread;

    if (pthread_create(&thread, NULL, &threadFunction, &x) != 0)
    {
        cout << "Failed to create the thread" << endl;
        return -1;  // exit program if thread not created
    }

    while (y < 5)
    {
        cout << "The value of x = " << x << " and y = " << y++ << endl;
        usleep(1000);
    }

    void * result;
    pthread_join(thread, &result);
    int * z = (int *)result;
    cout << "Final: x = " << x << ", y = " << y << " and z = " << *z << endl;

    return 0;
}