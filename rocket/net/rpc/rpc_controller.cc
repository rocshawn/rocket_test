
#include "rocket/net/rpc/rpc_controller.h"

namespace rocket {

// 重置RpcController的状态。
void RpcController::Reset() {
  m_error_code = 0;            // 将错误代码设置为0。
  m_error_info = "";           // 清空错误信息。
  m_msg_id = "";               // 清空消息ID。
  m_is_failed = false;         // 设置请求为“未失败”状态。
  m_is_cancled = false;        // 设置请求为“未取消”状态。
  m_is_finished = false;       // 设置请求为“未完成”状态。
  m_local_addr = nullptr;      // 清空本地地址。
  m_peer_addr = nullptr;       // 清空对方地址。
  m_timeout = 1000;            // 将超时设置为1000毫秒。
}

// 返回请求是否失败。
bool RpcController::Failed() const {
  return m_is_failed;
}

// 返回错误文本。
std::string RpcController::ErrorText() const {
  return m_error_info;
}

// 开始取消请求。
void RpcController::StartCancel() {
  m_is_cancled = true;         // 设置请求为“已取消”状态。
  m_is_failed = true;          // 设置请求为“已失败”状态。
  SetFinished(true);           // 设置请求为“已完成”状态。
}

// 设置请求失败的原因。
void RpcController::SetFailed(const std::string& reason) {
  m_error_info = reason;
  m_is_failed = true;
}

// 返回请求是否被取消。
bool RpcController::IsCanceled() const {
  return m_is_cancled;
}

// 在请求被取消时调用指定的回调（当前此函数为空，没有实现）。
void RpcController::NotifyOnCancel(google::protobuf::Closure* callback) {

}

// 设置错误代码和错误信息。
void RpcController::SetError(int32_t error_code, const std::string error_info) {
  m_error_code = error_code;   // 设置错误代码。
  m_error_info = error_info;   // 设置错误信息。
  m_is_failed = true;          // 设置请求为“已失败”状态。
}

// 获取错误代码。
int32_t RpcController::GetErrorCode() {
  return m_error_code;
}

// 获取错误信息。
std::string RpcController::GetErrorInfo() {
  return m_error_info;
}

// 设置消息ID。
void RpcController::SetMsgId(const std::string& msg_id) {
  m_msg_id = msg_id;
}

// 获取消息ID。
std::string RpcController::GetMsgId() {
  return m_msg_id;
}

// 设置本地地址。
void RpcController::SetLocalAddr(NetAddr::s_ptr addr) {
  m_local_addr = addr;
}

// 设置对方地址。
void RpcController::SetPeerAddr(NetAddr::s_ptr addr) {
  m_peer_addr = addr;
}

// 获取本地地址。
NetAddr::s_ptr RpcController::GetLocalAddr() {
  return m_local_addr;
}

// 获取对方地址。
NetAddr::s_ptr RpcController::GetPeerAddr() {
  return m_peer_addr;
}

// 设置超时时间。
void RpcController::SetTimeout(int timeout) {
  m_timeout = timeout;
}

// 获取超时时间。
int RpcController::GetTimeout() {
  return m_timeout;
}

// 返回请求是否已完成。
bool RpcController::Finished() {
  return m_is_finished;
}

// 设置请求是否已完成。
void RpcController::SetFinished(bool value) {
  m_is_finished = value;
}

}