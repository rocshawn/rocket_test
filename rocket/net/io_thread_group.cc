#include "rocket/net/io_thread_group.h"
#include "rocket/common/log.h"


namespace rocket {

//构造函数。根据给定的大小初始化 I/O 线程组，并创建相应数量的 IOThread 对象。
IOThreadGroup::IOThreadGroup(int size) : m_size(size) {
  m_io_thread_groups.resize(size);
  for (size_t i = 0; (int)i < size; ++i) {
    m_io_thread_groups[i] = new IOThread();
  }
}

//析构函数
IOThreadGroup::~IOThreadGroup() {

}

//启动所有的 I/O 线程。
void IOThreadGroup::start() {
  for (size_t i = 0; i < m_io_thread_groups.size(); ++i) {
    m_io_thread_groups[i]->start();
  }
}

//用于阻塞当前线程，直到所有的 I/O 线程都已经结束。
void IOThreadGroup::join() {
  for (size_t i = 0; i < m_io_thread_groups.size(); ++i) {
    m_io_thread_groups[i]->join();
  }
} 

//返回一个 IOThread 对象。它首先检查当前的索引是否已经超出了数组的大小或是否为-1，如果是，则将索引重置为0。
//然后返回相应索引的 IOThread 对象，并将索引加1。
IOThread* IOThreadGroup::getIOThread() {
  if (m_index == (int)m_io_thread_groups.size() || m_index == -1)  {
    m_index = 0;
  }
  return m_io_thread_groups[m_index++];
}

}