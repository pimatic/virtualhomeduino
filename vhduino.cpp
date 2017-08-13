#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
#include "pigpio_functions.h"
extern "C" {
#include "pigpio/pigpio.h"
}
pthread_t pth;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
void enableRealtime() {
	struct sched_param sp;
	// Lock memory to ensure no swapping is done.
	if(mlockall(MCL_FUTURE|MCL_CURRENT)){
		fprintf(stderr,"WARNING: Failed to lock memory\n");
	}
        int nice_level = -20 ;
        setpriority (PRIO_PROCESS, getpid(), nice_level);
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
        if (sp.sched_priority == -1) {
		fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}
   	if (sched_setscheduler(0, SCHED_FIFO, &sp) < 0) {
		fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}
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

static void handle_pigpio_interrupt(int gpio, int level, uint32_t tick) {
if (level < 2) {
	hw_callInterrupt(tick);
}
}

void checkData() {
                        if(RFControl::hasData()) {
                                unsigned int *timings;
                                unsigned int timings_size;
                                RFControl::getRaw(&timings, &timings_size);
                                unsigned int buckets[8];
                                unsigned int pulse_length_divider = RFControl::getPulseLengthDivider();
                                RFControl::compressTimings(buckets, timings, timings_size);
                                fprintf(stderr, "RF receive ");
                                for(unsigned int i=0; i < 8; i++) {
                                        unsigned long bucket = buckets[i] * pulse_length_divider;
                                        fprintf(stderr, "%lu ", bucket);
                                }
                                for(unsigned int i=0; i < timings_size; i++) {
                                        fprintf(stderr, "%d", timings[i]);
                                }
                                fprintf(stderr, "\n");
                                RFControl::continueReceiving();
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
	if(hw_getInterruptPin() == -1) {
	pthread_mutex_lock(&print_mutex);
        if (gpioSetMode(interrupt_pin, PI_INPUT) == 0) {
        gpioSetPullUpDown(interrupt_pin, PI_PUD_UP); 
	if (gpioSetAlertFunc(interrupt_pin,handle_pigpio_interrupt) != 0) {
		printf("Error setting gpioSetAlertFunc for pin %d",interrupt_pin);
	}
	gpioSetTimerFunc(0,50,checkData);
	RFControl::startReceiving(interrupt_pin);
	}
	fprintf(stderr, "ACK\n");
	}
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
	pthread_mutex_lock(&print_mutex);
	//fprintf(stderr,"pin %d",transmitter_pin);
	if (gpioGetMode(transmitter_pin) != 1) {
	if (gpioSetMode(transmitter_pin,PI_OUTPUT) != 0) {
	printf("Error setting output mode for pin %d",transmitter_pin);
	}
	}
	RFControl::sendByTimings(transmitter_pin, sending_timings, sending_timings_size, sending_repeats);
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
	enableRealtime();
	gpioCfgMemAlloc(PI_MEM_ALLOC_PAGEMAP);
	gpioCfgBufferSize(256);
	gpioInitialise();
        gpioCfgSetInternals(PI_CFG_RT_PRIORITY);
	pthread_mutex_init(&print_mutex, NULL);
	//pthread_create(&pth, NULL, interrupt, NULL);
	fprintf(stderr, "ready\n");
	while(fgets(input, sizeof(input), stdin)) {
		if(strlen(input) == 1) {
			continue;
		}
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
	gpioTerminate();
	return 0;
}
