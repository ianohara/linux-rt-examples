/* Copyright 2013 Ian O'Hara
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Exemplifies how to check if hrtimers are available,
 * and then if they are how to use them.
 * 
 *
 * Note: A lot of the "rt" idioms were found in the rt-tests source code.
 *
 * Note/TODO: This just explains how to setup a timer, but does not ensure that
 *       the process is scheduled with SCHED_FIFO and has a high rt priority.
 */

#include <stdio.h>  /* printf */
#include <time.h>   /* clock_getres, clock_gettime, time */
#include <signal.h> /* sigevent, sigaction */

static const unsigned long USEC_PER_SEC = 1000000u;

/* This is taken from cyclictest.c in the rt-tests suite.
 * (Comments added by me)
 */
static int have_hrtimers(void)
{
    struct timespec ts;

    /* Check the resolution of the Monotonic clock, which
     * counts up monotonically from system boot.
     *
     * Should return 0 if there are no problems with the clock.
     */
    if (clock_getres(CLOCK_MONOTONIC, &ts)) {
        perror("check_timer clock_getres problem");
        return 0;
    }

    /* If the monotonic clock has 1 nanosecond resolution, then
     * the kernel is using hrtimers.
     */
    return !(ts.tv_sec != 0 || ts.tv_nsec != 1);
}

/* This will get called every time our timer goes off.  Increment
 * a global each time so we can tell how many times it has been
 * called and print out trigget_cnt periodically in the main
 * loop.
 *
 * This is where you'd read in your sensor data, run your controller, or whatever.  
 * Remember though, that if you're scheduled with SCHED_FIFO and have the highest
 * realtime priority, this callback could end up crashing the whole system by 
 * hogging the cpu!!!  It is essentially a userspace interrupt, so beware!
 */
static unsigned long trigger_cnt = 0;
static void sig_alarm_handler(int sig) {
    trigger_cnt++;
}

int main(int argc, char *argv[])
{
    if (!have_hrtimers()) {
        printf("Error: hrtimers are not enabled/available, cannot start hrtimerTest!");
        return 1;
    } else {
        printf("--> hrtimers are available.\n");
    }

    /* Create a timer that triggers every 100us which calls a function that prints
     * information every 1s (so we don't get hung on io trying to print something
     * every 100us)
     *
     * The timer_create manpage is useful (it has a source code example):
     *   http://www.kernel.org/doc/man-pages/online/pages/man2/timer_create.2.html
     */
    unsigned long interval_us = 1000;
    printf("Creating a timer that triggers every %u microseconds.\n", interval_us);
    struct timespec interval = {
        .tv_sec = interval_us / USEC_PER_SEC,
        .tv_nsec = (interval_us % USEC_PER_SEC) * 1000
    };
    /* For keeping track of the timer we setup */
    timer_t timer_id;
    /* For telling the timer when it should fire first, and at what interval from then on. */
    struct itimerspec timer_spec;
    /* For telling the timer what to do when it fires */
    struct sigevent sigevt;
    /* For telling Linux what to do when the timer sends the signal to this process */
    struct sigaction sigact;

    /* Have the timer send us a SIGALRM when it fires.  Note that this means
     * we cannot use any of the sleep, usleep, etc mechanisms that might use
     * SIGALRM.
     */
    sigevt.sigev_notify = SIGEV_SIGNAL;
    sigevt.sigev_signo = SIGALRM;

     /* Do nothing special when the signal from the timer is received,
      * just call the specified handler. */
    sigact.sa_handler = &sig_alarm_handler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(sigevt.sigev_signo, &sigact, NULL)) {
        perror("sigaction: ");
        return 1;
    }
    
    /* Create the timer using CLOCK_MONOTONIC which is the system clock
     * that *always* counts up (ie: leap days, daylight savings, etc do
     * not cause it to do funky things).
     */
    if (timer_create(CLOCK_MONOTONIC, &sigevt, &timer_id)) {
        perror("timer_create: ");
        return 1;
    }

    /* After the timer fires, the interval to set for the next
     * firing of the timer.  If zero, this timer only fires once.
     */
    timer_spec.it_interval = interval;
    /* When to trigger for the first time, by
     * default this is relative to time of call to timer_settime */
    timer_spec.it_value = interval; 
    if (timer_settime(timer_id, 0, &timer_spec, NULL)) {
        perror("timer_settime: ");
        return 1;
    }
     
    printf("Now we wait...\n");
    unsigned int total_time = 0;
    time_t sleep_int = 2;
    time_t now, last = time(NULL);
    time_t now;
    for (;;) {
        /* IMPORTANT USAGE NOTE: sleep, usleep, and nanosleep all have unspecified
         * behaviors when used in parallel with signals.  Beware.
         */
        now = time(NULL);
        if ((now - last) < sleep_int) continue;
        last = now;
        total_time += sleep_int;
        printf("After ~%u seconds the timer has triggered %lu times.\n", total_time, trigger_cnt);
    }
}
