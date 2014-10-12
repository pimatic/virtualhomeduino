#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>

extern "C" {
	#include "wiringX/wiringX.h"
}

#include "rfcontrol/RFControl.h"


void (*interruptCallback)(void);
void attachInterrupt(uint8_t, void (*ic)(void), int mode) {
	interruptCallback = ic;
}

unsigned long micros(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return 1000000 * (unsigned int)tv.tv_sec + (unsigned int)tv.tv_usec;
}


void realtime() {
	// Lock memory to ensure no swapping is done.
	if(mlockall(MCL_FUTURE|MCL_CURRENT)){
			fprintf(stderr,"WARNING: Failed to lock memory\n");
	}

	// Set our thread to real time priority
	struct sched_param sp;
	sp.sched_priority = 30;
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
			fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}
}


pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;


void *interrupt(void *param) {
	realtime();
	while(1) {
		waitForInterrupt(1, 10000);
		interruptCallback();
		if(RFControl::hasData()) {
			unsigned int *timings;
			unsigned int timings_size;
			RFControl::getRaw(&timings, &timings_size);
			pthread_mutex_lock(&print_mutex);
			for(int i=0; i < timings_size; i++) {
				printf("%d", timings[i]);
				printf(" ");
				if((i+1)%16 == 0) {
					printf("\n");
				}
			}
			printf("\n");
			printf("\n");
			pthread_mutex_unlock(&print_mutex);
			RFControl::continueReceiving();
		}
		// int duration = irq_read(1);
		// printf("d: %d\n", duration);
	}

}


int main(void) {
	pthread_t pth;

	wiringXSetup();

	pthread_mutex_init(&print_mutex, NULL);

	pinMode(0, OUTPUT);
	wiringXISR(1, INT_EDGE_BOTH);

	RFControl::startReceiving(0);
	pthread_create(&pth, NULL, interrupt, NULL);

	while(1) {
		char input[256];
  	 	scanf ("%s",input);
  	 	pthread_mutex_lock(&print_mutex);
  	 	printf("input: %s", input);
  	 	pthread_mutex_unlock(&print_mutex);
		usleep(1000);
	}

	return 0;
}


void pinMode(uint8_t, uint8_t){}
void digitalWrite(uint8_t, uint8_t){}
int digitalRead(uint8_t){return 0;}
int analogRead(uint8_t){return 0;}
void analogReference(uint8_t mode){}
void analogWrite(uint8_t, int){}
void delay(unsigned long){}
void delayMicroseconds(unsigned int us){}
void detachInterrupt(uint8_t){}

