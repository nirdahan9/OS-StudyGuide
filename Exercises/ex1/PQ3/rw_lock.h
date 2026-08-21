#ifndef RW_LOCK_H
#define RW_LOCK_H

#include "cond_var.h"
#include "tl_semaphore.h"

typedef struct 
{
    int active_readers;  
    int waiting_writers;
    int active_writer;
    
    ticket_lock lock;
    condition_variable read_cv;
    condition_variable write_cv;

} rwlock;

void rwlock_init(rwlock* lock);

void rwlock_acquire_read(rwlock* lock);

void rwlock_release_read(rwlock* lock);

void rwlock_acquire_write(rwlock* lock);

void rwlock_release_write(rwlock* lock);

#endif
