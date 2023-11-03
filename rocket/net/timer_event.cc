
#include "rocket/net/timer_event.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"

namespace rocket {
//构造函数，接收三个参数：间隔时间、是否重复和一个回调函数
TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> cb)
   : m_interval(interval), m_is_repeated(is_repeated), m_task(cb) {
  resetArriveTime();
}

//调用 resetArriveTime 函数来初始化定时器的到达时间。
void TimerEvent::resetArriveTime() {
  //设置 m_arrive_time 为当前时间加上定时器的间隔时间。
  m_arrive_time = getNowMs() + m_interval;
  // DEBUGLOG("success create timer event, will excute at [%lld]", m_arrive_time);
  // 表示定时器事件成功创建，将在 m_arrive_time 时执行
}

}