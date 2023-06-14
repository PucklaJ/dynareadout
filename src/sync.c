#include "sync.h"
#include "profiling.h"

#ifdef _WIN32
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