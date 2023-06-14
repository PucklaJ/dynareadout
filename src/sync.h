#ifndef SYNC_H
#define SYNC_H

#ifdef _WIN32
#include <windows.h>

typedef HANDLE sync_t;
#else
#include <pthread.h>

typedef pthread_mutex_t sync_t;
#endif

/* Creates a synchronisation object (Usually called mutex). Needs to be
 * destroyed by sync_destroy*/
sync_t sync_create();

/* Blocks until the mutex is unlocked and locks it. Return 0 on success.*/
int sync_lock(sync_t *snc);

/* Locks the mutex and returns 0 if it is unlocked. If the mutex is currently
 * locked it returns EBUSY and does not block otherwise it returns the error
 * code.*/
int sync_trylock(sync_t *snc);

/* Unlocks the mutex if it is currently locked and returns 0 on success*/
int sync_unlock(sync_t *snc);

/* Destroy the mutex*/
void sync_destroy(sync_t *snc);

#endif