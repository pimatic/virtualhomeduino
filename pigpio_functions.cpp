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
