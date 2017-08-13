#include <sys/stat.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "rfcontrol/RFControl.h"


extern "C" {
//#include "pigpio/pigpiod_if2.h"
#include "pigpio/pigpio.h"
}

#define MAX_RECORDINGS 4096
#define OUTPUT PI_OUTPUT
#define LOW PI_LOW
extern int _hw_interrupt_pin;
extern long _lastmic;
extern void (*_hw_interruptCallback)(void);
static inline int hw_getInterruptPin() {
	return _hw_interrupt_pin;
}

static inline void hw_callInterrupt(uint32_t tick) {
		_lastmic = tick;
		_hw_interruptCallback();
}



static inline void hw_detachInterrupt(uint8_t){
	_hw_interrupt_pin = -1;
	_hw_interruptCallback = 0;
}

static inline unsigned long hw_micros(void) {
	return _lastmic;
}
static inline void hw_delayMicroseconds(unsigned long time_to_wait) {
	uint32_t delay=gpioDelay(time_to_wait);
	uint32_t diff = delay - time_to_wait;
//       fprintf(stderr,"Sleeping for %lu\n",time_to_wait);
//       time_to_wait=time_to_wait*1000;
//          fprintf(stderr,"GPIO gpioDelay=%d\trequested_delay=%lu\tdiff=%d\n",delay,time_to_wait,diff);
//usleep(time_to_wait);
// struct timespec wait;
//    wait.tv_sec = time_to_wait / (1000 * 1000);
//    wait.tv_nsec = (time_to_wait % (1000 * 1000)) * 1000;
//    nanosleep(&wait, NULL);
}

static inline void hw_pinMode(uint8_t pin, uint8_t mode){
//	fprintf(stderr,"Setting pin %d to %d\n",pin,mode);
	if (gpioSetMode(pin, mode) != 0) {
                fprintf(stderr,"GPIO set mode error\n");
        }

}

static inline void hw_digitalWrite(uint8_t pin, uint8_t value){
	if (gpioWrite(pin, value) == 0) {
//        fprintf(stderr,"Writing %d to %d\n",value,pin);
	} else {
		fprintf(stderr,"GPIO write error\n");
	}
}
