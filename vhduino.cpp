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

void *interrupt(void *param);

void (*interruptCallback)(void) = NULL;
int interrupt_pin = -1;
pthread_t pth;

void attachInterrupt(uint8_t _interrput_pin, void (*ic)(void), int mode) {
	interruptCallback = ic;
	interrupt_pin = _interrput_pin;
	wiringXISR(interrupt_pin, INT_EDGE_BOTH);
	pthread_create(&pth, NULL, interrupt, NULL);
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
		waitForInterrupt(interrupt_pin, 10000);
		interruptCallback();
		if(RFControl::hasData()) {
			unsigned int *timings;
			unsigned int timings_size;
			RFControl::getRaw(&timings, &timings_size);
			unsigned int buckets[8];
			RFControl::compressTimings(buckets, timings, timings_size);
			printf("RF receive ");
			for(unsigned int i=0; i < 8; i++) {
			 	printf("%d ",buckets[i]);
			}
			for(unsigned int i=0; i < timings_size; i++) {
			 	printf("%d", timings[i]);
			}
			printf("\n");
			RFControl::continueReceiving();
		}
		// int duration = irq_read(1);
		// printf("d: %d\n", duration);
	}
}

char input[256];
char delimiter[] = " ";

void argument_error() {
	printf("ERR argument_error\n");
}

void ping_command() {
	char *arg;
	printf("PING");
	arg = strtok(NULL, delimiter);

	if (arg != NULL) {
		printf(" %s", arg);
	}
	printf("\n");
}

void rfcontrol_command_receive() {
	char* arg = strtok(NULL, delimiter);
	if(arg == NULL) {
		argument_error();
		return;
	}
	int interrupt_pin = atoi(arg);
	if(interruptCallback == NULL) {
		RFControl::startReceiving(interrupt_pin);
	}
	printf("ACK\n");
}

void rfcontrol_command_send() {
  char* arg = strtok(NULL, delimiter);
  if(arg == NULL) {
    argument_error();
    return;
  }
  int transmitter_pin = atoi(arg);

  arg = strtok(NULL, delimiter);
  if(arg == NULL) {
    argument_error();
    return;
  }
  int repeats = atoi(arg);

  // read pulse lengths
  unsigned int buckets[8];
  for(unsigned int i = 0; i < 8; i++) {
    arg = strtok(NULL, delimiter);
    if(arg == NULL) {
      argument_error();
      return;
    }
    buckets[i] = atoi(arg);
  }
  //read pulse sequence
  arg = strtok(NULL, delimiter);
  if(arg == NULL) {
    argument_error();
    return;
  }
  unsigned int timings_size = strlen(arg);
  unsigned int timings[timings_size];
  for(unsigned int i = 0; i < timings_size; i++) {
    unsigned int index = arg[i] - '0';
    timings[i] = buckets[index];
  }
  RFControl::sendByTimings(transmitter_pin, timings, timings_size, repeats);
  printf("ACK\n");
}

void rfcontrol_command() {
  char* arg = strtok(NULL, delimiter);
  if(arg == NULL) {
    argument_error();
    return;
  }
  if (strcmp(arg, "send") == 0) {
    rfcontrol_command_send();
  } else if (strcmp(arg, "receive") == 0) {
    rfcontrol_command_receive();
  } else {
    argument_error();
  }
}


int main(void) {
	wiringXSetup();
	pthread_mutex_init(&print_mutex, NULL);
	printf("ready\n");
	while(fgets(input, sizeof(input), stdin)) {
		input[strlen(input)-1] = '\0'; //remove tailing new line
		//printf("input=\"%s\"", input); 
  	 	pthread_mutex_lock(&print_mutex);
  	 	char* command = strtok(input, delimiter);
  	 	if(strcmp("PING", command) == 0) {
  	 		ping_command();
  	 	} else if(strcmp("RF", command) == 0) {
  	 		rfcontrol_command();
  	 	} else {
  	 		printf("ERR unknown_command\n");
  	 	}
  	 	pthread_mutex_unlock(&print_mutex);
	}

	return 0;
}


void pinMode(uint8_t pin, uint8_t mode){
	pinMode((int)pin, (int)mode);
}
void digitalWrite(uint8_t pin, uint8_t value){
	digitalWrite((int)pin, (int)value);
}
int digitalRead(uint8_t pin){
	digitalRead((int)pin);
}

int analogRead(uint8_t){return 0;}
void analogReference(uint8_t mode){}
void analogWrite(uint8_t, int){}
void delay(unsigned long){}
void delayMicroseconds(unsigned int us){}
void detachInterrupt(uint8_t){}

