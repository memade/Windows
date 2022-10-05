#include "stdafx.h"

namespace shared {
 namespace nt {
  std::string NTObj::__CommandLineGet(const HANDLE& hProcess, const RTL_USER_PROCESS_PARAMETERS& RtlUserProcessParameters,
   const std::function<void(const std::string&)>& rescb) const {
   std::string result;
   do {
    if (!hProcess)
     break;
    SIZE_T dwDummy = 0;
    if (RtlUserProcessParameters.CommandLine.Length <= 0)
     break;
    auto pBuffer = new wchar_t[RtlUserProcessParameters.CommandLine.Length * 2];
    ::memset(pBuffer, 0x00, RtlUserProcessParameters.CommandLine.Length * 2);
    if (::ReadProcessMemory(hProcess, RtlUserProcessParameters.CommandLine.Buffer, pBuffer, RtlUserProcessParameters.CommandLine.Length, &dwDummy)) {
     result = shared::IConv::WStringToMBytes(std::wstring(pBuffer, dwDummy));
     rescb(result);
    }
    SK_DELETE_PTR_BUFFER(pBuffer);
   } while (0);
   return result;
  }
  std::string NTObj::__ImagePathNameGet(const HANDLE& hProcess, const RTL_USER_PROCESS_PARAMETERS& RtlUserProcessParameters,
   const std::function<void(const std::string&)>& rescb) const {
   std::string result;
   do {
    if (!hProcess)
     break;
    SIZE_T dwDummy = 0;
    if (RtlUserProcessParameters.ImagePathName.Length <= 0)
     break;
    auto pBuffer = new wchar_t[RtlUserProcessParameters.ImagePathName.Length * 2];
    ::memset(pBuffer, 0x00, RtlUserProcessParameters.ImagePathName.Length * 2);
    if (::ReadProcessMemory(hProcess, RtlUserProcessParameters.ImagePathName.Buffer, pBuffer, RtlUserProcessParameters.ImagePathName.Length, &dwDummy)) {
     result = shared::IConv::WStringToMBytes(std::wstring(pBuffer, dwDummy));
     rescb(result);
    }
    SK_DELETE_PTR_BUFFER(pBuffer);
   } while (0);
   return result;
  }
  HANDLE NTObj::__OpenPorcess(const DWORD& ProcessId, const ACCESS_MASK& accessMask /*= PROCESS_ALL_ACCESS*/) const {
   HANDLE result = nullptr;
   do {
    if (ProcessId <= 4)
     break;
    shared::Win::EnableDebugPrivilege(true);
    CLIENT_ID clientId;
    clientId.UniqueProcess = reinterpret_cast<HANDLE>(static_cast<size_t>(ProcessId));
    clientId.UniqueThread = nullptr;
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, nullptr, 0, nullptr, nullptr);
    NtOpenProcess(&result, accessMask, &objectAttributes, &clientId);
   } while (0);
   return result;
  }
  bool NTObj::__GetProcessBasicInformation(const HANDLE& hProcess, PROCESS_BASIC_INFORMATION& BasicInformation) const {
   bool result = false;
   do {
    if (!hProcess)
     break;
    auto status = NtQueryInformationProcess(
     hProcess,
     ProcessBasicInformation,
     &BasicInformation,
     sizeof(PROCESS_BASIC_INFORMATION),
     NULL
    );
    result = status == STATUS_SUCCESS;
   } while (0);
   return result;
  }

