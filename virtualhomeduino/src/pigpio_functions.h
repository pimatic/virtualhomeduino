#include <sys/stat.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../../pigpio/pigpio.h"

#define MAX_RECORDINGS 1024
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

static inline void hw_pinMode(uint8_t pin, uint8_t mode){
	gpioSetMode(pin, mode);

}
static inline void hw_digitalWrite(uint8_t pin, uint8_t value){
	gpioWrite(pin, value);
}
static inline void hw_delayMicroseconds(unsigned long time_to_wait) {
uint32_t diffTick;
uint32_t startTick = gpioTick();
while(true){
diffTick = gpioTick() - startTick;
if (diffTick > time_to_wait) {
break;
}
}
}
