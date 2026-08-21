#include "rw_lock.h"

void rwlock_init(rwlock* lock)
{
    lock->active_readers = 0;
    lock->waiting_writers = 0;
    lock->active_writer = 0;
    
    ticketlock_init(&lock->lock); 
    condition_variable_init(&lock->read_cv);
    condition_variable_init(&lock->write_cv);
}


void rwlock_acquire_read(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);
    
    // As long as there is an active writer or even just waiting writers - the reader waits.
    while (lock->active_writer > 0 || lock->waiting_writers > 0) {
        condition_variable_wait(&lock->read_cv, &lock->lock);
    }
    
    // We can now enter (and update the count). Then release the lock immediately.
    lock->active_readers++;
    ticketlock_release(&lock->lock);
}

void rwlock_release_read(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);
    
    lock->active_readers--;
    
    // Case where we are the last reader leaving and there are writers waiting in line - wake up one of them
    if (lock->active_readers == 0 && lock->waiting_writers > 0) {
        condition_variable_signal(&lock->write_cv);
    }

    ticketlock_release(&lock->lock);
}

void rwlock_acquire_write(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);
    
    // "Declare" intention to write
    lock->waiting_writers++;
    
    // Waiting until there are no active readers and no active writer.
    while (lock->active_readers > 0 || lock->active_writer > 0) {
        condition_variable_wait(&lock->write_cv, &lock->lock);
    }
    
    // Done waiting
    lock->waiting_writers--;
    lock->active_writer = 1;
    
    ticketlock_release(&lock->lock);
}

void rwlock_release_write(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);
    
    // Done writing
    lock->active_writer = 0;
    
    // 1+ writers waiting - wake exactly one of them up.
    if (lock->waiting_writers > 0) {
        condition_variable_signal(&lock->write_cv);
    } 
    // No writers are waiting - wake up all readers
    else {
        condition_variable_broadcast(&lock->read_cv);
    }
    
    ticketlock_release(&lock->lock);
}
