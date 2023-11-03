#include <sys/timerfd.h>
#include <string.h>
#include "rocket/net/timer.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"

namespace rocket {

//定义 Timer 类的默认构造函数，并调用父类 FdEvent 的默认构造函数
Timer::Timer() : FdEvent() {

  //使用 timerfd_create 函数创建一个新的计时器，并用日志输出文件描述符
  m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  DEBUGLOG("timer fd=%d", m_fd);

  // 把 fd 可读事件放到了 eventloop 上监听,并绑定 onTimer 成员函数作为回调
  listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
}
//析构函数
Timer::~Timer() {
}

//定义处理计时器事件的函数。
void Timer::onTimer() {
  // 处理缓冲区数据，防止下一次继续触发可读事件
  // DEBUGLOG("ontimer");
  // 清除计时器缓冲区的数据。
  char buf[8];
  while(1) {
    if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN) {
      break;
    }
  }

  // 获取当前时间,执行定时任务
  int64_t now = getNowMs();
  //两个向量，一个存储计时器事件，另一个存储任务和它们的到达时间。
  std::vector<TimerEvent::s_ptr> tmps;
  std::vector<std::pair<int64_t, std::function<void()>>> tasks;
  //创建一个互斥锁的实例，确保线程安全。
  ScopeMutex<Mutex> lock(m_mutex);
  //定义迭代器并初始化为 m_pending_events 的开始位置。
  auto it = m_pending_events.begin();

  //遍历所有待处理的事件，并将到达时间小于或等于当前时间的事件放入 tmps 和 tasks 向量中。
  for (it = m_pending_events.begin(); it != m_pending_events.end(); ++it) {
    if ((*it).first <= now) {
      if (!(*it).second->isCancled()) {
        tmps.push_back((*it).second);
        tasks.push_back(std::make_pair((*it).second->getArriveTime(), (*it).second->getCallBack()));
      }
    } else {
      break;
    }
  }
  //删除已处理的事件，并解锁。
  m_pending_events.erase(m_pending_events.begin(), it);
  lock.unlock();


  // 需要把重复的Event 再次添加进去
  for (auto i = tmps.begin(); i != tmps.end(); ++i) {
    if ((*i)->isRepeated()) {
      // 调整 arriveTime
      (*i)->resetArriveTime();
      addTimerEvent(*i);
    }
  }
  //重置到达时间
  resetArriveTime();
  //执行所有到达的任务。
  for (auto i: tasks) {
    if (i.second) {
      i.second();
    }
  }

}
//重置计时器的到达时间
void Timer::resetArriveTime() {
  ScopeMutex<Mutex> lock(m_mutex);
  auto tmp = m_pending_events;
  lock.unlock();

  if (tmp.size() == 0) {
    return;
  }

  int64_t now = getNowMs();

  auto it = tmp.begin();
  int64_t inteval = 0;
  if (it->second->getArriveTime() > now) {
    inteval = it->second->getArriveTime() - now;
  } else {
    inteval = 100;
  }

  timespec ts;
  memset(&ts, 0, sizeof(ts));
  ts.tv_sec = inteval / 1000;
  ts.tv_nsec = (inteval % 1000) * 1000000;

  itimerspec value;
  memset(&value, 0, sizeof(value));
  value.it_value = ts;

  int rt = timerfd_settime(m_fd, 0, &value, NULL);
  if (rt != 0) {
    ERRORLOG("timerfd_settime error, errno=%d, error=%s", errno, strerror(errno));
  }
  // DEBUGLOG("timer reset to %lld", now + inteval);

}
//添加新的计时器事件
void Timer::addTimerEvent(TimerEvent::s_ptr event) {
  bool is_reset_timerfd = false;

  ScopeMutex<Mutex> lock(m_mutex);
  if (m_pending_events.empty()) {
    is_reset_timerfd = true;
  } else {
    auto it = m_pending_events.begin();
    if ((*it).second->getArriveTime() > event->getArriveTime()) {
      is_reset_timerfd = true;
    }
  }
  m_pending_events.emplace(event->getArriveTime(), event);
  lock.unlock();

  if (is_reset_timerfd) {
    resetArriveTime();
  }


}
//删除计时器事件。
void Timer::deleteTimerEvent(TimerEvent::s_ptr event) {
  event->setCancled(true);

  ScopeMutex<Mutex> lock(m_mutex);

  auto begin = m_pending_events.lower_bound(event->getArriveTime());
  auto end = m_pending_events.upper_bound(event->getArriveTime());

  auto it = begin;
  for (it = begin; it != end; ++it) {
    if (it->second == event) {
      break;
    }
  }

  if (it != end) {
    m_pending_events.erase(it);
  }
  lock.unlock();

  DEBUGLOG("success delete TimerEvent at arrive time %lld", event->getArriveTime());

}




  
}