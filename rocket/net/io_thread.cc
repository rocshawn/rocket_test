
#include <pthread.h>
#include <assert.h>
#include "rocket/net/io_thread.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"


namespace rocket {

//构造函数
IOThread::IOThread() {  
  //初始化 m_init_semaphore 信号量
  int rt = sem_init(&m_init_semaphore, 0, 0);
  assert(rt == 0);
  //初始化 m_start_semaphore 信号量
  rt = sem_init(&m_start_semaphore, 0, 0);
  assert(rt == 0);

  pthread_create(&m_thread, NULL, &IOThread::Main, this);

  // wait, 直到新线程执行完 Main 函数的前置
  sem_wait(&m_init_semaphore);

  DEBUGLOG("IOThread [%d] create success", m_thread_id);
}

//析构函数
IOThread::~IOThread() {
  //停止事件循环。
  m_event_loop->stop();
  //销毁之前创建的两个信号量。
  sem_destroy(&m_init_semaphore);
  sem_destroy(&m_start_semaphore);

  pthread_join(m_thread, NULL);

  if (m_event_loop) {
    delete m_event_loop;
    m_event_loop = NULL;
  }
}


void* IOThread::Main(void* arg) {
  IOThread* thread = static_cast<IOThread*> (arg);
  //初始化事件循环和线程ID。
  thread->m_event_loop = new EventLoop();
  thread->m_thread_id = getThreadId();


  // 唤醒等待的线程
  sem_post(&thread->m_init_semaphore);

  // 让IO 线程等待，直到我们主动的启动

  DEBUGLOG("IOThread %d created, wait start semaphore", thread->m_thread_id);

  sem_wait(&thread->m_start_semaphore);
  //开始事件循环
  DEBUGLOG("IOThread %d start loop ", thread->m_thread_id);
  thread->m_event_loop->loop();

  DEBUGLOG("IOThread %d end loop ", thread->m_thread_id);

  return NULL;

}

//获取事件循环对象的函数。
EventLoop* IOThread::getEventLoop() {
  return m_event_loop;
}

//开始IO线程的函数，通过释放 m_start_semaphore 信号量。
void IOThread::start() {
  DEBUGLOG("Now invoke IOThread %d", m_thread_id);
  sem_post(&m_start_semaphore);
}

//等待IO线程结束。
void IOThread::join() {
  pthread_join(m_thread, NULL);
}



}