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
#if !defined(INC_H___19A77F54_4FFC_418D_BFEB_0CA3C94A278D__HEAD__)
#define INC_H___19A77F54_4FFC_418D_BFEB_0CA3C94A278D__HEAD__

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
 using tfOnClientMessageGetSendData = std::function<void(const char*, const size_t&)>;
 using tfOnClientMessage = std::function<void(const std::string&, const tfOnClientMessageGetSendData&)>;
 using tfOnClientStatus = std::function<void(const EnClientStatus&)>;
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
  virtual void MessageCb(const tfOnClientMessage&) = 0;
  virtual void StatusCb(const tfOnClientStatus&) = 0;
  virtual void Write(const unsigned long long&, const std::string&) = 0;
  virtual void Write(const std::string&) = 0;
  virtual void Release() const = 0;
 };

 class ISession {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual const std::string& Name() const = 0;
  virtual void Write(const unsigned long long&, const std::string&) = 0;
  virtual void Read(std::vector<std::string>&) = 0;
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

 using tfOnServerMessage = std::function<void(ISession*, const std::string&)>;
 using tfOnSessionCreateAfterCb = std::function<void(ISession*)>;
 using tfOnSessionDestoryAfterCb = std::function<void(ISession*)>;
 using tfOnSessionDestoryBeforeCb = std::function<void(ISession*)>;
 class IServer {
 public:
  virtual const std::string& Addr() const = 0;
  virtual const EnIPV& Ipv() const = 0;
  virtual const EnSocketType& SocketType() const = 0;
  virtual bool Start(const EnSocketType&, const EnIPV&, const std::string&) = 0;
  virtual void Stop() = 0;
  virtual bool Ready() const = 0;
  virtual void SessionCreateAfterCb(const tfOnSessionCreateAfterCb&) = 0;
  virtual void SessionDestoryAfterCb(const tfOnSessionDestoryAfterCb&) = 0;
  virtual void SessionDestoryBeforeCb(const tfOnSessionDestoryBeforeCb&) = 0;
  virtual void MessageCb(const tfOnServerMessage&) = 0;
  virtual void Write(const unsigned long long&, const std::string&) = 0;
  virtual void Release() const = 0;
 };

 class ILibuv {
 public:
  virtual IClient* CreateClient() = 0;
  virtual IServer* CreateServer() = 0;

 protected:
  ILibuv() {}
  ~ILibuv() {}
 public:
  inline static ILibuv* CreateInterface(const char* module_pathname);
  inline static void DestoryInterface(ILibuv*);
 protected:
  void* hModule = nullptr;
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
 };

 inline void ILibuv::DestoryInterface(ILibuv* instance) {
  do {
   if (!instance)
    break;
   if (!instance->hModule || !instance->api_object_uninit)
    break;
   HMODULE freeMod = reinterpret_cast<HMODULE>(instance->hModule);
   instance->api_object_uninit();
   instance = nullptr;
   ::FreeLibrary(freeMod);
   freeMod = nullptr;
  } while (0);
 }
 inline ILibuv* ILibuv::CreateInterface(const char* module_pathname) {
  ILibuv* result = nullptr;
  HMODULE hModule = nullptr;
  do {
   if (!module_pathname)
    break;
   hModule = ::LoadLibraryA(module_pathname);
   if (!hModule)
    break;
   auto api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hModule, "api_object_init"));
   auto api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hModule, "api_object_uninit"));
   if (!api_object_init || !api_object_uninit)
    break;
   result = reinterpret_cast<decltype(result)>(api_object_init(nullptr, 0));
   if (!result)
    break;
   result->hModule = hModule;
   result->api_object_init = api_object_init;
   result->api_object_uninit = api_object_uninit;
  } while (0);
  if (nullptr == result && hModule != nullptr) {
   ::FreeLibrary(hModule);
   hModule = nullptr;
  }
  return result;
 }

}///namespace libuvpp

/// /*新生®（上海）**/
/// /*2022_10_09T00:53:53.1398001Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___19A77F54_4FFC_418D_BFEB_0CA3C94A278D__HEAD__

