/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaJ/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 Jonas Pucher
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 ************************************************************************************/

#ifndef SYNC_H
#define SYNC_H

#ifdef _WIN32
#include <windows.h>

typedef HANDLE sync_t;
#else
#include <pthread.h>

typedef pthread_mutex_t sync_t;
#endif

#ifdef __cplusplus
extern "C" {
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

#ifdef __cplusplus
}
#endif

#endif