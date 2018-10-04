#ifndef DEVICES_TIMER_H
#define DEVICES_TIMER_H

#include <round.h>
#include <stdint.h>

/* Number of timer interrupts per second. */
#define TIMER_FREQ 100

void timer_init (void);
void timer_calibrate (void);

int64_t timer_ticks (void);
int64_t timer_elapsed (int64_t);

/* Sleep and yield the CPU to other threads. */
void timer_sleep (int64_t ticks);
void timer_msleep (int64_t milliseconds);
void timer_usleep (int64_t microseconds);
void timer_nsleep (int64_t nanoseconds);

/* Busy waits. */
void timer_mdelay (int64_t milliseconds);
void timer_udelay (int64_t microseconds);
void timer_ndelay (int64_t nanoseconds);

void timer_print_stats (void);

/* Returns true if TIME_REMAINING of node A is less than that of node B */
bool sleep_less (const struct list_elem *a,
		 const struct list_elem *b,
		 void *aux);

/* Ensure that blocked_thread points to a valid thread when unblocking */
typedef struct sleep_node {
  struct list_elem elem;
  int time_remaining;
  struct thread * blocked_thread;
} sleep_node;

#endif /* devices/timer.h */
