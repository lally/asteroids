#ifndef INCLUDE_LOCK_H
#define INCLUDE_LOCK_H

#include <pthread.h>


class Lock{
public:
  Lock(pthread_mutex_t& lock) 
    : m_lock(lock) {
    pthread_mutex_lock(&m_lock);
  }

  ~Lock() {
    pthread_mutex_unlock(&m_lock);
  }
private:
  pthread_mutex_t& m_lock;
};


#endif
