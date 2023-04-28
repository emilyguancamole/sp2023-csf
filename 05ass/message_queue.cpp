#include <cassert>
#include <ctime>
#include <pthread.h>
#include "guard.h"
#include "message_queue.h"

MessageQueue::MessageQueue() {
  // TODO: initialize the mutex and the semaphore
  sem_init(&m_avail, 0, 0);
  pthread_mutex_init(&m_lock, NULL);
}

MessageQueue::~MessageQueue() {
  // TODO: destroy the mutex and the semaphore
  sem_destroy(&m_avail);
  pthread_mutex_destroy(&m_lock);
}

void MessageQueue::enqueue(Message *msg) {
  // TODO: put the specified message on the queue
  // be sure to notify any thread waiting for a message to be
  // available by calling sem_post

  pthread_mutex_lock(&m_lock); // lock mutex
  m_messages.push_back(msg);
  pthread_mutex_unlock(&m_lock); // unlock after adding message
  sem_post(&m_avail);
}

Message *MessageQueue::dequeue() {
  struct timespec ts;

  // get the current time using clock_gettime:
  // we don't check the return value because the only reason
  // this call would fail is if we specify a clock that doesn't
  // exist
  clock_gettime(CLOCK_REALTIME, &ts);

  // compute a time one second in the future
  ts.tv_sec += 1;

  // call sem_timedwait to wait up to 1 second for a message
  //       to be available, return nullptr if no message is available
  // never release the guard so we can't enqueue if we lock here, so lock after
  if (sem_timedwait(&m_avail, &ts) != 0) { // returns 0 if successful
    return nullptr;
  }

  pthread_mutex_lock(&m_lock); // lock before manipulating message queue
  Message *msg = nullptr; // remove the next message from the queue, return it
  msg = m_messages.front();
  m_messages.pop_front();
  pthread_mutex_unlock(&m_lock);
  return msg;
}