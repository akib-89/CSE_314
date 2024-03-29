#include "rwlock.h"

void InitalizeReadWriteLock(read_write_lock *rw) {
  pthread_mutex_init(&rw->lock, NULL);
  pthread_cond_init(&rw->read, NULL);
  pthread_cond_init(&rw->write, NULL);
  rw->writers = 0;
  rw->readers = 0;
  rw->write_waiters = 0;
}

void ReaderLock(read_write_lock *rw) {
  pthread_mutex_lock(&rw->lock);
  while (rw->writers > 0) {
    pthread_cond_wait(&rw->read, &rw->lock);
  }
  rw->readers++;
  pthread_mutex_unlock(&rw->lock);
}

void WriterLock(read_write_lock *rw) {
  pthread_mutex_lock(&rw->lock);
  rw->write_waiters++;
  while (rw->readers > 0 || rw->writers > 0) {
    pthread_cond_wait(&rw->write, &rw->lock);
  }
  rw->write_waiters--;
  rw->writers++;
  pthread_mutex_unlock(&rw->lock);
}

void ReaderUnlock(read_write_lock *rw) {
  pthread_mutex_lock(&rw->lock);
  rw->readers--;
  if (rw->readers == 0 && rw->write_waiters > 0) {
    pthread_cond_signal(&rw->write);
  }
  pthread_mutex_unlock(&rw->lock);
}

void WriterUnlock(read_write_lock *rw) {
  pthread_mutex_lock(&rw->lock);
  rw->writers--;
  if (rw->write_waiters > 0) {
    pthread_cond_signal(&rw->write);
  } else {
    pthread_cond_broadcast(&rw->read);
  }
  pthread_mutex_unlock(&rw->lock);
}