  bool NTObj::__EnumProcessModules(
   const DWORD& ProcessId,
   const std::function<void(const HANDLE& ProcessHandle, shared::nt::PLDR_DATA_TABLE_ENTRY Entry, PVOID AddressOfEntry, PPEB, bool& itbreak)>& Callback
  ) const {
   bool result = false;
   HANDLE hProcess = nullptr;
   do {
    NTSTATUS status;
    PROCESS_BASIC_INFORMATION basicInfo;
    PPEB_LDR_DATA ldr;
    PEB_LDR_DATA pebLdrData;
    PLIST_ENTRY startLink;
    PLIST_ENTRY currentLink;
    ULONG dataTableEntrySize;
    LDR_DATA_TABLE_ENTRY currentEntry;
    ULONG i;
    bool __break = false;
    hProcess = __OpenPorcess(ProcessId);
    if (!hProcess)
     break;
    status = __GetProcessBasicInformation(hProcess, basicInfo);
    if (!NT_SUCCESS(status))
     break;
    status = NtReadVirtualMemory(hProcess, PTR_ADD_OFFSET(basicInfo.PebBaseAddress, FIELD_OFFSET(PEB, Ldr)), &ldr, sizeof(PVOID), NULL);
    if (!NT_SUCCESS(status))
     break;
    status = NtReadVirtualMemory(hProcess, ldr, &pebLdrData, sizeof(PEB_LDR_DATA), NULL);
    if (!NT_SUCCESS(status))
     break;
    if (!pebLdrData.Initialized)
     break;
    if (m_WindowsVersion.load() >= WINDOWS_8)
     dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WIN8;
    else
     dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WIN7;
    i = 0;
    startLink = reinterpret_cast<decltype(startLink)>(PTR_ADD_OFFSET(ldr, FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModuleList)));
    currentLink = pebLdrData.InLoadOrderModuleList.Flink;
    while (currentLink != startLink && i <= SK_ENUM_PROCESS_MODULES_LIMIT) {
     PVOID addressOfEntry;
     addressOfEntry = CONTAINING_RECORD(currentLink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
     status = NtReadVirtualMemory(
      hProcess,
      addressOfEntry,
      &currentEntry,
      dataTableEntrySize,
      NULL
     );
     if (!NT_SUCCESS(status))
      break;
     if (currentEntry.DllBase) {
      Callback(hProcess, &currentEntry, addressOfEntry, basicInfo.PebBaseAddress, __break);
      if (__break)
       break;
     }
     currentLink = currentEntry.InLoadOrderLinks.Flink;
     i++;
    }
    if (hProcess) {
     NtClose(hProcess);
     hProcess = nullptr;
    }
    result = true;
   } while (0);
   return result;
  }
  void NTObj::__EnumProcessOnce(_Inout_ time_t& max_process_create_time_prev) {
   do {
    decltype(m_ProcessSnapshootSimple) ProcessSnapshootSimpleCache = m_ProcessSnapshootSimple;
    m_ProcessSnapshootSimple.clear();

    __EnumProcess(
     max_process_create_time_prev,
     [&](
      const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>& snapshoot,
      const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>& creations,
      const time_t& max_process_create_time) {
       std::map<DWORD, SYSTEM_PROCESS_INFORMATION> deletions{};
       std::map<DWORD, PROCESSROUTEDATA> routeDeletions{}, routeCreations{};
       do {
        if (m_ProcessSnapshoot.empty()) {
         break;
        }
        deletions = *m_ProcessSnapshoot.src();
        for (auto it = deletions.begin(); it != deletions.end();) {
         auto found = snapshoot.find(it->first);
         if (found != snapshoot.end()) {
          it = deletions.erase(it);
          continue;
         }
         ++it;
        }
       } while (0);

       for (const auto& deletion : deletions) {
        ProcessSnapshootSimpleCache.pop(deletion.first,
         [&](const auto& pid, auto& remove) {
          routeDeletions.emplace(std::make_pair(pid, remove));
         });
       }

       for (const auto& creation : creations) {
        do {
         PROCESSROUTEDATA route;
         route.UniqueProcessId = SK_PROCESSID(creation.second.UniqueProcessId);
         route.InheritedFromUniqueProcessId = SK_PROCESSID(creation.second.InheritedFromUniqueProcessId);
         ProcessSnapshootSimpleCache.search(route.InheritedFromUniqueProcessId,
          [&](const PROCESSROUTEDATA& parent) {
           strcpy_s(route.ParentImageName, sizeof(route.ParentImageName), parent.ImageName);
           strcpy_s(route.ParentImagePathname, sizeof(route.ParentImagePathname), parent.ImagePathname);
          });
         route.CreateTime = creation.second.CreateTime.QuadPart;
         route.UniqueProcessKey = creation.second.UniqueProcessKey;
         route.SessionId = creation.second.SessionId;
         if (creation.second.ImageName.Length > 0)
          strcpy_s(route.ImageName, sizeof(route.ImageName),
           shared::IConv::WStringToMBytes(std::wstring(creation.second.ImageName.Buffer, creation.second.ImageName.Length)).c_str());
         __ProcessInformationGet(creation.first,
          [&](
           const HANDLE& hProcess,
           const PROCESS_BASIC_INFORMATION& ProcessBasicInformation,
           const RTL_USER_PROCESS_PARAMETERS& RtlUserProcessParameters,
           const PEB& Peb) {
            __CommandLineGet(hProcess, RtlUserProcessParameters,
             [&](const std::string& res) {
              strcpy_s(route.CommandLine, sizeof(route.CommandLine), res.c_str());
             });
            __ImagePathNameGet(hProcess, RtlUserProcessParameters,
             [&](const std::string& res) {
              strcpy_s(route.ImagePathname, sizeof(route.ImagePathname), res.c_str());
             });
          });
         std::string sign, origin, product;
         Win::File::Attribute::GetFileObjSign(route.ImagePathname, sign);
         Win::File::Attribute::GetOriginalFilename(route.ImagePathname, origin);
         Win::File::Attribute::GetProductName(route.ImagePathname, product);
         if (!sign.empty())
          strcpy_s(route.Signature, sizeof(route.Signature), sign.c_str());
         if (!product.empty())
          strcpy_s(route.ProductName, sizeof(route.ProductName), product.c_str());
         if (!origin.empty())
          strcpy_s(route.OriginalFilename, sizeof(route.OriginalFilename), origin.c_str());
         routeCreations.emplace(std::make_pair(creation.first, route));
         ProcessSnapshootSimpleCache.push(creation.first, route);
        } while (0);
       }

       for (const auto& node : snapshoot) {
        ProcessSnapshootSimpleCache.search(node.first,
         [&](const PROCESSROUTEDATA& found) {
          m_ProcessSnapshootSimple.push(node.first, found);
         });
       }
       m_ProcessSnapshoot = snapshoot;
       std::unique_lock<std::mutex> lock{ *m_UniqueMutex };
       if (m_PrivateEnumCb && (!routeCreations.empty() || !routeDeletions.empty()))
        m_PrivateEnumCb(routeCreations, routeDeletions, *m_ProcessSnapshootSimple.src());
       lock.unlock();
       max_process_create_time_prev = max_process_create_time;
     }, 0);
   } while (0);
  }
  void NTObj::__EnumProcess() {
   std::atomic_int64_t max_process_create_time_prev = 0;
   do {
    __EnumProcess(
     max_process_create_time_prev,
     [&](
      const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>& snapshoot,
      const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>& creations,
      const time_t& max_process_create_time) {
       std::map<DWORD, SYSTEM_PROCESS_INFORMATION> deletions{};
       std::map<DWORD, PROCESSROUTEDATA> routeDeletions{}, routeCreations{};
       do {
        if (m_ProcessSnapshoot.empty()) {
         break;
        }
        deletions = *m_ProcessSnapshoot.src();
        for (auto it = deletions.begin(); it != deletions.end();) {
         auto found = snapshoot.find(it->first);
         if (found != snapshoot.end()) {
          it = deletions.erase(it);
          continue;
         }
         ++it;
        }
       } while (0);

       for (const auto& deletion : deletions) {
        m_ProcessSnapshootSimple.pop(deletion.first,
         [&](const auto& pid, auto& remove) {
          routeDeletions.emplace(std::make_pair(pid, remove));
         });
       }

       for (const auto& creation : creations) {
        do {
         PROCESSROUTEDATA route;
         route.UniqueProcessId = SK_PROCESSID(creation.second.UniqueProcessId);
         route.InheritedFromUniqueProcessId = SK_PROCESSID(creation.second.InheritedFromUniqueProcessId);
         m_ProcessSnapshootSimple.search(route.InheritedFromUniqueProcessId,
          [&](const PROCESSROUTEDATA& parent) {
           strcpy_s(route.ParentImageName, sizeof(route.ParentImageName), parent.ImageName);
           strcpy_s(route.ParentImagePathname, sizeof(route.ParentImagePathname), parent.ImagePathname);
          });
         route.CreateTime = creation.second.CreateTime.QuadPart;
         route.UniqueProcessKey = creation.second.UniqueProcessKey;
         route.SessionId = creation.second.SessionId;
         if (creation.second.ImageName.Length > 0)
          strcpy_s(route.ImageName, sizeof(route.ImageName),
           shared::IConv::WStringToMBytes(std::wstring(creation.second.ImageName.Buffer, creation.second.ImageName.Length)).c_str());
         ProcessInformationGet(creation.first,
          [&](
           const HANDLE& hProcess,
           const PROCESS_BASIC_INFORMATION& ProcessBasicInformation,
           const RTL_USER_PROCESS_PARAMETERS& RtlUserProcessParameters,
           const PEB& Peb) {
            __CommandLineGet(hProcess, RtlUserProcessParameters,
             [&](const std::string& res) {
              strcpy_s(route.CommandLine, sizeof(route.CommandLine), res.c_str());
             });
            __ImagePathNameGet(hProcess, RtlUserProcessParameters,
             [&](const std::string& res) {
              strcpy_s(route.ImagePathname, sizeof(route.ImagePathname), res.c_str());
             });
          });
         std::string sign, origin, product;
         Win::File::Attribute::GetFileObjSign(route.ImagePathname, sign);
         Win::File::Attribute::GetOriginalFilename(route.ImagePathname, origin);
         Win::File::Attribute::GetProductName(route.ImagePathname, product);
         if (!sign.empty())
          strcpy_s(route.Signature, sizeof(route.Signature), sign.c_str());
         if (!product.empty())
          strcpy_s(route.ProductName, sizeof(route.ProductName), product.c_str());
         if (!origin.empty())
          strcpy_s(route.OriginalFilename, sizeof(route.OriginalFilename), origin.c_str());
         routeCreations.emplace(std::make_pair(creation.first, route));
         m_ProcessSnapshootSimple.push(creation.first, route);
        } while (0);
       }

       m_ProcessSnapshoot = snapshoot;
       std::unique_lock<std::mutex> lock{ *m_UniqueMutex };
       if (m_PrivateEnumCb && (!routeCreations.empty() || !routeDeletions.empty()))
        m_PrivateEnumCb(routeCreations, routeDeletions, *m_ProcessSnapshootSimple.src());
       lock.unlock();
       max_process_create_time_prev.store(max_process_create_time);
     }, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(m_ProcessEnumIntervalMS.load()));
    if (!m_Ready.load())
     break;
   } while (1);
  }
  bool NTObj::__EnumProcess(std::map<DWORD, PROCESSROUTEDATA>& outProcesss) const {
   bool result = false;
   outProcesss.clear();
   do {
    if (!m_Ready.load())
     break;
    NTSTATUS status = STATUS_COMMON_ERROR;
    ULONG ulSuccessSize = 0xFFFF, ulRealSize{ 0 };
    void* SystemInfoProcessBuffer = nullptr;
    do {
     if (!SystemInfoProcessBuffer)
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
     status = NtQuerySystemInformation(
      SYSTEM_INFORMATION_CLASS::SystemProcessInformation,
      SystemInfoProcessBuffer,
      ulSuccessSize,
      &ulRealSize
     );
     if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_INFO_LENGTH_MISMATCH) {
      SK_DELETE_PTR_C(SystemInfoProcessBuffer);
      ulSuccessSize += ulRealSize;
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
      if (!SystemInfoProcessBuffer)
       break;
     }
     else
      break;
    } while (1);//16ms~32ms
    SYSTEM_PROCESS_INFORMATION* process = SK_FIRST_PROCESS(SystemInfoProcessBuffer);
    do {
     if (!process)
      break;
     do {
      if (process->UniqueProcessId <= HANDLE(4))
       break;
      PROCESSROUTEDATA route;
      route.UniqueProcessId = SK_PROCESSID(process->UniqueProcessId);
      route.InheritedFromUniqueProcessId = SK_PROCESSID(process->InheritedFromUniqueProcessId);
      route.CreateTime = process->CreateTime.QuadPart;
      route.UniqueProcessKey = process->UniqueProcessKey;
      route.SessionId = process->SessionId;
      if (process->ImageName.Length > 0)
       strcpy_s(route.ImageName, sizeof(route.ImageName),
        shared::IConv::WStringToMBytes(std::wstring(process->ImageName.Buffer, process->ImageName.Length)).c_str());
      outProcesss.emplace(std::make_pair(route.UniqueProcessId, route));
     } while (0);
     process = SK_NEXT_PROCESS(process);
    } while (1);
    SK_DELETE_PTR_C(SystemInfoProcessBuffer);
    result = true;
   } while (0);
   return result;
  }
  bool NTObj::__EnumProcess(const std::function<void(const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>&)>& rescb) const {
   bool result = false;
   std::map<DWORD, SYSTEM_PROCESS_INFORMATION> outProcesss;
   do {
    if (!m_Ready.load())
     break;
    NTSTATUS status = STATUS_COMMON_ERROR;
    ULONG ulSuccessSize = 0xFFFF, ulRealSize{ 0 };
    void* SystemInfoProcessBuffer = nullptr;
    do {
     if (!SystemInfoProcessBuffer)
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
     status = NtQuerySystemInformation(
      SYSTEM_INFORMATION_CLASS::SystemProcessInformation,
      SystemInfoProcessBuffer,
      ulSuccessSize,
      &ulRealSize
     );
     if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_INFO_LENGTH_MISMATCH) {
      SK_DELETE_PTR_C(SystemInfoProcessBuffer);
      ulSuccessSize += ulRealSize;
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
      if (!SystemInfoProcessBuffer)
       break;
     }
     else
      break;
    } while (1);//16ms~32ms
    SYSTEM_PROCESS_INFORMATION* process = SK_FIRST_PROCESS(SystemInfoProcessBuffer);
    do {
     if (!process)
      break;
     do {
      if (process->UniqueProcessId <= HANDLE(4))
       break;
      outProcesss.emplace(std::make_pair(SK_PROCESSID(process->UniqueProcessId), *process));
     } while (0);
     process = SK_NEXT_PROCESS(process);
    } while (1);
    rescb(outProcesss);
    SK_DELETE_PTR_C(SystemInfoProcessBuffer);
   } while (0);
   return !outProcesss.empty();
  }
  bool NTObj::__EnumProcess(std::map<DWORD, SYSTEM_PROCESS_INFORMATION>& outProcesss) const {
   bool result = false;
   outProcesss.clear();
   do {
    if (!m_Ready.load())
     break;
    NTSTATUS status = STATUS_COMMON_ERROR;
    ULONG ulSuccessSize = 0xFFFF, ulRealSize{ 0 };
    void* SystemInfoProcessBuffer = nullptr;
    do {
     if (!SystemInfoProcessBuffer)
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
     status = NtQuerySystemInformation(
      SYSTEM_INFORMATION_CLASS::SystemProcessInformation,
      SystemInfoProcessBuffer,
      ulSuccessSize,
      &ulRealSize
     );
     if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_INFO_LENGTH_MISMATCH) {
      SK_DELETE_PTR_C(SystemInfoProcessBuffer);
      ulSuccessSize += ulRealSize;
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
      if (!SystemInfoProcessBuffer)
       break;
     }
     else
      break;
    } while (1);//16ms~32ms
    SYSTEM_PROCESS_INFORMATION* process = SK_FIRST_PROCESS(SystemInfoProcessBuffer);
    do {
     if (!process)
      break;
     do {
      if (process->UniqueProcessId <= HANDLE(4))
       break;
      outProcesss.emplace(std::make_pair(SK_PROCESSID(process->UniqueProcessId), *process));
     } while (0);
     process = SK_NEXT_PROCESS(process);
    } while (1);
    SK_DELETE_PTR_C(SystemInfoProcessBuffer);
   } while (0);
   return !outProcesss.empty();
  }
  bool NTObj::__EnumProcess(//!@ The Private method
   const std::uint64_t& max_process_create_time_prev,
   const std::function<void(
    const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>&,
    const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>&, const std::uint64_t&)>& rescb,
   const std::uint64_t& pause_time_ms /*= 0*/) {
   bool result = false;
   std::map<DWORD, SYSTEM_PROCESS_INFORMATION> snapshoots{}, creations{};
   do {
    if (!m_Ready.load())
     break;

    NTSTATUS status = STATUS_COMMON_ERROR;
    ULONG ulSuccessSize = 0xFFFF, ulRealSize{ 0 };
    void* SystemInfoProcessBuffer = nullptr;
    do {
     if (!SystemInfoProcessBuffer)
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
     status = NtQuerySystemInformation(
      SYSTEM_INFORMATION_CLASS::SystemProcessInformation,
      SystemInfoProcessBuffer,
      ulSuccessSize,
      &ulRealSize
     );
     if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_INFO_LENGTH_MISMATCH) {
      SK_DELETE_PTR_C(SystemInfoProcessBuffer);
      ulSuccessSize += ulRealSize;
      SystemInfoProcessBuffer = ::malloc(ulSuccessSize);
      if (!SystemInfoProcessBuffer)
       break;
     }
     else
      break;
     std::this_thread::sleep_for(std::chrono::milliseconds(0));
    } while (1);//16ms~32ms

#ifdef _DEBUG
    std::uint64_t __begin = ::GetTickCount64();
#endif
    std::uint64_t max_process_create_time = 0;
    SYSTEM_PROCESS_INFORMATION* process = SK_FIRST_PROCESS(SystemInfoProcessBuffer);
    do {
     if (!process)
      break;
     do {
      if (process->UniqueProcessId <= HANDLE(4))
       break;
      snapshoots.emplace(std::make_pair(SK_PROCESSID(process->UniqueProcessId), *process));
      if (static_cast<decltype(max_process_create_time_prev)>(process->CreateTime.QuadPart) > max_process_create_time_prev)
       creations.emplace(std::make_pair(SK_PROCESSID(process->UniqueProcessId), *process));
     } while (0);
     max_process_create_time = __max(max_process_create_time, static_cast<decltype(max_process_create_time)>(process->CreateTime.QuadPart));
     process = SK_NEXT_PROCESS(process);
     std::this_thread::sleep_for(std::chrono::milliseconds(pause_time_ms));
    } while (1);
#ifdef _DEBUG
    std::uint64_t __total = ::GetTickCount64() - __begin;
#endif
    rescb(snapshoots, creations, max_process_create_time);
    SK_DELETE_PTR_C(SystemInfoProcessBuffer);
   } while (0);
   return result;
  }
  void NTObj::__InitializeWindowsVersion() {
   RTL_OSVERSIONINFOEXW versionInfo;
   ULONG majorVersion;
   ULONG minorVersion;
   ULONG buildVersion;
   ::memset(&versionInfo, 0, sizeof(RTL_OSVERSIONINFOEXW));
   versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
   if (!NT_SUCCESS(RtlGetVersion(&versionInfo))) {
    m_WindowsVersion.store(WINDOWS_NEW);
    m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_NEW;
    return;
   }
   ::memcpy(&m_OsVersion, &versionInfo, sizeof(RTL_OSVERSIONINFOEXW));
   majorVersion = versionInfo.dwMajorVersion;
   minorVersion = versionInfo.dwMinorVersion;
   buildVersion = versionInfo.dwBuildNumber;
   if (majorVersion == 6 && minorVersion < 1 || majorVersion < 6)
   {
    m_WindowsVersion.store(WINDOWS_ANCIENT);
    m_OsName = "WINDOWS ANCIENT";
    m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_ANCIENT;
   }
   // Windows 7, Windows Server 2008 R2
   else if (majorVersion == 6 && minorVersion == 1)
   {
    m_WindowsVersion.store(WINDOWS_7);
    m_OsName = "WINDOWS 7";
    m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_7;
   }
   // Windows 8, Windows Server 2012
   else if (majorVersion == 6 && minorVersion == 2)
   {
    m_WindowsVersion.store(WINDOWS_8);
    m_OsName = "WINDOWS 8";
    m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_8;
   }
   // Windows 8.1, Windows Server 2012 R2
   else if (majorVersion == 6 && minorVersion == 3)
   {
    m_WindowsVersion.store(WINDOWS_8_1);
    m_OsName = "WINDOWS 8.1";
    m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_8_1;
   }
   // Windows 10, Windows Server 2016
   else if (majorVersion == 10 && minorVersion == 0)
   {
    if (buildVersion >= 22000)
    {
     m_WindowsVersion.store(WINDOWS_11);
     m_OsName = "WINDOWS 11";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_11;
    }
    else if (buildVersion >= 19043)
    {
     m_WindowsVersion.store(WINDOWS_10_21H1);
     m_OsName = "WINDOWS 10 21H1";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_21H1;
    }
    else if (buildVersion >= 19042)
    {
     m_WindowsVersion.store(WINDOWS_10_20H2);
     m_OsName = "WINDOWS 10 20H2";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_20H2;
    }
    else if (buildVersion >= 19041)
    {
     m_WindowsVersion.store(WINDOWS_10_20H1);
     m_OsName = "WINDOWS 10 20H1";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_20H1;
    }
    else if (buildVersion >= 18363)
    {
     m_WindowsVersion.store(WINDOWS_10_19H2);
     m_OsName = "WINDOWS 10 19H2";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_19H2;
    }
    else if (buildVersion >= 18362)
    {
     m_WindowsVersion.store(WINDOWS_10_19H1);
     m_OsName = "WINDOWS 10 19H1";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_19H1;
    }
    else if (buildVersion >= 17763)
    {
     m_WindowsVersion.store(WINDOWS_10_RS5);
     m_OsName = "WINDOWS 10 RS5";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_RS5;
    }
    else if (buildVersion >= 17134)
    {
     m_WindowsVersion.store(WINDOWS_10_RS4);
     m_OsName = "WINDOWS 10 RS4";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_RS4;
    }
    else if (buildVersion >= 16299)
    {
     m_WindowsVersion.store(WINDOWS_10_RS3);
     m_OsName = "WINDOWS 10 RS3";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_RS3;
    }
    else if (buildVersion >= 15063)
    {
     m_WindowsVersion.store(WINDOWS_10_RS2);
     m_OsName = "WINDOWS 10 RS2";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_RS2;
    }
    else if (buildVersion >= 14393)
    {
     m_WindowsVersion.store(WINDOWS_10_RS1);
     m_OsName = "WINDOWS 10 RS1";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_RS1;
    }
    else if (buildVersion >= 10586)
    {
     m_WindowsVersion.store(WINDOWS_10_TH2);
     m_OsName = "WINDOWS 10 TH2";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10_TH2;
    }
    else if (buildVersion >= 10240)
    {
     m_WindowsVersion.store(WINDOWS_10);
     m_OsName = "WINDOWS 10";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10;
    }
    else
    {
     m_WindowsVersion.store(WINDOWS_10);
     m_OsName = "WINDOWS 10";
     m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_10;
    }
   }
   else
   {
    m_WindowsVersion.store(WINDOWS_NEW);
    m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_NEW;
    m_OsName = "WINDOWS NEW";
   }
  }
  bool NTObj::__ProcessInformationGet
  (
   _In_ const DWORD& inPid,
   _In_ const std::function<void(
    _Out_ const HANDLE&,
    _Out_ const PROCESS_BASIC_INFORMATION&,
    _Out_ const RTL_USER_PROCESS_PARAMETERS&,
    _Out_ const PEB&)>& rescb
  ) const {
   bool result = false;
   HANDLE HANDLE___ = nullptr;
   PROCESS_BASIC_INFORMATION PROCESS_BASIC_INFORMATION___;
   RTL_USER_PROCESS_PARAMETERS RTL_USER_PROCESS_PARAMETERS___;
   PEB PEB___;
   do {
    if (inPid <= 4)
     break;
    HANDLE___ = __OpenPorcess(inPid, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
    if (!HANDLE___)
     break;
    NTSTATUS status = NtQueryInformationProcess(HANDLE___,
     PROCESSINFOCLASS::ProcessBasicInformation,
     (void*)&PROCESS_BASIC_INFORMATION___, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
    if (STATUS_SUCCESS != status)
     break;
    SIZE_T dwDummy = 0;
    if (0 == ::ReadProcessMemory(HANDLE___, PROCESS_BASIC_INFORMATION___.PebBaseAddress, &PEB___, sizeof(PEB), &dwDummy))
     break;
    if (0 == ::ReadProcessMemory(HANDLE___, PEB___.ProcessParameters, &RTL_USER_PROCESS_PARAMETERS___, sizeof(RTL_USER_PROCESS_PARAMETERS), &dwDummy))
     break;
    rescb(HANDLE___, PROCESS_BASIC_INFORMATION___, RTL_USER_PROCESS_PARAMETERS___, PEB___);
    result = true;
   } while (0);
   SK_CLOSE_HANDLE(HANDLE___);
   return result;
  }

  bool NTObj::__ProcessInformationGet
  (
   _In_ const DWORD& inPid,
   _Out_ PROCESS_BASIC_INFORMATION& outProcessBasicInformation,
   _Out_ RTL_USER_PROCESS_PARAMETERS& outRtlUserProcessParameters,
   _Out_ PEB& outPeb
  ) const {
   bool result = false;
   HANDLE hProcess = nullptr;
   ::memset(&outPeb, 0x00, sizeof(outPeb));
   ::memset(&outProcessBasicInformation, 0x00, sizeof(outProcessBasicInformation));
   ::memset(&outRtlUserProcessParameters, 0x00, sizeof(outRtlUserProcessParameters));
   do {
    if (inPid <= 4)
     break;
    hProcess = __OpenPorcess(inPid);
    if (!hProcess)
     break;
    NTSTATUS status = NtQueryInformationProcess(hProcess,
     PROCESSINFOCLASS::ProcessBasicInformation,
     (void*)&outProcessBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
    if (STATUS_SUCCESS != status)
     break;
    SIZE_T dwDummy = 0;
    if (0 == ::ReadProcessMemory(hProcess, outProcessBasicInformation.PebBaseAddress, &outPeb, sizeof(PEB), &dwDummy))
     break;
    if (0 == ::ReadProcessMemory(hProcess, outPeb.ProcessParameters, &outRtlUserProcessParameters, sizeof(RTL_USER_PROCESS_PARAMETERS), &dwDummy))
     break;
    result = true;
   } while (0);
   SK_CLOSE_HANDLE(hProcess);
   return result;
  }
  bool NTObj::__ProcessGet(const DWORD& ProcessId, const std::function<void(const PROCESSROUTEDATA&)>& rescb) const {
   bool result = false;
   do {
    std::map<DWORD, PROCESSROUTEDATA> Snapshoots;
    if (m_ProcessSnapshootSimple.empty())
     __EnumProcess(Snapshoots);
    else
     Snapshoots = *m_ProcessSnapshootSimple.src();
    if (Snapshoots.empty())
     break;
    auto found = Snapshoots.find(ProcessId);
    if (found == Snapshoots.end())
     break;
    rescb(found->second);
    result = true;
   } while (0);
   return result;
  }
  bool NTObj::__ProcessGet(const std::string& found, const std::function<void(const std::vector<PROCESSROUTEDATA>&)>& rescb) const {
   bool result = false;
   do {
    std::map<DWORD, PROCESSROUTEDATA> Snapshoots;
    if (m_ProcessSnapshootSimple.empty())
     __EnumProcess(Snapshoots);
    else
     Snapshoots = *m_ProcessSnapshootSimple.src();
    if (Snapshoots.empty())
     break;
    std::vector<PROCESSROUTEDATA> res;
    for (const auto& node : Snapshoots) {
     if (0 == ::_stricmp(found.c_str(), node.second.ImageName))
      res.emplace_back(node.second);
    }
    result = !res.empty();
    if (result)
     rescb(res);
   } while (0);
   return result;
  }
  bool NTObj::__ElevatedPrivileges() const {
   bool result = false;
   std::set<xstring> successs, faileds;
   HANDLE hToken = nullptr;
   do {
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
     break;
    for (size_t i = 0; i < _countof(SePrivileges); ++i) {
     bool success = false;
     do {
      LUID luid = { 0 };
      if (!::LookupPrivilegeValue(nullptr, SePrivileges[i], &luid))
       break;
      TOKEN_PRIVILEGES tokenPriv;
      tokenPriv.PrivilegeCount = 1;
      tokenPriv.Privileges[0].Luid = luid;
      tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      if (0 == ::AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
       break;
      success = ::GetLastError() == ERROR_SUCCESS;
     } while (0);
     success ? successs.emplace(SePrivileges[i]) : faileds.emplace(SePrivileges[i]);
    }
    result = true;
   } while (0);
   SK_CLOSE_HANDLE(hToken);
   return result;
  }


 }///namespace nt

}///namespace shared