#include "stdafx.h"

namespace shared {
 namespace nt {
  NTObj::NTObj(const bool& EnableAutoEnumProcess /*= false*/,
   const std::int64_t& ProcessEnumIntervalMS /*= 500*/) :
   m_ProcessEnumIntervalMS(ProcessEnumIntervalMS),
   m_EnableAutoEnumProcess(EnableAutoEnumProcess) {
   Init();
  }
  NTObj::~NTObj() {
   UnInit();
  }
  bool NTObj::Ready() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_Ready.load();
  }
  std::vector<SYSTEM_PROCESS_INFORMATION> \
   NTObj::ProcessSnapshoot() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_ProcessSnapshoot.Vector();
  }
  container::map<DWORD, PROCESSROUTEDATA> \
   NTObj::ProcessSnapshootSimple() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_ProcessSnapshootSimple;
  }
  const std::string& NTObj::OsName() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_OsName;
  }
  std::uint64_t NTObj::WindowsVersion() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_WindowsVersion.load();
  }
  bool NTObj::PrivateEnumCb() const {
   std::unique_lock<std::mutex> lock{ *m_UniqueMutex };
   return m_PrivateEnumCb != nullptr;
  }
  void NTObj::PrivateEnumCb(const std::function<void(const std::map<DWORD, PROCESSROUTEDATA>&, \
   const std::map<DWORD, PROCESSROUTEDATA>&, const std::map<DWORD, PROCESSROUTEDATA>&)>& cb) {
   std::unique_lock<std::mutex> lock{ *m_UniqueMutex };
   m_PrivateEnumCb = cb;
   lock.unlock();
  }
  bool NTObj::ProcessInformationGet(const std::string& processImageName, \
   const std::function<void(const std::set<PROCESSROUTEDATA>&/*枚举到的进程,包含父进程信息(如果父进程存在)*/, const std::set<PROCESSROUTEDATA>&/**/)>& rescb) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   do {
    if (processImageName.empty())
     break;
    std::set<DWORD> ProcessIds;
    std::set<PROCESSROUTEDATA> hosts;
    std::set<PROCESSROUTEDATA> processes;
    PROCESSROUTEDATA* host = nullptr;
    std::map<DWORD, PROCESSROUTEDATA> snapshoot;
    if (m_ProcessSnapshootSimple.empty()) {
     if (!__EnumProcess(snapshoot))
      break;
    }
    else
     snapshoot = *m_ProcessSnapshootSimple.src();
    if (snapshoot.empty())
     break;
    std::for_each(snapshoot.begin(), snapshoot.end(),
     [&](const auto& node) {
      if (!::_stricmp(node.second.ImageName, processImageName.c_str())) {
       ProcessIds.emplace(node.first);
       processes.emplace(node.second);
      }
     });
    result = !processes.empty();
    if (result) {
     std::for_each(processes.begin(), processes.end(),
      [&](const auto& node) {
       if (ProcessIds.find(node.InheritedFromUniqueProcessId) == ProcessIds.end())
        hosts.emplace(node);
      });
#if 0
     //!@ 补充父进程信息
     std::for_each(processes.begin(), processes.end(),
      [&](auto& pro) {
       auto found = snapshoot.find(pro.InheritedFromUniqueProcessId);
       if (found != snapshoot.end()) {


        auto ssksk = 0;
       }

      });
#endif
     rescb(processes, hosts);
    }
   } while (0);
   return result;
  }
  bool NTObj::ProcessIDGet(const std::string& processName, const std::function<void(const std::set<DWORD>&)>& rescb) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   do {
    if (processName.empty())
     break;
    std::set<DWORD> ress;
    std::map<DWORD, PROCESSROUTEDATA> snapshoot;
    if (m_ProcessSnapshootSimple.empty()) {
     if (!__EnumProcess(snapshoot))
      break;
    }
    else
     snapshoot = *m_ProcessSnapshootSimple.src();
    if (snapshoot.empty())
     break;
    std::for_each(snapshoot.begin(), snapshoot.end(),
     [&](const auto& node) {
      if (!::_stricmp(node.second.ImageName, processName.c_str()))
       ress.emplace(node.first);
     });
    result = !ress.empty();
    if (result)
     rescb(ress);
   } while (0);
   return result;
  }
  bool NTObj::ProcessInformationSet(
   _In_ const DWORD& ProcessId,
   _In_ const std::function<void(const HANDLE&, const PROCESS_BASIC_INFORMATION&, const RTL_USER_PROCESS_PARAMETERS&, const PEB&)>& rescb
  ) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE hProcess = nullptr;
   PEB outPeb;
   PROCESS_BASIC_INFORMATION outProcessBasicInformation;
   RTL_USER_PROCESS_PARAMETERS outRtlUserProcessParameters;
   do {
    if (ProcessId <= 4)
     break;
    hProcess = __OpenPorcess(ProcessId);
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
    rescb(hProcess, outProcessBasicInformation, outRtlUserProcessParameters, outPeb);
    result = true;
   } while (0);
   SK_NTCLOSE(hProcess);
   return result;
  }
  bool NTObj::ProcessInformationGet(
   _In_ const DWORD& inPid,
   _In_ const std::function<void(const HANDLE&, const PROCESS_BASIC_INFORMATION&, const RTL_USER_PROCESS_PARAMETERS&, const PEB&)>& rescb
  ) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE hProcess = nullptr;
   PEB outPeb;
   PROCESS_BASIC_INFORMATION outProcessBasicInformation;
   RTL_USER_PROCESS_PARAMETERS outRtlUserProcessParameters;
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
    rescb(hProcess, outProcessBasicInformation, outRtlUserProcessParameters, outPeb);
    result = true;
   } while (0);
   SK_NTCLOSE(hProcess);
   return result;
  }
  bool NTObj::ProcessSuspend(const DWORD& ProcessId) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE processHandle = nullptr;
   do {
    processHandle = __OpenPorcess(ProcessId, PROCESS_SUSPEND_RESUME);
    if (!processHandle)
     break;
    if (STATUS_SUCCESS != NtSuspendProcess(processHandle))
     break;
    result = true;
   } while (0);
   SK_NTCLOSE(processHandle);
   return result;
  }
  bool NTObj::ProcessResume(const DWORD& ProcessId) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE processHandle = nullptr;
   do {
    processHandle = __OpenPorcess(ProcessId, PROCESS_SUSPEND_RESUME);
    if (!processHandle)
     break;
    if (STATUS_SUCCESS != NtResumeProcess(processHandle))
     break;
    result = true;
   } while (0);
   SK_NTCLOSE(processHandle);
   return result;
  }
  bool NTObj::TerminateProcess(const DWORD& ProcessId) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE processHandle = nullptr;
   do {
    processHandle = __OpenPorcess(ProcessId, PROCESS_TERMINATE);
    if (!processHandle)
     break;
    if (STATUS_SUCCESS != NtTerminateProcess(processHandle, STATUS_SUCCESS))
     break;
    result = true;
   } while (0);
   SK_NTCLOSE(processHandle);
   return result;
  }

  bool NTObj::MasqueradeProcess(
   const DWORD& ProcessId,
   const std::string& ImageName,
   const std::string& ImagePathname,
   const std::string& CommandLine) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE hProcess = nullptr;
   do {
    if (ImageName.empty() || ImagePathname.empty())
     break;
    std::string routeCommandLine{ CommandLine };
    if (routeCommandLine.empty())
     routeCommandLine.append(R"(")").append(ImagePathname).append(R"(")");
    hProcess = __OpenPorcess(ProcessId);
    if (!hProcess)
     break;
    PROCESS_BASIC_INFORMATION processBasicInfo;
    if (!__GetProcessBasicInformation(hProcess, processBasicInfo))
     break;
    if (!processBasicInfo.PebBaseAddress)
     break;
    auto route = std::make_tuple(processBasicInfo.PebBaseAddress, this, hProcess, ImageName, ImagePathname, routeCommandLine);
    result = LdrEnumerateLoadedModules(0,
     [](_In_ LDR_DATA_TABLE_ENTRY* DataTableEntry,
      _In_ PVOID Context,
      _Inout_ BOOLEAN* StopEnumeration) {
       auto pTieRoute = reinterpret_cast<std::tuple<shared::nt::PPEB, const NTObj*, HANDLE, std::string, std::string, std::string>*>(Context);
       *StopEnumeration = 0;
       do {
        if (!pTieRoute) {
         *StopEnumeration = 1;
         break;
        }
        shared::nt::PPEB peb = std::get<0>(*pTieRoute);
        const NTObj* __this = std::get<1>(*pTieRoute);
        HANDLE handle = std::get<2>(*pTieRoute);
        std::string imageName = std::get<3>(*pTieRoute);
        std::string imagePathname = std::get<4>(*pTieRoute);
        std::string CommandLine = std::get<5>(*pTieRoute);
        if (!peb || !__this || !handle) {
         *StopEnumeration = 1;
         break;
        }
        if (DataTableEntry->DllBase != peb->ImageBaseAddress)
         break;
        *StopEnumeration = 1;

        __this->RtlEnterCriticalSection(peb->FastPebLock);
        {//!@ CommandLine
         void* pBuffer = nullptr;
         SIZE_T nBuffer = 0x1000;
         if (STATUS_SUCCESS == \
          __this->NtAllocateVirtualMemory(handle, &pBuffer, 0, &nBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {
          auto uTarget = shared::IConv::MBytesToWString(CommandLine);
          ::memcpy(pBuffer, uTarget.c_str(), __min(nBuffer, uTarget.size() * 2));
          __this->RtlInitUnicodeString((UNICODE_STRING*)&peb->ProcessParameters->CommandLine,
           reinterpret_cast<PCWSTR>(pBuffer));
         }
        }
        {//!@ ImagePathname
         void* pBuffer = nullptr;
         SIZE_T nBuffer = 0x1000;
         if (STATUS_SUCCESS == \
          __this->NtAllocateVirtualMemory(handle, &pBuffer, 0, &nBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {
          auto uTarget = shared::IConv::MBytesToWString(imagePathname);
          ::memcpy(pBuffer, uTarget.c_str(), __min(nBuffer, uTarget.size() * 2));
          __this->RtlInitUnicodeString((UNICODE_STRING*)&peb->ProcessParameters->ImagePathName,
           reinterpret_cast<PCWSTR>(pBuffer));
         }
        }
        {//!@ CurrentDirectory
         void* pBuffer = nullptr;
         SIZE_T nBuffer = 0x1000;
         if (STATUS_SUCCESS == \
          __this->NtAllocateVirtualMemory(handle, &pBuffer, 0, &nBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {
          auto uTarget = shared::IConv::MBytesToWString(shared::Win::PathFixedA(shared::Win::GetPathByPathnameA(imagePathname)));
          ::memcpy(pBuffer, uTarget.c_str(), __min(nBuffer, uTarget.size() * 2));
          __this->RtlInitUnicodeString((UNICODE_STRING*)&peb->ProcessParameters->CurrentDirectory,
           reinterpret_cast<PCWSTR>(pBuffer));
         }
        }
        __this->RtlLeaveCriticalSection(peb->FastPebLock);


        {//!@ FullDllName
         void* pBuffer = nullptr;
         SIZE_T nBuffer = 0x1000;
         if (STATUS_SUCCESS == \
          __this->NtAllocateVirtualMemory(handle, &pBuffer, 0, &nBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {
          auto uTarget = shared::IConv::MBytesToWString(imagePathname);
          ::memcpy(pBuffer, uTarget.c_str(), __min(nBuffer, uTarget.size() * 2));
          __this->RtlInitUnicodeString(&DataTableEntry->FullDllName,
           reinterpret_cast<PCWSTR>(pBuffer));
         }
        }

        {//!@ BaseDllName
         void* pBuffer = nullptr;
         SIZE_T nBuffer = 0x1000;
         if (STATUS_SUCCESS == \
          __this->NtAllocateVirtualMemory(handle, &pBuffer, 0, &nBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {
          auto uTarget = shared::IConv::MBytesToWString(imageName);
          ::memcpy(pBuffer, uTarget.c_str(), __min(nBuffer, uTarget.size() * 2));
          __this->RtlInitUnicodeString(&DataTableEntry->BaseDllName,
           reinterpret_cast<PCWSTR>(pBuffer));
         }
        }
       } while (0);
     }, &route) == STATUS_SUCCESS;
   } while (0);
   SK_NTCLOSE(hProcess);
   return result;
  }
  bool NTObj::NtCreateProcess(HANDLE& outHanle, DWORD& ouPid, const std::wstring& ImagePathname, const std::wstring& commandline, const std::string& shellcode) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   shared::Win::EnableDebugPrivilege();
   HANDLE hDelFile = NULL;
   HANDLE hSection = nullptr;
   do {
    if (!NtCreateProcessEx || !NtCreateThreadEx || !RtlCreateProcessParametersEx)
     break;
    if (!shared::Win::AccessW(ImagePathname))
     break;

    wchar_t dummy_name[MAX_PATH] = { 0 };
    wchar_t temp_path[MAX_PATH] = { 0 };
    DWORD size = GetTempPathW(MAX_PATH, temp_path);
    GetTempFileNameW(temp_path, L"SK", 0, dummy_name);


    // convert to NT path
				std::wstring nt_path = L"\\??\\" + std::wstring(dummy_name);
				UNICODE_STRING file_name;
				RtlInitUnicodeString(&file_name, nt_path.c_str());
				OBJECT_ATTRIBUTES attr;
				InitializeObjectAttributes(&attr, &file_name, OBJ_CASE_INSENSITIVE, NULL, NULL);
				IO_STATUS_BLOCK status_block;
				hDelFile = INVALID_HANDLE_VALUE;
				NTSTATUS status = NtOpenFile(&hDelFile,
					DELETE | SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
					(::OBJECT_ATTRIBUTES*)&attr,
					(::IO_STATUS_BLOCK*)&status_block,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					FILE_SUPERSEDE | FILE_SYNCHRONOUS_IO_NONALERT
				);
				if (!NT_SUCCESS(status))
					break;

    /* Set disposition flag */
    FILE_DISPOSITION_INFORMATION info;
    info.DeleteFile = TRUE;
    status = NtSetInformationFile(hDelFile, (::IO_STATUS_BLOCK*) & status_block, &info, sizeof(info), FileDispositionInformation);
    if (!NT_SUCCESS(status))
     break;
    LARGE_INTEGER ByteOffset = { 0 };
    status = NtWriteFile(
     hDelFile,
     NULL,
     NULL,
     NULL,
     (::IO_STATUS_BLOCK*)&status_block,
     (PVOID)shellcode.data(),
     static_cast<ULONG>(shellcode.size()),
     &ByteOffset,
     NULL
    );
    if (NULL == hDelFile || hDelFile == INVALID_HANDLE_VALUE)
     break;

    status = NtCreateSection(&hSection,
     SECTION_ALL_ACCESS,
     NULL,
     0,
     PAGE_READONLY,
     SEC_IMAGE,
     hDelFile
    );
    if (!NT_SUCCESS(status)) {
     auto error = ::GetLastError();
     break;
    }


    status = NtCreateProcessEx(
     &outHanle, //ProcessHandle
     PROCESS_ALL_ACCESS, //DesiredAccess
     NULL, //ObjectAttributes
     NtCurrentProcess(), //ParentProcess
     4/*PS_INHERIT_HANDLES*/, //Flags
     hSection, //sectionHandle
     NULL, //DebugPort
     NULL, //ExceptionPort
     FALSE //InJob
    );
    if (status != STATUS_SUCCESS)
     break;

    result = true;
   } while (0);
   if (hDelFile) {
    NtClose(hDelFile);
    hDelFile = NULL;
   }
   return result;
  }
  EnWindowsVersion NTObj::WindowsVersionTypeGet() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_WindowsVersionType;
  }
  std::string NTObj::ProcessImageNameSnapshot() const {
   std::string result;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   do {
    if (m_ProcessSnapshootSimple.empty())
     break;
    m_ProcessSnapshootSimple.iterate_const(
     [&](const PROCESSROUTEDATA& process, bool&) {
      if (result.find(std::string(process.ImageName) + ",") == std::string::npos)
       result.append(process.ImageName).append(",");
     });
    if (!result.empty())
     result.pop_back();
    if (!result.empty())
     result = shared::IConv::ToLowerA(result);
   } while (0);
   return result;
  }
  void NTObj::UpdateProcessCache() {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   do {
    if (m_EnableAutoEnumProcess.load())
     break;
    decltype(m_ProcessSnapshootSimple) ProcessSnapshootSimpleCache;
    __EnumProcess([&](const auto& snapshoot) {
     m_ProcessSnapshoot = snapshoot;
     m_ProcessSnapshoot.iterate_const(
      [&](const SYSTEM_PROCESS_INFORMATION& system_process_info, bool& itbreak) {
       PROCESSROUTEDATA route;
       route.UniqueProcessId = SK_PROCESSID(system_process_info.UniqueProcessId);
       route.InheritedFromUniqueProcessId = SK_PROCESSID(system_process_info.InheritedFromUniqueProcessId);
       m_ProcessSnapshootSimple.search(route.InheritedFromUniqueProcessId,
        [&](const PROCESSROUTEDATA& parent) {
         strcpy_s(route.ParentImageName, sizeof(route.ParentImageName), parent.ImageName);
         strcpy_s(route.ParentImagePathname, sizeof(route.ParentImagePathname), parent.ImagePathname);
        });
       route.CreateTime = system_process_info.CreateTime.QuadPart;
       route.UniqueProcessKey = system_process_info.UniqueProcessKey;
       route.SessionId = system_process_info.SessionId;
       if (system_process_info.ImageName.Length > 0)
        strcpy_s(route.ImageName, sizeof(route.ImageName),
         shared::IConv::WStringToMBytes(std::wstring(system_process_info.ImageName.Buffer, system_process_info.ImageName.Length)).c_str());
       __ProcessInformationGet(route.UniqueProcessId,
        [&](const auto& hProcess, const auto& systemBasicInfo, const auto& userParameters, const auto& Peb) {
         __CommandLineGet(hProcess, userParameters,
          [&](const std::string& res) {
           strcpy_s(route.CommandLine, sizeof(route.CommandLine), res.c_str());
          });
         __ImagePathNameGet(hProcess, userParameters,
          [&](const std::string& res) {
           strcpy_s(route.ImagePathname, sizeof(route.ImagePathname), res.c_str());
          });
        });
#if 0
       std::string sign, origin;
       Windows::GetFileObjSign(route.ImagePathname, sign);
       file::Attribute::GetOriginalFilename(route.ImagePathname, origin);
       if (!sign.empty())
        shared::Windows::SafeCharArraySet(route.Signature, sizeof(route.Signature), sign.c_str());
       if (!origin.empty())
        shared::Windows::SafeCharArraySet(route.OriginalFilename, sizeof(route.OriginalFilename), origin.c_str());
#endif
       ProcessSnapshootSimpleCache.push(route.UniqueProcessId, route);
      });
     });
    m_ProcessSnapshootSimple = ProcessSnapshootSimpleCache;
   } while (0);
  }
  HANDLE NTObj::GetCurrentHandle() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_CurrentProcessHandle.load();
  }
  DWORD NTObj::GetCurrentProcessId() const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   return m_CurrentProcessId.load();
  }
  bool NTObj::GetCurrentPeb(const std::function<void(shared::nt::PPEB)>&) {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   do {
    if (!RtlGetCurrentPeb)
     break;
    shared::nt::PPEB ppeb = RtlGetCurrentPeb();
    result = ppeb != nullptr;
   } while (0);
   return result;
  }
  bool NTObj::EnumProcessModuleInformation(
   const DWORD& ProcessId,
   const std::function<void(const HANDLE& ProcessHandle, PLDR_DATA_TABLE_ENTRY Entry, PVOID AddressOfEntry, PPEB, bool& itbreak)>& Callback
  ) const {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   bool result = __EnumProcessModules(ProcessId, Callback);
   return result;
  }
  void NTObj::EnumProcessOnce(
   const std::function<void(
    const std::map<DWORD, ProcessInfo>& creations, \
    const std::map<DWORD, ProcessInfo>& deletions, \
    const std::map<DWORD, ProcessInfo>& snapshoot)>& enum_cb,
   const bool& bPatchProcessInfo,
   const bool& bPatchVersionInfo,
   const bool& bPatchParentProcessInfo,
   const bool& bPatchAccountSid,
   const std::uint64_t& pause_time_ms) {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   bool result = false;
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
    std::map<DWORD, ProcessInfo> resultCreations{}, resultDeletions{ m_EnumProcessOncePrivateSnapshootBak };
    SYSTEM_PROCESS_INFORMATION* process = SK_FIRST_PROCESS(SystemInfoProcessBuffer);
    do {
     if (!process)
      break;
     do {
      if (process->UniqueProcessId <= HANDLE(4))
       break;

      //!@ 进行时进程
      auto found_process = resultDeletions.find(SK_PROCESSID(process->UniqueProcessId));
      if (found_process != resultDeletions.end()) {
       if (found_process->second.CreateTime == process->CreateTime.QuadPart) {
        resultDeletions.erase(found_process);
        break;
       }
      }

      ProcessInfo Info;
      Info.UniqueProcessId = SK_PROCESSID(process->UniqueProcessId);
      Info.InheritedFromUniqueProcessId = SK_PROCESSID(process->InheritedFromUniqueProcessId);
      Info.CreateTime = process->CreateTime.QuadPart;
      if (process->ImageName.Length > 0)
       Info.ImageName = shared::IConv::WStringToMBytes(std::wstring(process->ImageName.Buffer, process->ImageName.Length));
      if (bPatchProcessInfo) {
       //!@ 补充进程信息
       HANDLE hProcess = nullptr;
       HANDLE hProcessToken = nullptr;
       do {
        hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Info.UniqueProcessId);
        if (!hProcess)
         hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Info.UniqueProcessId);
        if (!hProcess)
         hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, Info.UniqueProcessId);
        if (!hProcess)
         break;

        //!@ Token Sud -> Account and Domain
        char* pTokenUserBuffer = nullptr;
        do{
         if (!bPatchAccountSid)
          break;
         if (!::OpenProcessToken(hProcess, TOKEN_QUERY, &hProcessToken))
          break;
         DWORD returnLen = 0;
         ::GetTokenInformation(hProcessToken, TOKEN_INFORMATION_CLASS::TokenUser, nullptr, 0, &returnLen);
         if (returnLen <= 0)
          break;
         DWORD dwInBufferSize = returnLen;
         pTokenUserBuffer = new char[dwInBufferSize];
         ::memset(pTokenUserBuffer, 0x00, dwInBufferSize);
         if (!::GetTokenInformation(hProcessToken, TOKEN_INFORMATION_CLASS::TokenUser, pTokenUserBuffer, dwInBufferSize, &returnLen))
          break;
         PTOKEN_USER pTokenUser = reinterpret_cast<PTOKEN_USER>(pTokenUserBuffer);
         SID_NAME_USE sid_name_user;
         char AccountName[_MAX_PATH] = { 0 };
         DWORD nAccountName = _MAX_PATH;
         char DomainName[_MAX_PATH] = { 0 };
         DWORD nDomainName = _MAX_PATH;
         if (!::LookupAccountSidA(NULL, pTokenUser->User.Sid, AccountName, &nAccountName, DomainName, &nDomainName, &sid_name_user))
          break;
         Info.Account = std::string(AccountName, nAccountName);
         Info.Domain = std::string(DomainName, nDomainName);
        } while (0);
        SK_DELETE_PTR_BUFFER(pTokenUserBuffer);

        PROCESS_BASIC_INFORMATION PROCESS_BASIC_INFORMATION___;
        RTL_USER_PROCESS_PARAMETERS RTL_USER_PROCESS_PARAMETERS___;
        PEB PEB___;
        if (STATUS_SUCCESS != NtQueryInformationProcess(hProcess,
         PROCESSINFOCLASS::ProcessBasicInformation,
         (void*)&PROCESS_BASIC_INFORMATION___, sizeof(PROCESS_BASIC_INFORMATION), nullptr))
         break;
        SIZE_T dwDummy = 0;
        if (FALSE == ::ReadProcessMemory(hProcess, PROCESS_BASIC_INFORMATION___.PebBaseAddress, &PEB___, sizeof(PEB), &dwDummy))
         break;
        if (FALSE == ::ReadProcessMemory(hProcess, PEB___.ProcessParameters, &RTL_USER_PROCESS_PARAMETERS___, sizeof(RTL_USER_PROCESS_PARAMETERS), &dwDummy))
         break;
        if (RTL_USER_PROCESS_PARAMETERS___.CommandLine.Length > 0) {
         wchar_t* pBuffer = new wchar_t[RTL_USER_PROCESS_PARAMETERS___.CommandLine.Length * sizeof(wchar_t)];
         ::memset(pBuffer, 0x00, RTL_USER_PROCESS_PARAMETERS___.CommandLine.Length * sizeof(wchar_t));
         if (TRUE == ::ReadProcessMemory(hProcess, RTL_USER_PROCESS_PARAMETERS___.CommandLine.Buffer, pBuffer, RTL_USER_PROCESS_PARAMETERS___.CommandLine.Length, &dwDummy))
          Info.CommandLine = shared::IConv::WStringToMBytes(std::wstring(pBuffer, dwDummy));
         SK_DELETE_PTR_BUFFER(pBuffer);
        }
        if (RTL_USER_PROCESS_PARAMETERS___.ImagePathName.Length > 0) {
         wchar_t* pBuffer = new wchar_t[RTL_USER_PROCESS_PARAMETERS___.ImagePathName.Length * sizeof(wchar_t)];
         ::memset(pBuffer, 0x00, RTL_USER_PROCESS_PARAMETERS___.ImagePathName.Length * sizeof(wchar_t));
         if (TRUE == ::ReadProcessMemory(hProcess, RTL_USER_PROCESS_PARAMETERS___.ImagePathName.Buffer, pBuffer, RTL_USER_PROCESS_PARAMETERS___.ImagePathName.Length, &dwDummy)) {
          std::wstring wstrImagePathname(pBuffer, dwDummy);
          Info.ImagePathname = shared::IConv::WStringToMBytes(wstrImagePathname);
          if (bPatchVersionInfo) {
           shared::Win::File::Attribute::GetFileObjSign(Info.ImagePathname, Info.Signature);
           shared::Win::File::Attribute::GetVersionInfoA(Info.ImagePathname, Info.FileVersionInfo);
          }
         }
         SK_DELETE_PTR_BUFFER(pBuffer);
        }
       } while (0);
       SK_CLOSE_HANDLE(hProcessToken);
       SK_CLOSE_HANDLE(hProcess);
      }

      //!@ 备份最后创建进程的时候 用来计算新创建的进程
      if (static_cast<std::uint64_t>(process->CreateTime.QuadPart) > m_EnumProcessOnceLastCreateTimeBak.load()) {
       resultCreations.emplace(Info.UniqueProcessId, Info);

       auto found = m_EnumProcessOncePrivateSnapshootBak.find(Info.UniqueProcessId);
       if (found != m_EnumProcessOncePrivateSnapshootBak.end())
        m_EnumProcessOncePrivateSnapshootBak.erase(found);
       m_EnumProcessOncePrivateSnapshootBak.emplace(Info.UniqueProcessId, Info);

       auto foundDeletion = resultDeletions.find(Info.UniqueProcessId);
       if (foundDeletion != resultDeletions.end())
        resultDeletions.erase(foundDeletion);

       m_EnumProcessOnceLastCreateTimeBak.store(static_cast<std::uint64_t>(process->CreateTime.QuadPart));
      }

     } while (0);

     process = SK_NEXT_PROCESS(process);
     std::this_thread::sleep_for(std::chrono::milliseconds(pause_time_ms));
    } while (1);
    //!@ 补全父进程信息
    if (bPatchParentProcessInfo) {
     auto temp = m_EnumProcessOncePrivateSnapshootBak;
     for (auto& snap : m_EnumProcessOncePrivateSnapshootBak) {
      auto found = temp.find(snap.second.InheritedFromUniqueProcessId);
      if (found != temp.end()) {
       snap.second.ParentImageName = found->second.ImageName;
       snap.second.ParentImagePathname = found->second.ImagePathname;
      }
     }
    }

    for (const auto& del : resultDeletions) {
     auto found = m_EnumProcessOncePrivateSnapshootBak.find(del.first);
     if (found != m_EnumProcessOncePrivateSnapshootBak.end())
      m_EnumProcessOncePrivateSnapshootBak.erase(found);
    }
    if (enum_cb)
     enum_cb(resultCreations, resultDeletions, m_EnumProcessOncePrivateSnapshootBak);
    SK_DELETE_PTR_C(SystemInfoProcessBuffer);
    result = true;
   } while (0);
  }
  void NTObj::EnumProcessOnce(_Inout_ time_t& remember_time) {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   __EnumProcessOnce(remember_time);
  }
  void NTObj::PopRuntimelogs(const std::function<void(const std::vector<std::tuple<time_t, std::string>>&)>& rescb) {
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   rescb(m_RuntimelogQ.pops());
  }

  bool NTObj::OpenProcessWithToken(const std::string& tokenHost, \
   const std::string& openProcessPathname, \
   const std::string& starupCommandLine, \
   const DWORD& dwCreateFlag /*= CREATE_NEW_CONSOLE*/,
   const time_t& wait /*= INFINITE*/, \
   const bool& show /*= true*/) const {
   bool result = false;
   std::lock_guard<std::mutex> lock{ *m_SharedMutex };
   HANDLE hTokenTarget = nullptr;
   HANDLE hToken = nullptr;
   HANDLE hTokenNew = nullptr;
   do {
    if (!shared::Win::AccessA(openProcessPathname))
     break;
    if (tokenHost.empty())
     break;
    __ProcessGet(tokenHost,
     [&](const auto& found) {
      for (const PROCESSROUTEDATA& node : found) {
       hTokenTarget = __OpenPorcess(node.UniqueProcessId);
       if (hTokenTarget)
        break;
      }
     });
    if (!hTokenTarget)
     break;
    if (STATUS_SUCCESS != NtOpenProcessToken(hTokenTarget, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken))
     break;
    if (STATUS_SUCCESS != NtDuplicateToken(hToken,
     MAXIMUM_ALLOWED,
     nullptr,
     SECURITY_IMPERSONATION_LEVEL::SecurityImpersonation,
     TOKEN_TYPE::TokenPrimary,
     &hTokenNew))
     break;
    std::wstring exePathname{ shared::IConv::MBytesToWString(openProcessPathname) };
    std::wstring startParamenters{ shared::IConv::MBytesToWString(starupCommandLine) };
    STARTUPINFO startup_info = { 0 };
    PROCESS_INFORMATION process_info = { 0 };
    if (!::CreateProcessWithTokenW(hTokenNew, 0,
     exePathname.empty() ? NULL : exePathname.c_str(),   // No module name (use command line)
     startParamenters.empty() ? NULL : const_cast<wchar_t*>((L"/c " + startParamenters).c_str()),        // Command line
     CREATE_NEW_CONSOLE,
     NULL,
     NULL,
     &startup_info,
     &process_info))
     break;
    SK_CLOSE_HANDLE(process_info.hThread);
    SK_CLOSE_HANDLE(process_info.hProcess);
    result = true;
   } while (0);
   SK_NTCLOSE(hToken);
   SK_NTCLOSE(hTokenNew);
   SK_NTCLOSE(hTokenTarget);
   return result;
  }

 }///namespace nt
}///namespace shared