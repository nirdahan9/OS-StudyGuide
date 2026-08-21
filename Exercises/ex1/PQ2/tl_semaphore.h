#ifndef T1_SEMAPHORE_H
#define T1_SEMAPHORE_H

#include <stdatomic.h>

typedef struct
{
    atomic_int ticket;
    atomic_int cur_ticket;
} ticket_lock;

void ticketlock_init(ticket_lock* lock);
void ticketlock_acquire(ticket_lock* lock);
void ticketlock_release(ticket_lock* lock);

typedef struct
{
    int value;
    ticket_lock lock;
} semaphore;

void semaphore_init(semaphore* sem, int initial_value);
void semaphore_wait(semaphore* sem);
void semaphore_signal(semaphore* sem);

#endif 
