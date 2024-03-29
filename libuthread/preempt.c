#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 * Note: 0.01 second = 10000 microsecond
 */
#define HZ 100
#define ELAPSED_TIME 10000

/*
 * signal handler for SIGVTALRM
 * whenever we receive SIGVTALRM
 * we force the current running thread
 * to yield
 */
void VTALRM_handler(int signum)
{
    uthread_yield();
}

void preempt_disable(void)
{
    //uninstall the signal handler
    struct sigaction new_action;
    new_action.sa_handler = SIG_IGN;
    sigfillset(&new_action.sa_mask);
    sigdelset(&new_action.sa_mask, SIGVTALRM);
    new_action.sa_flags = 0;

    if(sigaction(SIGVTALRM, &new_action, NULL) < 0)
        printf("Preempt disable fail.\n");
}

void preempt_enable(void)
{
    //reinstall the signal handler
    struct sigaction new_action;
    new_action.sa_handler = VTALRM_handler;
    sigfillset(&new_action.sa_mask);
    sigdelset(&new_action.sa_mask, SIGVTALRM);
    new_action.sa_flags = 0;

    if(sigaction(SIGVTALRM, &new_action, NULL) < 0)
        printf("Preempt disable fail.\n");
}

void preempt_start(void)
{
	//install a signal handler
    struct sigaction new_action;
	new_action.sa_handler = VTALRM_handler;
	//block every signal except SIGVTALRM
	sigfillset(&new_action.sa_mask);
	sigdelset(&new_action.sa_mask, SIGVTALRM);
	new_action.sa_flags = 0;

    struct itimerval timer = {};
    timer.it_interval.tv_usec = (long int)ELAPSED_TIME;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_usec = (long int)ELAPSED_TIME;
    timer.it_value.tv_sec = 0;

	if(sigaction(SIGVTALRM, &new_action, NULL) < 0
	|| setitimer(ITIMER_VIRTUAL, &timer, NULL) < 0){
	    printf("Preempt_start fail.\n");
	}
}