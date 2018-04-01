#include<iostream>

using namespace std;

typedef int (*CallbackType)(int);

int doSomething(CallbackType callback)
{
    return callback(10);
}

int callbackFunction(int var)
{
    cout << "I am the Callback Function; var = " << var << endl;
    return 2 * var;
}

int main()
{
    cout << "Hello Beaglebone" << endl;

    int y = doSomething(&callbackFunction);
    cout << "Value of y is: " << y << endl;

    return 0;
}