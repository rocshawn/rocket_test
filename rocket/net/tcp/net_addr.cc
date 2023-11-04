#include <string.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/net_addr.h"


namespace rocket {

//定义静态函数 CheckValid ，用于检查传入的 addr 字符串是否是一个有效的IP地址
bool IPNetAddr::CheckValid(const std::string& addr) {
  //查找字符串 addr 中的 : 字符，如果找不到则返回 false
  size_t i = addr.find_first_of(":");
  if (i == addr.npos) {
    return false;
  }
  //根据找到的 : 字符位置，拆分出IP地址和端口号
  std::string ip = addr.substr(0, i);
  std::string port = addr.substr(i + 1, addr.size() - i - 1);
  //如果IP地址或端口号为空，则返回 false
  if (ip.empty() || port.empty()) {
    return false;
  }
  //将端口号字符串转换为整数，并检查它是否在合法的范围内
  int iport = std::atoi(port.c_str());
  if (iport <= 0 || iport > 65536) {
    return false;
  }

  return true;

}
//定义 IPNetAddr 的构造函数，接受IP地址字符串和端口号作为参数
IPNetAddr::IPNetAddr(const std::string& ip, uint16_t port) : m_ip(ip), m_port(port) {
  //将 m_addr 结构体的内存清零
  memset(&m_addr, 0, sizeof(m_addr));
  //为 m_addr 结构体设置IP地址、端口号和协议族
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
  m_addr.sin_port = htons(m_port);
}

//定义 IPNetAddr 的构造函数，接受一个 addr 字符串，它应该包含IP地址和端口号
IPNetAddr::IPNetAddr(const std::string& addr) {
  //解析传入的 addr 字符串，提取IP地址和端口号
  size_t i = addr.find_first_of(":");
  if (i == addr.npos) {
    ERRORLOG("invalid ipv4 addr %s", addr.c_str());
    return;
  }
  m_ip = addr.substr(0, i);
  m_port = std::atoi(addr.substr(i + 1, addr.size() - i - 1).c_str());
  //为 m_addr 结构体设置IP地址、端口号和协议族
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
  m_addr.sin_port = htons(m_port);

}

//定义 IPNetAddr 的构造函数，接受一个 sockaddr_in 结构体作为参数
IPNetAddr::IPNetAddr(sockaddr_in addr) : m_addr(addr) {
  //从结构体中提取IP地址和端口号
  m_ip = std::string(inet_ntoa(m_addr.sin_addr));
  m_port = ntohs(m_addr.sin_port);

}
//返回一个指向 m_addr 的 sockaddr 类型的指针
sockaddr* IPNetAddr::getSockAddr() {
  return reinterpret_cast<sockaddr*>(&m_addr);
}
//返回 m_addr 的大小
socklen_t IPNetAddr::getSockLen() {
  return sizeof(m_addr);
}
//返回IP地址的协议族(IPv4)
int IPNetAddr::getFamily() {
  return AF_INET;
}
//将IP地址和端口号转换为字符串
std::string IPNetAddr::toString() {
  //构造并返回IP地址和端口号的字符串表示
  std::string re;
  re = m_ip + ":" + std::to_string(m_port);
  return re;
}
//检查当前对象中的IP地址和端口号是否有效
bool IPNetAddr::checkValid() {
  if (m_ip.empty()) {
    return false;  
  }
  //检查端口号是否在合法范围内
  if (m_port < 0 || m_port > 65536) {
    return false;
  }
  //检查IP地址是否是一个有效的IPv4地址
  if (inet_addr(m_ip.c_str()) == INADDR_NONE) {
    return false;
  }
  return true;
}

}