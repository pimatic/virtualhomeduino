#ifndef _WIRING_X_FUNCTIONS_H_
#define _WIRING_X_FUNCTIONS_H_

#include <sys/stat.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "wiringX/wiringX.h"
}

#define MAX_RECORDINGS 1024

extern int _hw_interrupt_pin;
extern void (*_hw_interruptCallback)(void);

static inline int hw_getInterruptPin() {
	return _hw_interrupt_pin;
}

static inline void hw_callInterrupt() {
	if(_hw_interrupt_pin != -1) {
		_hw_interruptCallback();
	}
}

static inline void hw_attachInterrupt(uint8_t interrput_pin, void (*ic)(void)) {
	fprintf(stderr, "attatch %d\n", interrput_pin);
	_hw_interruptCallback = ic;
	wiringXISR(interrput_pin, INT_EDGE_BOTH);
	_hw_interrupt_pin = interrput_pin;
}


static inline void hw_detachInterrupt(uint8_t){
	_hw_interrupt_pin = -1;
	_hw_interruptCallback = 0;
}

static inline unsigned long hw_micros(void) {
	struct timeval tv;
	gettimeofday(&tv, 0);
	return 1000000 * (unsigned int)tv.tv_sec + (unsigned int)tv.tv_usec;
}

static inline void hw_delayMicroseconds(unsigned long time_to_wait) {
	return delayMicroseconds(time_to_wait);
}

static inline void hw_pinMode(uint8_t pin, uint8_t mode){
	// printf("pin mode %d %d\n", pin, mode);
	pinMode((int)pin, (int)mode);
}

static inline void hw_digitalWrite(uint8_t pin, uint8_t value){
	// printf("write %d %d\n",pin, value );
	digitalWrite((int)pin, (int)value);
}

#endif