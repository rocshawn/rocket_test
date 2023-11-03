#include "rocket/net/fd_event_group.h"
#include "rocket/common/mutex.h"
#include "rocket/common/log.h"

namespace rocket {


static FdEventGroup* g_fd_event_group = NULL;  //定义了一个静态的全局 FdEventGroup 指针并初始化为 NULL。

FdEventGroup* FdEventGroup::GetFdEventGroup() {  //定义了一个静态成员函数来获取 FdEventGroup 的实例。如果实例已经存在，就直接返回；否则，创建一个新的实例并返回。
  if (g_fd_event_group != NULL) {
    return g_fd_event_group;
  }

  g_fd_event_group = new FdEventGroup(128);
  return g_fd_event_group;
}

FdEventGroup::FdEventGroup(int size) :m_size(size) {  //构造函数，接收一个大小参数。它初始化大小，并为每个索引创建一个 FdEvent 对象。
  for (int i = 0; i < m_size; i++) {
    m_fd_group.push_back(new FdEvent(i));
  }
}

FdEventGroup::~FdEventGroup() {  //析构函数。它释放了分配给 m_fd_group 的所有内存。
  for (int i = 0; i < m_size; ++i) {
    if (m_fd_group[i] != NULL) {
      delete m_fd_group[i];
      m_fd_group[i] = NULL;
    }
  }
}

FdEvent* FdEventGroup::getFdEvent(int fd) {  //用于获取指定文件描述符的 FdEvent。
  //首先锁定互斥锁，然后检查请求的文件描述符是否在当前大小范围内。
  //如果不在范围内，它将扩展 m_fd_group 的大小，并创建新的 FdEvent 对象，最后返回请求的 FdEvent。
  ScopeMutex<Mutex> lock(m_mutex);
  if ((size_t) fd < m_fd_group.size()) {
    return m_fd_group[fd];
  }

  int new_size = int(fd * 1.5);
  for (int i = m_fd_group.size(); i < new_size; ++i) {
    m_fd_group.push_back(new FdEvent(i));
  }
  return m_fd_group[fd];

}

}