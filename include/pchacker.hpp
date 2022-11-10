/*===- pchacker.hpp - Wrapper for 'extern "C"' ---------------*- C -*-=========*\
|*                                                                             *|
|* Part of the PCHacker® Project, under the Apache License v2.0 with PCHacker® *|
|* Exceptions.                                                                 *|
|* See http://skstu.com/pchacker/LICENSE.txt for license information.          *|
|* SPDX-License-Identifier: Apache-2.0 WITH PCHACKER-exception                 *|
|*                                                                             *|
|*===----------------------------------------------------------------------=== *|
|*                                                                             *|
|* This file is a summary of the interface for this project.                   *|
|*                                                                             *|
\*===----------------------------------------------------------------------====*/
#if !defined(INC_H___E0CDC3C1_8FD7_4C35_8638_D30703A362DE__HEAD__)
#define INC_H___E0CDC3C1_8FD7_4C35_8638_D30703A362DE__HEAD__

#include <functional>

namespace pchacker {
 using TypeID = unsigned long long;
 using TypeIdentify = unsigned long long;
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);
 using tf_api_result_std_string_cb = std::function<void(const std::string&)>;

 namespace libuvpp {

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
  };

  class ILibuv {
  public:
   virtual IClient* CreateClient() = 0;
   virtual IServer* CreateServer() = 0;
   virtual void DestoryServer(IServer*&) = 0;
  protected:
   ILibuv() {}
   ~ILibuv() {}
  };

 }///namespace libuvpp

 namespace libcurlpp {
  using TypeHeaders = std::list<std::string>;

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

  class ILibcurlpp {
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


 const unsigned long long TYPE_TASKMAN_MSG_HELLO = 0x00100201;
 const unsigned long long TYPE_TASKMAN_MSG_WELCOME = TYPE_TASKMAN_MSG_HELLO;
 const unsigned long long TYPE_TASKMAN_MSG_DOWNLOAD = TYPE_TASKMAN_MSG_WELCOME + 1;
 const unsigned long long TYPE_TASKMAN_MSG_DOWNLOAD_NOTIFY = TYPE_TASKMAN_MSG_WELCOME + 1;
 const unsigned long long TYPE_TASKMAN_MSG_EXTRACT = TYPE_TASKMAN_MSG_WELCOME + 2;
 const unsigned long long TYPE_TASKMAN_MSG_EXTRACT_NOTIFY = TYPE_TASKMAN_MSG_WELCOME + 2;
 const unsigned long long TYPE_TASKMAN_MSG_EXIT = TYPE_TASKMAN_MSG_WELCOME + 3;
 const unsigned long long TYPE_TASKMAN_MSG_DOWN_FAILED = TYPE_TASKMAN_MSG_WELCOME + 4;
 const unsigned long long TYPE_TASKMAN_MSG_DOWN_SUCCESS = TYPE_TASKMAN_MSG_WELCOME + 5;
 const unsigned long long TYPE_TASKMAN_MSG_EXTRACT_FAILED = TYPE_TASKMAN_MSG_WELCOME + 6;
 const unsigned long long TYPE_TASKMAN_MSG_EXTRACT_SUCCESS = TYPE_TASKMAN_MSG_WELCOME + 7;

 const unsigned long long TYPE_CARDPACKAGE_MSG_HELLO = 0x00200201;
 const unsigned long long TYPE_CARDPACKAGE_MSG_WELCOME = TYPE_TASKMAN_MSG_WELCOME;
 const unsigned long long TYPE_CARDPACKAGE_MSG_CHECK_FAILED = TYPE_CARDPACKAGE_MSG_HELLO + 1;
 const unsigned long long TYPE_CARDPACKAGE_MSG_CHECK_SUCCESS = TYPE_CARDPACKAGE_MSG_HELLO + 2;
 const unsigned long long TYPE_CARDPACKAGE_MSG_EXIT = TYPE_CARDPACKAGE_MSG_HELLO + 3;

 enum class EnTaskType : unsigned int {
  Unknow = 0,
  Downloader = 1,
  Extract = 2,
  Notify = 3,
 };

 enum class EnLocalMessage : unsigned int {
  Begin = WM_USER + 0x2710 + 1,
  ProcessActivate = Begin + 0,
  ProcessQuit = Begin + 1,
  ThreadActivate = Begin + 2,
  ThreadQuit = Begin + 3,
  ExtractProgress = Begin + 4,
  ExtractReady = Begin + 5,
  ExtractSuccess = Begin + 6,
  ExtractFailed = Begin + 7,
  ExtractPerform = Begin + 8,
  ExtractBreak = Begin + 9,

  End = ExtractProgress,
  Total = 5,
 };

 typedef enum class EnUIType : unsigned long long {
  UnknownUI = 0x0000,
  DownManagerUI = 0x1000,
  SystemConfigureUI = 0x2000,
  DownManagerUI2 = 0x2001,
  StatusBarUI = 0x3000,
  MessageBoxUI = 0x4000,
  MainUI = 0x5000,
  WXUI_MAIN = 0x6000,

  Begin = UnknownUI,
  End = WXUI_MAIN,
 }UIType;

 typedef enum class EnActionType : unsigned long long {
  Normal = 0x0000, // empty action is no handle.
  DownPause = 0x0001,// 暂停任务
  DownStart = 0x0002,// 开始
  DownStop = 0x0003,// 暂停或停止任务
  DownRemove = 0x0004,// 移除任务
  DownAppend = 0x0005,// 添加任务
  DownReset = 0x0006,// 任务重置
  DownPreparation = 0x0008,// 装备任务
  DownWaitinline = 0x0009,// 排队等待

  DownFailed = 0x000C,// 下载失败
  DownReady = 0x0007,// 任务已就绪
  DownStoping = 0x0013,//暂停或停止任务 进行中
  DownRuntime = 0x000A,// 任务运行时|运行中|正在工作
  DownBeworking = 0x000A,
  DownRunning = 0x000A,
  DownInPreparation = 0x0018,// 装备任务
  DownStopd = 0x0023,//任务停止完成
  DownFinished = 0x000B,// 下载完成了的


  InstallAppend = 0x0100,// 添加安装任务
  InstallStart = 0x0101,// 开始安装
  InstallBeworking = 0x0111,// 正在安装|安装工作进行中
  InstallStop = 0x0102,// 停止安装
  InstallStoping = 0x0112,// 正在停止安装
  InstallStopd = 0x0122,// 停止完成
  InstallFailed = 0x0114,// 安装失败
  InstallFinish = 0x0104,// 安装完成(安装逻辑执行结束)
  InstallPreparation = 0x0115,//!@ 安装准备
  InstallInPreparation = 0x0125,//!@ 安装准备中
  InstallReady = 0x0135,//!@ 安装准备就绪


 }EnActionType, EnStatusType;

 enum class EnDockingMessageType : unsigned long long {
  Undefined = 0x0000,
  LocalInstalled = 0x0001,
  TriggerDownload = 0x0002,
  TriggerStart = 0x0003,
 };

 enum class EnDockingResultType : unsigned long long {
  Success = 0x0000,
  Failed = 0x0001,
 };

#pragma pack(push,1)

 typedef struct tagTaskmanMsg {
  enum class EnTaskmanStatus : unsigned int {
   Normal = 0,
   Tasking = 1,
   Success = 2,
   Finished = 3,
  };
  struct tagDetails final {
   char StartupPEPathname[260];
   char ProductLogoPathname[260];//!@ ico pathname
   char ProductName[260];
   unsigned long ProductID;
   tagDetails() { ::memset(this, 0x00, sizeof(*this)); }
  };
  TypeID TaskId;
  unsigned long ProcessId;
  char InPathname[260];
  char OutPath[260];
  unsigned long extract_progress;
  unsigned long long extract_target_total_size;
  unsigned long long extract_total_size;
  unsigned long long extract_current_extract_total_size;
  EnTaskmanStatus task_status;
  tagDetails Details;
  tagTaskmanMsg() {
   ::memset(this, 0x00, sizeof(*this));
   task_status = EnTaskmanStatus::Normal;
  }
 }TASKMANMSG, * PTASKMANMSG;

 struct IPacketHead final {
  unsigned long long identify_head;
  unsigned long size_head_std;//!@ Contains data[1] size.(+sizeof(char))
  unsigned long size_head_real;
  unsigned long long command;
  unsigned long size_data;
  unsigned long size_total;
  unsigned long long identify_tail;
  char data[1];
  IPacketHead() {
   ::memset(this, 0x00, sizeof(*this));
   identify_head = 0xFAC9C2D0;
   identify_tail = 0xB4B4AAC1;
  }
  bool Verify() const {
   return identify_head == 0xFAC9C2D0 && identify_tail == 0xB4B4AAC1;
  }
 };

 const size_t LENTASKMANMSG = sizeof(TASKMANMSG);

 typedef struct tagExtractProgressInfo final {
  long long extract_total;
  long long extract_current;
  long long extract_percentage;
  long long extract_time_s;
  tagExtractProgressInfo() { ::memset(this, 0x00, sizeof(*this)); }
 }ExtractProgressInfo, EXTRACTPROGRESSINFO, * PEXTRACTPROGRESSINFO;

#pragma pack(pop)


 using tfDockingMessageCb = std::function<void(const EnDockingMessageType&, const EnDockingResultType&, const std::string&)>;

 class ITaskResultStatus {
 public:
  virtual const double& down_total() const = 0;
  virtual const double& down_current() const = 0;
  virtual const double& down_speed_s() const = 0;
  virtual const double& down_percentage() const = 0;
  virtual const long long& down_time_s() const = 0;
  virtual const long long& extract_total() const = 0;
  virtual const long long& extract_current() const = 0;
  virtual const long long& extract_percentage() const = 0;
  virtual const long long& extract_time_s() const = 0;
  virtual const std::string& FinishPath() const = 0;
  virtual const std::string& FinishPathname() const = 0;
  virtual void operator<<(const EXTRACTPROGRESSINFO&) = 0;
  virtual void operator<<(const tagTaskmanMsg::tagDetails&) = 0;
 };

 typedef class IDownTaskNode {
 public:
  virtual const TypeID& ID() const = 0;
  virtual void Url(const std::string&) = 0;
  virtual const std::string& Url() const = 0;
  virtual void LogoUrl(const std::string&) = 0;
  virtual const std::string& LogoUrl() const = 0;
  virtual void LogoPathname(const std::string&) = 0;
  virtual const std::string& LogoPathname() const = 0;
  virtual EnActionType Status() const = 0;
  virtual void Action(const EnActionType&) = 0;
  virtual bool Verify() const = 0;
  virtual const unsigned int& VipLevel() const = 0;
  virtual void VipLevel(const unsigned int&) = 0;
  virtual void LocalResDir(const std::string&) = 0;
  virtual const std::string& LocalResDir() const = 0;
  virtual void Name(const std::string&) = 0;
  virtual const std::string& Name() const = 0;
  virtual void FinishPath(const std::string&) = 0;
  virtual const std::string& FinishPath() const = 0;
  virtual void FinishPathname(const std::string&) = 0;
  virtual const std::string& FinishPathname() const = 0;
  virtual void DownCacheFilePathname(const std::string&) = 0;
  virtual const std::string& DownCacheFilePathname() const = 0;
  virtual void OpenCommandLine(const std::string&) = 0;
  virtual void RoutePtr(void*) = 0;
  virtual void* RoutePtr() const = 0;
  virtual void BindUI(void*) = 0;
  virtual void* BindUI() const = 0;
  virtual void BindUI2(void*) = 0;
  virtual void* BindUI2() const = 0;
  virtual void BindPtr(void*) = 0;
  virtual void* BindPtr() const = 0;
  virtual void DownLimitSpeed(const long long&/*b*/) = 0;
  virtual ITaskResultStatus* Result() const = 0;
  virtual void operator<<(const EXTRACTPROGRESSINFO&) = 0;
  virtual void operator<<(const tagTaskmanMsg::tagDetails&) = 0;
  virtual bool Perform() = 0;
  virtual bool Install() = 0;
  virtual bool FinalResult() const = 0;
  virtual void Release() const = 0;
 protected:
  IDownTaskNode() {}
  ~IDownTaskNode() {}
 }ITaskNode;

 enum class EnEncryptionRSAType : unsigned long long {
  PKCS1 = 1,
  PKCS8 = 8,
 };
 class IEncryption {
 public:
  virtual bool MD5(const std::string input, std::string& output, const std::vector<std::uint8_t> seed = {}, const bool& hex_output = true, const bool& base64_output = false) const = 0;
  virtual bool HMAC_SHA1(const std::string& input, std::string& output, const std::string& seed, const bool& hex_output = true, const bool& base64_output = false) const = 0;
  virtual bool HMAC_SHA256(const std::string& input, std::string& output, const std::string& seed, const bool& hex_output = true, const bool& base64_output = false) const = 0;
  virtual bool SHA256(const std::string& inData, const bool& isHex, std::string& outData) const = 0;
  virtual bool Base64Encode(const std::string& input, std::string& output, const bool& multiline = false) const = 0;
  virtual bool Base64Decode(const std::string& input, std::string& output, const bool& multiline = false) const = 0;
  virtual bool RsaGenerate(const EnEncryptionRSAType&, const std::string& pwd, std::string& outKeyPublic, std::string& outKeyPrivate) const = 0;
  virtual bool RsaPrivateToPublic(const EnEncryptionRSAType&, const std::string& pwd, const std::string& inKeyPrivate, std::string& outKeyPublic) const = 0;
  virtual bool RsaDERPublic(const EnEncryptionRSAType&, const std::string& keyPublic, const std::string& pwd, std::string& outDER) const = 0;
  virtual bool RsaEncode(const EnEncryptionRSAType&, const std::string& pwd, const std::string& keyPublic, const std::string& keyPrivate, const std::string& inData, std::string& outData) const = 0;
  virtual bool RsaDecode(const EnEncryptionRSAType&, const std::string& pwd, const std::string& keyPublic, const std::string& keyPrivate, const std::string& inData, std::string& outData) const = 0;
 };

 class ICom {
 public:
  virtual bool CreateLnk(const std::string& szPath, const std::string& szLink, const std::string& szIcoPath = "", const std::string& szArgument = "") const = 0;
  virtual bool CreateLnkUrl(
   const std::string& BindLnkUrl,
   const std::string& BindLnkPathname, /*xxx.lnk*/
   const std::string& BindLnkIcoPathname/*xxx.ico | xxx.png*/) const = 0;
 };

 class IWin {
 public:
  virtual bool IsX64(const std::string&) const = 0;
  virtual bool IsPEDLL(const std::string&) const = 0;
  virtual bool IsPEEXE(const std::string&) const = 0;
  virtual bool CreateDirectoryA(const std::string&) const = 0;
  virtual void GetModulePathnameA(const tf_api_result_std_string_cb&, const HINSTANCE& hModule = nullptr) const = 0;
  virtual void GetModulePathA(const tf_api_result_std_string_cb&, const HINSTANCE& hModule = nullptr) const = 0;
  virtual void RealtimeSpeed(const tf_api_result_std_string_cb&, const long long& speed_bype_size, const bool& divide = true) const = 0;
  virtual void TimePeriodUnMade(const tf_api_result_std_string_cb&, const UINT& TimePeriod) const = 0;
  virtual void ReadFile(const tf_api_result_std_string_cb&, const std::string& FilePathname, const int& OpenMode = std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary) const = 0;
  virtual bool WriteFile(const std::string& FilePathname, const std::string& WriteData, const int& OpenMode = std::ios::binary | std::ios::out | std::ios::trunc) const = 0;
  virtual bool AccessA(const std::string&) const = 0;
  virtual bool GetFileNameAndFormat(const std::string& pathname, std::string& out_name, std::string& out_format) const = 0;
  virtual void GetNameByPathnameA(const tf_api_result_std_string_cb&, const std::string& pathname) const = 0;
  virtual void GetPathByPathnameA(const tf_api_result_std_string_cb&, const std::string& pathname) const = 0;
  virtual void ReadAssignSize(const tf_api_result_std_string_cb&, const std::string& FilePathname, const size_t& assign_size, const int& OpenMode = std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary) const = 0;
  virtual void PathFixedA(const tf_api_result_std_string_cb&, const std::string& PathOrPathname) const = 0;
  virtual void GetSpecialFolderLocationA(const tf_api_result_std_string_cb&, const int& csidl) const = 0;
  virtual bool ProcessCreateA(const std::string& exePathname,
   const std::string& commandline,
   const std::function<void(const HANDLE&, const DWORD&)> create_process_cb,
   const bool& Inheriting_parent_process = false,
   const bool& isShow = false,
   const DWORD& wait_time = 0) const = 0;
  virtual void ParseCommandLineParameters(const std::string& commandline, const std::function<void(const std::map<std::string, std::string>&)>&) const = 0;

  virtual void FindFileAssignPath(
   const std::string& path,
   const std::vector<std::string>& ffname_s,
   std::map<std::string, std::string>& found_s) const = 0;

  virtual void FindFileAssignPathOnce(
   const std::string& path,
   const std::vector<std::string>& ffname_s,
   std::map<std::string, std::string>& found_s) const = 0;

  virtual void EnumFolder(const std::string& Path,
   std::map<std::string, std::string>& Folders,
   std::map<std::string, std::string>& Files,
   const char* FileFilter = "*.*",
   bool bSleepDirect = false,
   const std::function<void(const std::string& pathname, const std::string& identify, const bool& is_directory)>& enumcb = nullptr) const = 0;

  virtual bool PacketMade(const unsigned long long&, const std::string&, std::string&) const = 0;
  virtual size_t PacketUnMade(std::string&, std::vector<std::string>&, const bool& fixed_buffer = true) const = 0;
 };

 class IZip {
 public:
  //!@ Decompress a single compressed file to buffer.
  virtual bool UncompressBuffer(const std::string& zbuffer, std::string& outbuffer) const = 0;
  //!@ Decompress a single compressed file to path.
  virtual bool UncompressBuffer(const std::string& zbuffer, \
   const std::function<bool(const std::string&, const std::string&, bool&)>& uncompress_cb, const std::string& outpath = "") const = 0;
 };

 class IConfigure {
 public:
  /// Project Current path
  virtual void ProjectCurrentPath(const std::string&) = 0;
  virtual const std::string& ProjectCurrentPath() const = 0;
  /// Project logger recorder path
  virtual bool ProjectLoggerRecorderModuleName(const std::string&) = 0;
  virtual const std::string& ProjectLoggerRecorderModuleName() const = 0;
  virtual bool ProjectLoggerRecorderPath(const std::string&) = 0;
  virtual const std::string& ProjectLoggerRecorderPath() const = 0;
  /// The buffer path to download the resource @Cache
  virtual bool DownResourceCachePath(const std::string&) = 0;
  virtual const std::string& DownResourceCachePath() const = 0;
  /// Specifies the installation path of the downloaded resource @Finish
  virtual bool FinishInstalledPath(const std::string&) = 0;
  virtual const std::string& FinishInstalledPath() const = 0;
  /// Prepared resource path before you start downloading @Prepared
  virtual bool DownPreparedResourcePath(const std::string&) = 0;
  virtual const std::string& DownPreparedResourcePath() const = 0;
  /// Address of the local multi-process service
  virtual void LocalServiceTcpAddr(const std::string&) = 0;
  virtual const std::string& LocalServiceTcpAddr() const = 0;
  /// Set the default download buffer file format . For example : [.cache]
  virtual void DefaultDownloadCacheFileFormat(const std::string&) = 0;
  virtual const std::string& DefaultDownloadCacheFileFormat() const = 0;
  /// 
  virtual void EnableLibuvppServer(const bool&) = 0;
  virtual const bool& EnableLibuvppServer() const = 0;
  /// 
  virtual void EnableLibcurlpp(const bool&) = 0;
  virtual const bool& EnableLibcurlpp() const = 0;
 };

 using tfTaskResultStatusCb = std::function<void(ITaskNode*)>;
 using tfOpenResourceCallback = std::function<void(unsigned long long&)>;
 class IPCHacker {
 public:
  virtual IZip* ZipGet() const = 0;
  virtual ICom* ComGet() const = 0;
  virtual IWin* WinGet() const = 0;
  virtual IConfigure* ConfigureGet() const = 0;
  virtual IEncryption* EncryptionGet() const = 0;
  virtual libcurlpp::ILibcurlpp* LibcurlppGet() const = 0;
  virtual libuvpp::ILibuv* LibuvppGet() const = 0;
 public:
  virtual ITaskNode* TaskCreate(const TypeID&) = 0;
  virtual bool TaskAction(const TypeID&, const EnActionType&) = 0;
  virtual void RegisterTaskResultStatusCallback(const tfTaskResultStatusCb&) = 0;
  virtual void RegisterOpenResourceCallback(const tfOpenResourceCallback&) = 0;
  virtual bool Start(const IConfigure*) = 0;
  virtual void Stop() = 0;
 protected:
  void* hModule = nullptr;
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
 protected:
  inline IPCHacker();
  inline ~IPCHacker();
 public:
  inline static IPCHacker* CreateInterface(
   const char* pchacher_pe_pathname, const void* route, unsigned long nroute);
  inline static void DestoryInterface(IPCHacker*& pchacher_obj);
 };
 //////////////////////////////////////////////////////////////////////////////////////////
 inline IPCHacker::IPCHacker() {}
 inline IPCHacker::~IPCHacker() {}
 inline void IPCHacker::DestoryInterface(IPCHacker*& instance) {
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
 inline IPCHacker* IPCHacker::CreateInterface(const char* module_pathname, const void* param = nullptr, unsigned long size_param = 0) {
  IPCHacker* result = nullptr;
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
   result = reinterpret_cast<decltype(result)>(api_object_init(param, size_param));
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



}///namespace pchacker

/// /*新生®（上海）**/
/// /*2022_10_01T04:07:40.6988834Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___E0CDC3C1_8FD7_4C35_8638_D30703A362DE__HEAD__

