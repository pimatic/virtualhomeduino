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
	wiringXISR(_interrput_pin, INT_EDGE_BOTH);
	interrupt_pin = _interrput_pin;
}

unsigned long micros(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return 1000000 * (unsigned int)tv.tv_sec + (unsigned int)tv.tv_usec;
}

void enableRealtime() {
	// Lock memory to ensure no swapping is done.
	if(mlockall(MCL_FUTURE|MCL_CURRENT)){
		fprintf(stderr,"WARNING: Failed to lock memory\n");
	}

	struct sched_param sp;
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
		fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}
}

void disableRealtime() {
		struct sched_param sp;
	sp.sched_priority = 0;
	if(pthread_setschedparam(pthread_self(), SCHED_OTHER, &sp)){
		fprintf(stderr,"WARNING: Failed to set thread to lower priority\n");
	}
}


unsigned int sending_timings_size;
unsigned int sending_timings[256];
unsigned int transmitter_pin;
unsigned int sending_repeats;
bool sending = false;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;


void *interrupt(void *param) {
	enableRealtime();
	while(1) {
		if(sending) {
			RFControl::sendByTimings(transmitter_pin, sending_timings, sending_timings_size, sending_repeats);
			sending = false;
		}
		if(interrupt_pin != -1) {
			waitForInterrupt(interrupt_pin, 1000);
			interruptCallback();
			if(RFControl::hasData()) {
				unsigned int *timings;
				unsigned int timings_size;
				RFControl::getRaw(&timings, &timings_size);
				unsigned int buckets[8];
				unsigned int pulse_length_divider = RFControl::getPulseLengthDivider();
				RFControl::compressTimings(buckets, timings, timings_size);
				pthread_mutex_lock(&print_mutex);
				fprintf(stderr, "RF receive ");
				for(unsigned int i=0; i < 8; i++) {
					unsigned long bucket = buckets[i] * pulse_length_divider;
					fprintf(stderr, "%lu ", bucket);
				}
				for(unsigned int i=0; i < timings_size; i++) {
					fprintf(stderr, "%d", timings[i]);
				}
				fprintf(stderr, "\n");
				pthread_mutex_unlock(&print_mutex);
				RFControl::continueReceiving();
			}
		} else {
			usleep(500);
		}
	}
}

char input[256];
char delimiter[] = " ";

void argument_error() {
	fprintf(stderr, "ERR argument_error\n");
}

void ping_command() {
	char *arg;
	pthread_mutex_lock(&print_mutex);
	fprintf(stderr, "PING");
	arg = strtok(NULL, delimiter);

	if (arg != NULL) {
		fprintf(stderr, " %s", arg);
	}
	fprintf(stderr, "\n");
	pthread_mutex_unlock(&print_mutex);
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
	pthread_mutex_lock(&print_mutex);
	fprintf(stderr, "ACK\n");
	pthread_mutex_unlock(&print_mutex);
}

void rfcontrol_command_send() {
	char* arg = strtok(NULL, delimiter);
	if(arg == NULL) {
		argument_error();
		return;
	}
	transmitter_pin = atoi(arg);

	arg = strtok(NULL, delimiter);
	if(arg == NULL) {
		argument_error();
		return;
	}
	sending_repeats = atoi(arg);

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
	sending_timings_size = strlen(arg);
	for(unsigned int i = 0; i < sending_timings_size; i++) {
		unsigned int index = arg[i] - '0';
		sending_timings[i] = buckets[index];
	}
	sending = true;
	//wait till message was send
	while(sending) {
		usleep(500);
	}
	pthread_mutex_lock(&print_mutex);	
	fprintf(stderr, "ACK\n");
	pthread_mutex_unlock(&print_mutex);
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
	pthread_create(&pth, NULL, interrupt, NULL);
	fprintf(stderr, "ready\n");
	while(fgets(input, sizeof(input), stdin)) {
		input[strlen(input)-1] = '\0'; //remove tailing new line
		//printf("input=\"%s\"", input); 
			char* command = strtok(input, delimiter);
			if(strcmp("PING", command) == 0) {
				ping_command();
			} else if(strcmp("RF", command) == 0) {
				rfcontrol_command();
			} else {
				pthread_mutex_lock(&print_mutex);	
				fprintf(stderr, "ERR unknown_command\n");
				pthread_mutex_unlock(&print_mutex);
			}
	}

	return 0;
}


void pinMode(uint8_t pin, uint8_t mode){
	// printf("pin mode %d %d\n", pin, mode);
	pinMode((int)pin, (int)mode);
}
void digitalWrite(uint8_t pin, uint8_t value){
	// printf("write %d %d\n",pin, value );
	digitalWrite((int)pin, (int)value);
}
int digitalRead(uint8_t pin){
	return digitalRead((int)pin);
}

int analogRead(uint8_t){return 0;}
void analogReference(uint8_t mode){}
void analogWrite(uint8_t, int){}
void delay(unsigned long){}
/*

 * delayMicroseconds:
 * from wiringPi
 *	This is somewhat intersting. It seems that on the Pi, a single call
 *	to nanosleep takes some 80 to 130 microseconds anyway, so while
 *	obeying the standards (may take longer), it's not always what we
 *	want!
 *
 *	So what I'll do now is if the delay is less than 100uS we'll do it
 *	in a hard loop, watching a built-in counter on the ARM chip. This is
 *	somewhat sub-optimal in that it uses 100% CPU, something not an issue
 *	in a microcontroller, but under a multi-tasking, multi-user OS, it's
 *	wastefull, however we've no real choice )-:
 *
 *      Plan B: It seems all might not be well with that plan, so changing it
 *      to use gettimeofday () and poll on that instead...
 *********************************************************************************
 */


static void delayMicrosecondsHard(unsigned int howLong) {
	struct timeval tNow, tLong, tEnd ;

	gettimeofday(&tNow, NULL);
	tLong.tv_sec  = (__time_t)howLong / 1000000;
	tLong.tv_usec = (__suseconds_t)howLong % 1000000;
	timeradd(&tNow, &tLong, &tEnd);

	while(timercmp(&tNow, &tEnd, <))
		gettimeofday(&tNow, NULL);
}

void delayMicroseconds(unsigned int howLong) {
	struct timespec sleeper;
	long int uSecs = (__time_t)howLong % 1000000;
	unsigned int wSecs = howLong / 1000000;

	if(howLong == 0) {
		return;
	} else if(howLong  < 100) {
		delayMicrosecondsHard(howLong);
	} else {
		sleeper.tv_sec = (__time_t)wSecs;
		sleeper.tv_nsec = (long)(uSecs * 1000L);
		nanosleep(&sleeper, NULL);
	}
}

/*void delayMicrosecondsHard (unsigned int howLong)
{
	struct timeval tNow, tLong, tEnd ;

	gettimeofday (&tNow, NULL) ;
	tLong.tv_sec  = howLong / 1000000 ;
	tLong.tv_usec = howLong % 1000000 ;
	timeradd (&tNow, &tLong, &tEnd) ;

	while (timercmp (&tNow, &tEnd, <))
		gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds (unsigned int howLong)
{
	struct timespec sleeper ;
	unsigned int uSecs = howLong % 1000000 ;
	unsigned int wSecs = howLong / 1000000 ;

	if (howLong ==   0)
		return ;
	else if (howLong  < 100)
		delayMicrosecondsHard (howLong) ;
	else
	{
		sleeper.tv_sec  = wSecs ;
		sleeper.tv_nsec = (long)(uSecs * 1000L) ;
		nanosleep (&sleeper, NULL) ;
	}
}
*/
void detachInterrupt(uint8_t){}

