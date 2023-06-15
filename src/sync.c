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

#include "sync.h"
#include "errno.h"
#include "profiling.h"

#ifdef _WIN32
sync_t sync_create() {
  BEGIN_PROFILE_FUNC();

  sync_t snc = (sync_t)CreateEventW(NULL, FALSE, TRUE, NULL);

  END_PROFILE_FUNC();
  return snc;
}

int sync_lock(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  const DWORD win_rv = WaitForSingleObject(*((HANDLE *)snc), INFINITE);

  int rv;
  switch (win_rv) {
  case WAIT_FAILED:
  case WAIT_TIMEOUT:
    rv = EINVAL;
    break;
  default:
    rv = 0;
    break;
  }

  END_PROFILE_FUNC();
  return rv;
}

int sync_trylock(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  const DWORD win_rv = WaitForSingleObject(*((HANDLE *)snc), 0);

  int rv;
  switch (win_rv) {
  case WAIT_TIMEOUT:
    rv = EBUSY;
    break;
  case WAIT_FAILED:
    rv = EINVAL;
    break;
  default:
    rv = 0;
    break;
  }

  END_PROFILE_FUNC();
  return rv;
}

int sync_unlock(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  const BOOL win_rv = SetEvent(*((HANDLE *)snc));
  const int rv = win_rv != 0 ? 0 : EINVAL;

  END_PROFILE_FUNC();
  return rv;
}

void sync_destroy(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  CloseHandle(*((HANDLE *)snc));

  END_PROFILE_FUNC();
}

#else
sync_t sync_create() {
  BEGIN_PROFILE_FUNC();

  sync_t snc;
  pthread_mutex_init((pthread_mutex_t *)&snc, NULL);

  END_PROFILE_FUNC();
  return snc;
}

int sync_lock(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  const int rv = pthread_mutex_lock((pthread_mutex_t *)snc);

  END_PROFILE_FUNC();
  return rv;
}

int sync_trylock(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  const int rv = pthread_mutex_trylock((pthread_mutex_t *)snc);

  END_PROFILE_FUNC();
  return rv;
}

int sync_unlock(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  const int rv = pthread_mutex_unlock((pthread_mutex_t *)snc);

  END_PROFILE_FUNC();
  return rv;
}

void sync_destroy(sync_t *snc) {
  BEGIN_PROFILE_FUNC();

  pthread_mutex_destroy((pthread_mutex_t *)snc);

  END_PROFILE_FUNC();
}

#endif