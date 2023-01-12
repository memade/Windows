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
#if !defined(__64986266_3CC9_4313_8BE3_B64555A2E46A__)
#define __64986266_3CC9_4313_8BE3_B64555A2E46A__

#include <dllinterface.h>

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

 typedef enum class EnRequestAction : unsigned long long {
  Normal = 0x0000,
  Start = 0x0010,
  Running = 0x0011,
  Stop = 0x0020,
  Stopping = 0x0021,
  Stopped = 0x0022,
  Suspend = 0x0023,
  Reset = 0x0030,
  Remove = 0x0040,
  Finish = 0x0050,
  Finished = 0x0051,
 }EnRequestStatus;

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

 enum class EnResumeFromLargeMode : int {
  Addup,
  NoAddup,
 };

 class IProgressInfo {//!@ Download or upload progress data
 public:
  virtual const double& total() const = 0;
  virtual const double& current() const = 0;
  virtual const double& speed_s() const = 0;
  virtual const double& percentage() const = 0;
  virtual const long long& time_s() const = 0;
 };

 class IResponse {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual void* RoutePtr() const = 0;
  virtual const std::string& WhatRequest() const = 0;
  virtual const std::string& WhatResponse() const = 0;
  virtual const std::string& CachePathname() const = 0;
  virtual const unsigned int& CurlCode() const = 0;
  virtual const unsigned int& CurlMsg() const = 0;
  virtual const long& HttpCode() const = 0;
  virtual const std::string& OriginalRequestUrl() const = 0;
  virtual const std::string& FixedRequestUrl() const = 0;
  virtual const std::string& Body() const = 0;
  virtual const TypeHeaders& ResponseHeaders() const = 0;
  virtual const size_t& ContentLength() const = 0;
  virtual bool ResultFinal() const = 0;
  virtual EnRequestType RequestType() const = 0;
  virtual EnRequestAction Status() const = 0;
  virtual long long TargetTotalSize() const = 0;
  virtual long long ResumeFromLarge() const = 0;
  virtual EnResumeFromLargeMode ResumeFromLargeMode() const = 0;
  virtual long long MaxRecvSpeedLarge() const = 0;
 protected:
  IResponse() {}
  ~IResponse() {}
 };

 using tfFinishCb = std::function<void(const IResponse*)>;
 using tfProgressCb = std::function<ProgressActionType(const IProgressInfo*, const IProgressInfo*)>;
 class IRequest {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual void Verbose(const bool&) = 0;
  virtual void Header(const bool&) = 0;
  virtual void RequestType(const EnRequestType&) = 0;
  /// 1.Repair the URL
  /// 2.If need escape so escape handle.
  virtual void RequestUrl(const std::string&) = 0;
  virtual void HeadersSet(const TypeHeaders&) = 0;
  virtual void HeadersAdd(const std::string&) = 0;
  virtual void RoutePtr(void*) = 0;
  virtual void* RoutePtr() const = 0;
  virtual void Action(const EnRequestAction&) = 0;
  virtual EnRequestStatus Status() const = 0;
  virtual void FinishCb(const tfFinishCb&) = 0;
  virtual void ProgressCb(const tfProgressCb&) = 0;
  /// If this option is enabled, there will be no callbacks
  virtual void EnableWriteStream(const bool&) = 0;
  virtual bool CachePathname(const std::string&) = 0;
  virtual void MaxRecvSpeedLarge(const long long&) = 0;
  virtual void ResumeFromLarge(const long long&) = 0;
  //!@ This method is not applicable
  virtual void ResumeFromLargeMode(const EnResumeFromLargeMode&) = 0;
  virtual long long ResumeFromLarge() const = 0;
  virtual long long LastDownSize() const = 0;
  virtual long long TargetTotalSize() const = 0;
  virtual const IResponse* ResponseGet() const = 0;
 protected:
  IRequest() {}
  ~IRequest() {}
 };

 class ILibcurlpp : public shared::InterfaceDll<ILibcurlpp> {
 protected:
  ILibcurlpp() {}
  ~ILibcurlpp() {}
 public:
  virtual IRequest* CreateRequest(const TypeIdentify&) = 0;
  virtual void DestoryRequest(const TypeIdentify&) = 0;
  virtual void DestoryRequest(const std::vector<TypeIdentify>&) = 0;
  virtual IRequest* GetRequest(const TypeIdentify&) const = 0;
  virtual void Perform(IRequest*) const = 0;
  virtual void PerformM(const std::vector<IRequest*>&) const = 0;
  virtual void Release() const = 0;
  virtual bool Start() = 0;
  virtual void Stop() = 0;
 };

}///namespace libcurlpp

/// /*_ Memade®（新生™） _**/
/// /*_ Fri, 09 Dec 2022 00:03:19 GMT _**/
/// /*_____ https://www.skstu.com/ _____ **/
#endif///__64986266_3CC9_4313_8BE3_B64555A2E46A__

