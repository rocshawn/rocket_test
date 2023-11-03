#include <string.h>
#include <fcntl.h>
#include "rocket/net/fd_event.h"
#include "rocket/common/log.h"

namespace rocket {
//构造函数，它初始化了文件描述符并将 m_listen_events 结构体的内存清零。
FdEvent::FdEvent(int fd) : m_fd(fd) {
  memset(&m_listen_events, 0, sizeof(m_listen_events));
}

//不带参数的构造函数，它同样将 m_listen_events 结构体的内存清零。
FdEvent::FdEvent() {
  memset(&m_listen_events, 0, sizeof(m_listen_events));
}


//析构函数
FdEvent::~FdEvent() {

}

//根据传入的事件类型返回相应的回调函数。
std::function<void()> FdEvent::handler(TriggerEvent event) {
  if (event == TriggerEvent::IN_EVENT) {
    return m_read_callback;
  } else if (event == TriggerEvent::OUT_EVENT) {
    return m_write_callback;
  } else if (event == TriggerEvent::ERROR_EVENT) {
    return m_error_callback;
  }
  return nullptr;
}

// 为特定事件类型设置监听，并指定对应的回调函数。
void FdEvent::listen(TriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback /*= nullptr*/) {
  if (event_type == TriggerEvent::IN_EVENT) {
    m_listen_events.events |= EPOLLIN;
    m_read_callback = callback;
  } else {
    m_listen_events.events |= EPOLLOUT;
    m_write_callback = callback;
  }

  if (m_error_callback == nullptr) {
    m_error_callback = error_callback;
  } else {
    m_error_callback = nullptr;
  }

  m_listen_events.data.ptr = this;
}

//取消对特定事件的监听。
void FdEvent::cancle(TriggerEvent event_type) {
  if (event_type == TriggerEvent::IN_EVENT) {
    m_listen_events.events &= (~EPOLLIN);
  } else {
    m_listen_events.events &= (~EPOLLOUT);
  }
}

//将文件描述符设置为非阻塞模式。
void FdEvent::setNonBlock() {
  
  int flag = fcntl(m_fd, F_GETFL, 0);
  if (flag & O_NONBLOCK) {
    return;
  }

  fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
}


}