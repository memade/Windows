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
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

#pragma pack(push,1)
 typedef struct tagDockingData {
  struct tagYXInstallData {//!@ 游戏游戏盒安装数据
   unsigned int VipLevel;
   wchar_t ResId[256];
   wchar_t ResName[512];
   wchar_t ResIcoUrl[2084];
   int ResStyle;
   int InternalType;
   int LocalType;
   tagYXInstallData() { ::memset(this, 0x00, sizeof(*this)); }
  };
  tagYXInstallData YXInstallData;
#if 0
  xlapi_3_2_2_30::DownTaskParam* pXL_DownTaskParam;
  bool Verify() const {
   bool result = false;
   do {
    if (!pXL_DownTaskParam)
     break;
    if (pXL_DownTaskParam->szTaskUrl[0] == 0)
     break;
    result = true;
   } while (0);
   return result;
  }
#endif
  tagDockingData() { ::memset(this, 0x00, sizeof(*this)); }
 }DockingData, DOCKINGDATA, * PDOCKINGDATA;

 /*@tagProtocolExtractInitData
* When the decompression process is ready, send the data structure.
* When the decompression process completes its task, send this data structure.
* This structure data is sent when the decompression process terminates the task
* abnormally or completes the task, normally sending the 'ExtractProgress' message ID
*/
 typedef struct tagExtractRouteData {
  unsigned long ExtractProcessId;
  unsigned long ExtractMessageThreadId;
  unsigned long long ExtractTargetDataTotalSize;
  unsigned long long ExtractFinishDataTotalSize;
  char ExtractPath[_MAX_PATH];
  char ExtractTargetDataPathname[_MAX_PATH];
  char ExtractFailedReason[256];

  tagExtractRouteData() { ::memset(this, 0x00, sizeof(*this)); }
  void operator=(const tagExtractRouteData& obj) { ::memcpy(this, &obj, sizeof(*this)); }
 }EXTRACTROUTEDATA, ExtractRouteData, * PEXTRACTROUTEDATA;

#pragma pack(pop)

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

 const size_t LENTASKMANMSG = sizeof(TASKMANMSG);


 typedef struct tagExtractProgressInfo final {
  long long extract_total;
  long long extract_current;
  long long extract_percentage;
  long long extract_time_s;
  tagExtractProgressInfo() { ::memset(this, 0x00, sizeof(*this)); }
 }ExtractProgressInfo,EXTRACTPROGRESSINFO,*PEXTRACTPROGRESSINFO;
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
  virtual bool operator<<(const DockingData&) = 0;
  virtual void DownPath(const std::string&) = 0;
  virtual void DownPathname(const std::string&) = 0;
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
  virtual void Release() const = 0;
  virtual bool Preparation() = 0;
  virtual bool Perform() = 0;
  virtual bool Install() = 0;
 }ITaskNode;

 class IConfigure {
 public:
  /// 开机自动启动
  virtual bool StartsAutomaticallyWhenStarts() const = 0;
  /// 启动时自动下载上次未完成的下载
  virtual bool LastIncompleteDownloadIsDownloadedAutomaticallyAtStartup() const = 0;
  /// 下载完成后自动安装游戏
  virtual bool AutomaticallyInstalledAfterDownloading() const = 0;
  /// 打开后不显示推广广告
  virtual bool NoPromotionalAdsAreDisplayedWhenOpened() const = 0;
  /// 每次关闭时不再提醒我
  virtual bool NoMoreRemindersEverytimeItClosesInClickCloseBtn() const = 0;
  /// 最小化到系统托盘区
  virtual bool MinimizeToSystemTrayInClickCloseBtn() const = 0;
  /// 立即退出
  virtual bool ExitImmediatelyInClickCloseBtn() const = 0;
  /// 安装包存储路径
  virtual const std::string& PathForStoringTheInstallationPackage() const = 0;
  /// 安装路径
  virtual const std::string& ProgramInstallationPath() const = 0;
  /// 不限制下载速度
  virtual bool NolimitOnDownloadSpeed() const = 0;
  /// 下载速度阀值
  virtual const unsigned int& DownloadSpeedThreshold() const = 0;
  /// 自动下载安装包至默认路径
  virtual bool InstallationPackageAutomaticallyDownloadedToDefaultPath() const = 0;
  /// 安装包保留天数
  virtual const unsigned int& TheInstallationPackageIsReservedDays() const = 0;
  /// 下载完成后关机
  virtual bool ShutDownAfterDownloading() const = 0;
  /// 关闭延迟分钟数
  virtual const unsigned int& DisableDelayInMinutes() const = 0;
 };

 using tfTaskResultStatusCb = std::function<void(ITaskNode*)>;

 class IPCHacker {
 public:
  virtual bool Bit7zArchiveProcess() = 0;
  virtual bool ZipArchiveProcess() = 0;
 public:
  virtual void ParentHwndSet(const UIType&, const HWND&) = 0;
  virtual void* UIGetHwnd(const UIType&) const = 0;
  virtual HWND UICreate(const UIType&, const bool& show) = 0;
  virtual void UIShow(const UIType&, const bool& show) = 0;
  virtual bool UIShowIs(const UIType&) const = 0;
  virtual void UIRefresh() const = 0;
  virtual void UIPositionSet(const UIType&, const ::tagPOINT&, const ::tagSIZE&) = 0;
  //!@ 状态栏的进度条显示控制
  virtual void UIShowStatusbarProgressCtrl(const bool&) = 0;
  virtual ITaskNode* TaskCreate(const TypeID&) = 0;
  virtual bool TaskAction(const TypeID&, const EnActionType&) = 0;
  virtual bool TaskDestory(ITaskNode*) = 0;
  virtual bool TaskPerform(ITaskNode*) = 0;
  virtual const std::wstring& SystemDirectoryW() const = 0;
  virtual const std::string& SystemDirectoryA() const = 0;
  virtual const std::wstring& CurrentUserDirectoryW() const = 0;
  virtual const std::string& CurrentUserDirectoryA() const = 0;
  virtual const IConfigure* SystemConfigureGet() const = 0;
  virtual bool OnDockingFormDockingData(DockingData*) = 0;
  virtual bool OnDockingFormHost(const std::wstring& json_data, const tfDockingMessageCb&) = 0;

  //!@ OpenResourceCreateDaemonNode
  //!@ 1. Create a desktop shortcut icon (*customization)
  //!@ 2. Create resource(*executable file) process.
  //!@ 3. Join the Resource service daemon package
  virtual bool OpenResourceCreateDaemonNode(const std::string&) = 0;

  virtual void RegisterTaskResultStatusCallback(const tfTaskResultStatusCb&) = 0;
 protected:
  void* hModule = nullptr;
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
  //-------------------------------------------------------------------------------------------------------
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


