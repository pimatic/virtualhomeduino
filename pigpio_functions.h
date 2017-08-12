#ifndef _WIRING_X_FUNCTIONS_H_
#define _WIRING_X_FUNCTIONS_H_

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
	gpioDelay(time_to_wait);
}

static inline void hw_pinMode(uint8_t pin, uint8_t mode){
//	gpioSetMode(pin, mode);
}

static inline void hw_digitalWrite(uint8_t pin, uint8_t value){
	gpioWrite(pin, value);
}

#endif
