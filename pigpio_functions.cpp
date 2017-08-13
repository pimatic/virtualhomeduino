#include <sys/stat.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
int _hw_interrupt_pin = -1;
uint32_t _lastmic = 0;
void (*_hw_interruptCallback)(void) = 0;
void nsleep(long us)
{
    struct timespec wait;
    //printf("Will sleep for is %ld\n", diff); //This will take extra ~70 microseconds
    
    wait.tv_sec = us / (1000 * 1000);
    wait.tv_nsec = (us % (1000 * 1000)) * 1000;
    nanosleep(&wait, NULL);
}
