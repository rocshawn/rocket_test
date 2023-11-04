#include <memory>
#include <string.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/tcp_buffer.h"

namespace rocket {


//定义 TcpBuffer 类的构造函数，接受一个整数 size 作为参数，设置成员变量 m_size 的值并调整 m_buffer 的大小。
TcpBuffer::TcpBuffer(int size) : m_size(size) {
  m_buffer.resize(size);
}

//析构函数
TcpBuffer::~TcpBuffer() {

}

// 返回可读字节数,通过计算写索引和读索引的差来得到的
int TcpBuffer::readAble() {
  return m_write_index - m_read_index;
}

// 返回可写的字节数,通过计算缓冲区的大小和写索引的差来得到的
int TcpBuffer::writeAble() {
  return m_buffer.size() - m_write_index;
}

//读索引的值
int TcpBuffer::readIndex() {
  return m_read_index;
}

//返回写索引的值
int TcpBuffer::writeIndex() {
  return m_write_index;
}

//将给定的 buf 写入缓冲区，size 是 buf 的长度
void TcpBuffer::writeToBuffer(const char* buf, int size) {
  //如果要写入的数据大小超过了可写空间，就调整缓冲区的大小
  if (size > writeAble()) {
    // 调整 buffer 的大小，扩容
    int new_size = (int)(1.5 * (m_write_index + size));
    resizeBuffer(new_size);
  }
  //将数据从 buf 拷贝到缓冲区，并调整写索引
  memcpy(&m_buffer[m_write_index], buf, size);
  m_write_index += size; 
}

//定义一个函数，从缓冲区中读取数据到给定的 re，size 是期望读取的字节数
void TcpBuffer::readFromBuffer(std::vector<char>& re, int size) {
  if (readAble() == 0) {
    return;
  }
  //计算实际读取的字节数，然后从缓冲区中拷贝到临时向量，再将临时向量的内容和给定的向量交换，并调整读索引及缓冲区
  int read_size = readAble() > size ? size : readAble();

  std::vector<char> tmp(read_size);
  memcpy(&tmp[0], &m_buffer[m_read_index], read_size);

  re.swap(tmp); 
  m_read_index += read_size;

  adjustBuffer();
}

//用于调整缓冲区的大小
void TcpBuffer::resizeBuffer(int new_size) {
  //创建一个新的临时缓冲区，然后从原始缓冲区中拷贝数据到临时缓冲区，并交换两个缓冲区的内容。调整读、写索引
  std::vector<char> tmp(new_size);
  int count = std::min(new_size, readAble());
  
  memcpy(&tmp[0], &m_buffer[m_read_index], count);
  m_buffer.swap(tmp);

  m_read_index = 0;
  m_write_index = m_read_index + count;

}

//用于调整缓冲区，确保未读的数据移动到缓冲区的开始位置
void TcpBuffer::adjustBuffer() {
  //如果读索引小于缓冲区大小的1/3，直接返回
  if (m_read_index < int(m_buffer.size() / 3)) {
    return;
  }
  //创建一个新的缓冲区，然后从原始缓冲区中拷贝未读数据到新缓冲区，并交换两个缓冲区的内容。调整读、写索引
  std::vector<char> buffer(m_buffer.size());
  int count = readAble();

  memcpy(&buffer[0], &m_buffer[m_read_index], count);
  m_buffer.swap(buffer);
  m_read_index = 0;
  m_write_index = m_read_index + count;

  buffer.clear();
}

//移动读索引
void TcpBuffer::moveReadIndex(int size) {
  //计算新的读索引，如果它超出了缓冲区的大小，记录错误并返回。否则设置新的读索引并调整缓冲区
  size_t j = m_read_index + size;
  if (j >= m_buffer.size()) {
    ERRORLOG("moveReadIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_read_index, m_buffer.size());
    return;
  }
  m_read_index = j;
  adjustBuffer();
}

//移动写索引
void TcpBuffer::moveWriteIndex(int size) {
  size_t j = m_write_index + size;
  if (j >= m_buffer.size()) {
    ERRORLOG("moveWriteIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_read_index, m_buffer.size());
    return;
  }
  m_write_index = j;
  adjustBuffer();

}

}