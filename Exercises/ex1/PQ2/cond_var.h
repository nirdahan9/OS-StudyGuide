#ifndef COND_VAR_H
#define COND_VAR_H

#include <stdatomic.h>
#include <sched.h>
#include "tl_semaphore.h"

typedef struct 
{
    semaphore sem;
    atomic_int ticket_head;
    atomic_int ticket_tail;
} condition_variable;

void condition_variable_init(condition_variable* cv);

void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock);

void condition_variable_signal(condition_variable* cv);

void condition_variable_broadcast(condition_variable* cv);

#endif
