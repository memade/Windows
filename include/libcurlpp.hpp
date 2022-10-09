/*===- libcurlpp.hpp - Wrapper for 'extern "C"' ---------------*- C -*-=======*\
|*                                                                                                                        *|
|* Part of the Libcurlpp® Project, under the Apache License v2.0 with Libcurlpp®*|
|* Exceptions.                                                                                                      *|
|* See http://skstu.com/libcurlpp/LICENSE.txt for license information.                 *|
|* SPDX-License-Identifier: Apache-2.0 WITH LIBCURLPP-exception                     *|
|*                                                                                                                        *|
|*===----------------------------------------------------------------------====== *|
|*                                                                                                                        *|
|* This file is a summary of the interface for this project.                                      *|
|*                                                                                                                        *|
\*===----------------------------------------------------------------------======*/
#if !defined(INC_H___0C762477_855F_488C_8D6A_65BDA5BAC3CF__HEAD__)
#define INC_H___0C762477_855F_488C_8D6A_65BDA5BAC3CF__HEAD__

/// Projects are compiled using static libraries
#define ENABLE_STATIC_COMPILATION 0



#if ENABLE_STATIC_COMPILATION
#ifdef SHARED_API
#undef SHARED_API
#endif
#ifdef _USRDLL
#undef _USRDLL
#endif
#else
#ifndef SHARED_API
#define SHARED_API
#endif
#ifndef _USRDLL
#define _USRDLL
#endif
#endif


namespace libcurlpp {

 using TypeIdentify = unsigned long long;
 using TypeHeaders = std::list<std::string>;
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

 enum class EnRequestType : unsigned long long {
  //!@ HTTP 1.0
  REQUEST_TYPE_GET = 0x0000,
  REQUEST_TYPE_POST = 0x0001,
  REQUEST_TYPE_HEAD = 0x0002,
  //!@ HTTP 1.1
  REQUEST_TYPE_OPTIONS = 0x0003,
  REQUEST_TYPE_PUT = 0x0004,
  REQUEST_TYPE_PATCH = 0x0005,
  REQUEST_TYPE_DELETE = 0x0006,
  REQUEST_TYPE_TRACE = 0x0007,
  REQUEST_TYPE_CONNECT = 0x0008,

  REQUEST_TYPE_BEGIN = REQUEST_TYPE_GET,
  REQUEST_TYPE_END = REQUEST_TYPE_CONNECT,
 };

 enum class EnRequestStatus : unsigned long long {
  Normal = 0x0000,
  Running = 0x0001,
  Stopping = 0x0002,
  Finished = 0x0003,
  Stopped = 0x0004,
  Suspend = 0x0005,
 };

 enum class EnRequestAction : unsigned long long {
  Normal = 0x0000,
  Start = 0x0001,
  Stop = 0x0002,
  Reset = 0x0003,
  Remove = 0x0004,
  Finish = 0x0005,
 };

 enum class EnDownSuccessType : unsigned long long {
  /*HTTP OK*/
  Success = 0xC8,
  /*Partial Content success.*/
  PartialContentSuccess = 0xCE,
  /*Requested Range Not Satisfiable*/
  RequestedRangeNotSatisfiableSuccess = 0x1A0,
 };

 typedef enum class EnProgressActionType : int {
  Continue = 0x0000,
  Break = 0x0001,
 }ProgressActionType;


 class IProgressInfo {
 public:
  virtual const double& total() const = 0;
  virtual const double& current() const = 0;
  virtual const double& speed_s() const = 0;
  virtual const double& percentage() const = 0;
  virtual const long long& time_s() const = 0;
 };

 class IResponse {
 public:
  virtual const std::string& OriginalRequestUrl() const = 0;
 protected:
  IResponse() {}
  ~IResponse() {}
 };

 using tfFinishCb = std::function<void(const IResponse*)>;
 using tfProgressCb = std::function<ProgressActionType(const IProgressInfo*, const IProgressInfo*)>;
 class IRequest {
 public:
  virtual TypeIdentify Identify() const = 0;
  virtual void Default() = 0;
  virtual void Verbose(const bool&) = 0;
  virtual void Header(const bool&) = 0;
  virtual void RequestType(const EnRequestType&) = 0;
  /// 1.Repair the URL
  /// 2.If need escape so escape handle.
  virtual void RequestUrl(const std::string&) = 0;
  virtual void HeadersSet(const TypeHeaders&) = 0;
  virtual bool HeadersAdd(const std::string&) = 0;
  virtual void RoutePtr(void*) = 0;
  virtual void* RoutePtr() const = 0;
  virtual void Action(const EnRequestAction&) = 0;
  virtual const EnRequestStatus& Status() const = 0;
  virtual void FinishCb(const tfFinishCb&) = 0;
  virtual void ProgressCb(const tfProgressCb&) = 0;
  /// If this option is enabled, there will be no callbacks
  virtual void EnableWriteStream(const bool&) = 0;
  virtual bool CachePathname(const std::string&) = 0;
  virtual void MaxRecvSpeedLarge(const long long&) = 0;
 protected:
  IRequest() {}
  ~IRequest() {}
 };

 class IHttpApi {
 protected:
  IHttpApi() {}
  ~IHttpApi() {}
 public:
  inline static IHttpApi* CreateInterface(const char* module_pathname);
  inline static void DestoryInterface(IHttpApi*);
 public:
  virtual IRequest* CreateRequest() = 0;
  virtual void DestoryRequest(const IRequest*) = 0;
  virtual void DestoryRequest(const std::vector<IRequest*>&) = 0;
  /// !!! Please use caution to prevent blocking
  virtual IRequest* SearchRequest(const TypeIdentify&) const = 0;
  virtual void Perform(IRequest*) const = 0;
  virtual void PerformM(const std::vector<IRequest*>&) const = 0;
 protected:
  void* hModule = nullptr;
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
 };
 inline void IHttpApi::DestoryInterface(IHttpApi* instance) {
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
 inline IHttpApi* IHttpApi::CreateInterface(const char* module_pathname) {
  IHttpApi* result = nullptr;
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

}///namespace libcurlpp

/// /*新生®（上海）**/
/// /*2022_09_07T01:08:08.0147270Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___0C762477_855F_488C_8D6A_65BDA5BAC3CF__HEAD__
