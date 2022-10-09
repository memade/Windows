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

 typedef enum class EnDownActionType : unsigned long long {
  Normal = 0x0000, // empty action is no handle.
  Pause = 0x0001,// 暂停任务
  Start = 0x0002,// 开始
  Stop = 0x0003,// 暂停或停止任务
  Remove = 0x0004,// 移除任务
  Append = 0x0005,// 添加任务
  Reset = 0x0006,// 任务重置
  Ready = 0x0007,// 任务已就绪
  Preparation = 0x0008,// 装备任务
  Waitinline = 0x0009,// 排队等待
  Runtime = 0x000A,// 任务运行时|运行中|正在工作
  Beworking = 0x000A,
  Running = 0x000A,

  Begin = Normal,
  End = Running,
 }DownActionType;

 typedef enum class EnTaskStatus : unsigned long long {
  Normal = 0x0000, // or Downloading .
  Success = 0x0001,
  Failed = 0x0002,
  Error = 0x0003,
  Pause = 0x0004,
  InPreparation = 0x0005,
 }TaskStatus;

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
 using tfDockingMessageCb = std::function<void(const EnDockingMessageType&, const EnDockingResultType&, const std::string&)>;


 typedef class IDownTaskNode {
 public:
  virtual void ID(const TypeID&) = 0;
  virtual TypeID ID() const = 0;
  virtual void Url(const std::string&) = 0;
  virtual const std::string& Url() const = 0;
  virtual void Logo(const std::string&) = 0;
  virtual const std::string& Logo() const = 0;
  virtual EnTaskStatus Status() const = 0;
  virtual void Action(const DownActionType&) = 0;
  virtual bool Verify() const = 0;
  virtual const unsigned int& VipLevel() const = 0;
  virtual void VipLevel(const unsigned int&) = 0;
  virtual void LocalResDir(const std::string&) = 0;
  virtual const std::string& LocalResDir() const = 0;
  virtual void Name(const std::string&) = 0;
  virtual const std::string& Name() const = 0;
  virtual bool operator<<(const DockingData&) = 0;
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

 class IPCHacker {
 public:
  virtual bool Bit7zArchiveProcess() = 0;
  virtual bool ZipArchiveProcess() = 0;
  virtual void* GetLibcurlppHandle() const = 0;
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
  virtual ITaskNode* TaskCreate() = 0;
  virtual bool TaskAction(const TypeID&, const DownActionType&) = 0;
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


