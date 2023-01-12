/*===- libuvpp.hpp - Wrapper for 'extern "C"' ---------------*- C -*-=======*\
|*                                                                                                                       *|
|* Part of the Libuvpp® Project, under the Apache License v2.0 with Libuvpp®  *|
|* Exceptions.                                                                                                     *|
|* See http://skstu.com/libuvpp/LICENSE.txt for license information.                   *|
|* SPDX-License-Identifier: Apache-2.0 WITH LIBUVPP-exception                        *|
|*                                                                                                                        *|
|*===----------------------------------------------------------------------====== *|
|*                                                                                                                        *|
|* This file is a summary of the interface for this project.                                      *|
|*                                                                                                                        *|
\*===----------------------------------------------------------------------======*/
#if !defined(__83D45951_27E1_4618_9C45_A70B814586FA__)
#define __83D45951_27E1_4618_9C45_A70B814586FA__

#include <dllinterface.h>

namespace libuvpp {
 using TypeIdentify = unsigned long long;
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

 enum class EnIPV {
  IPV4 = 0,
  IPV6 = 1,
 };

 enum class EnSocketType {
  UDP = 0,
  TCP = 1,
  PIPE = 3,
 };

 enum class EnClientStatus {
  ConnectSuccess = 0,
  ConnectFail = 1,
  ConnectClose = 2,
  ReconnectionIng = 3,
  ConnectTimeout = 4,
  Unknow = 5,
 };
 //!@ To client.
 using tfOnClientMessageGetSendData = std::function<void(const char*, const size_t&)>;
 using tfOnClientMessage = std::function<void(const std::string&, const tfOnClientMessageGetSendData&)>;
 using tfOnClientStatus = std::function<void(const EnClientStatus&)>;
 //!@ To server.
 using tfOnServerMessage = std::function<void(class ISession*, const std::string&)>;
 using tfOnSessionCreateAfterCb = std::function<void(class ISession*)>;
 using tfOnSessionDestoryAfterCb = std::function<void(class ISession*)>;
 using tfOnSessionDestoryBeforeCb = std::function<void(class ISession*)>;
 //!@ To session.
 using tfSessionReadCb = std::function<void(const std::string&)>;

 class IConfig {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual void Identify(const TypeIdentify&) = 0;
  virtual const EnIPV& IPV() const = 0;
  virtual void IPV(const EnIPV&) = 0;
  virtual const EnSocketType& SocketType() const = 0;
  virtual void SocketType(const EnSocketType&) = 0;
  virtual const std::string& IPAddr() const = 0;
  virtual void IPAddr(const std::string&) = 0;
  virtual const unsigned short& Port() const = 0;
  virtual void Port(const unsigned short&) = 0;
  virtual void Release() const = 0;
 };


 class IClient {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual const std::string& Name() const = 0;
  virtual const EnIPV& Ipv() const = 0;
  virtual const EnSocketType& SocketType() const = 0;
  virtual const std::string& ToAddr() const = 0;
  virtual bool Start(const EnSocketType&, const EnIPV&, const std::string&) = 0;
  virtual void Stop() = 0;
  virtual EnClientStatus Status() const = 0;
  virtual void Reconnection(const time_t&) = 0;
  virtual void ReconnectionCount(const std::uint16_t&) = 0;
  virtual void Write(const unsigned long long&, const std::string&) = 0;
  virtual void Write(const std::string&) = 0;
  virtual void Release() const = 0;
  virtual void OnClientMessageGetSendData(const tfOnClientMessageGetSendData&) = 0;
  virtual void OnClientMessage(const tfOnClientMessage&) = 0;
  virtual void OnClientStatus(const tfOnClientStatus&) = 0;
 };

 class ISession {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual const std::string& Name() const = 0;
  virtual void Write(const unsigned long long&, const std::string&) = 0;
  virtual void Read(std::vector<std::string>&) = 0;
  virtual void Read(std::string&) = 0;//!@ No custom handle.
  virtual void Read(const tfSessionReadCb&) = 0;
  virtual void Write(const std::string&) = 0;//!@ No custom handle.
  virtual void Release() const = 0;
  virtual bool Ready() const = 0;
  virtual void BindUL(const unsigned long&) = 0;
  virtual const unsigned long& BindUL() const = 0;
  virtual void BindULL(const unsigned long long&) = 0;
  virtual const unsigned long long& BindULL() const = 0;
  virtual void BindStr(const std::string&) = 0;
  virtual const std::string& BindStr() const = 0;
  virtual void BindPtr(void*) = 0;
  virtual void* BindPtr() const = 0;
  virtual void BindProcessId(const unsigned long&) = 0;
  virtual const unsigned long& BindProcessId() const = 0;
  virtual void BindTaskId(const unsigned long long&) = 0;
  virtual const unsigned long long& BindTaskId() const = 0;
 };

 class IServer {
 public:
  virtual IConfig* ConfigGet() const = 0;
  virtual bool Start() = 0;
  virtual void Stop() = 0;
  virtual bool Ready() const = 0;
  virtual void Release() const = 0;
  virtual void Write(const unsigned long long&, const std::string&) = 0;
  virtual void OnServerMessage(const tfOnServerMessage&) = 0;
  virtual void OnSessionCreateAfterCb(const tfOnSessionCreateAfterCb&) = 0;
  virtual void OnSessionDestoryAfterCb(const tfOnSessionDestoryAfterCb&) = 0;
  virtual void OnSessionDestoryBeforeCb(const tfOnSessionDestoryBeforeCb&) = 0;
 };

 class ILibuv : public shared::InterfaceDll<ILibuv> {
 public:
  virtual IClient* CreateClient() = 0;
  virtual IServer* CreateServer() = 0;
  virtual void Release() const = 0;
 protected:
  ILibuv() {}
  ~ILibuv() {}
 };


}///namespace libuvpp

/// /*_ Memade®（新生™） _**/
/// /*_ Fri, 09 Dec 2022 00:12:39 GMT _**/
/// /*_____ https://www.skstu.com/ _____ **/
#endif///__83D45951_27E1_4618_9C45_A70B814586FA__


