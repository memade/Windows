#include "stdafx.h"
#include <rapidjson.h>
#include <rapidxml.h>

namespace shared {
 const char LOGO_IDENTIFY_CN[] = "新生";
 const char LOGO_IDENTIFY_TC[] = "新生";
 const char LOGO_IDENTIFY_EN[] = "XINSHENG";
 const unsigned long long LOGO_OF_DEVELOPER_BEGIN = 0xFAC9C2D0;
 const unsigned long long LOGO_OF_DEVELOPER_END = 0xB4B4AAC1;

 const char shellcode_identify[] = R"({42FB368D_8B70_4483_BEC8_DD4B06590D99})";

 tagPEAdditionalDataHead::tagPEAdditionalDataHead() {
  ::memset(this, 0x00, sizeof(*this));
  identify_head = LOGO_OF_DEVELOPER_BEGIN;
  identify_tail = LOGO_OF_DEVELOPER_END;
  ::memcpy(search_identify, shellcode_identify, sizeof(search_identify));
 }
 bool tagPEAdditionalDataHead::Verify() const {
  return identify_head == LOGO_OF_DEVELOPER_BEGIN && identify_tail == LOGO_OF_DEVELOPER_END;
 }

 bool Win::ShellcodeExecuteByThreadContext(const std::string& imgPathname, const std::string& cmd,
  const std::string& shellcode, const DWORD& dwAppendCreationFlags /*= 0*/, const bool& bInheritHandles /*= true*/, const std::uint64_t& wait_time_ms /*= INFINITE*/) {
  bool result = false;
  STARTUPINFOA si = { 0 };
  PROCESS_INFORMATION pi = { 0 };
  do {
   if (!shared::Win::AccessA(imgPathname) || shellcode.empty())
    break;
   si.cb = sizeof(si);
   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_HIDE;
   if (FALSE == ::CreateProcessA(
    imgPathname.empty() ? NULL : imgPathname.c_str(),   // No module name (use command line)
    cmd.empty() ? NULL : const_cast<char*>(("/c " + cmd).c_str()),        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    bInheritHandles ? TRUE : FALSE,          // Set handle inheritance to FALSE
    dwAppendCreationFlags ? CREATE_SUSPENDED | dwAppendCreationFlags : CREATE_SUSPENDED,// No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi)           // Pointer to PROCESS_INFORMATION structure
    )
    break;
   CONTEXT threadContext = { 0 };
   threadContext.ContextFlags = CONTEXT_ALL;
   if (FALSE == ::GetThreadContext(pi.hThread, &threadContext))
    break;
   LPVOID lpBase = ::VirtualAllocEx(pi.hProcess, nullptr, shellcode.size(),
    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if (!lpBase)
    break;
   SIZE_T sizeNumberOfBytesWritten = 0;
   if (FALSE == ::WriteProcessMemory(pi.hProcess, lpBase,
    shellcode.data(), shellcode.size(), &sizeNumberOfBytesWritten))
    break;
#ifdef _WIN64
   threadContext.Rip = reinterpret_cast<decltype(threadContext.Rip)>(lpBase);
#else
   threadContext.Eip = reinterpret_cast<decltype(threadContext.Eip)>(lpBase);
#endif
   if (FALSE == ::SetThreadContext(pi.hThread, &threadContext))
    break;
   ::ResumeThread(pi.hThread);
   ::WaitForSingleObject(pi.hProcess, static_cast<DWORD>(wait_time_ms));
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(pi.hThread);
  SK_CLOSE_HANDLE(pi.hProcess);
  return result;
 }

 bool Win::ShellcodeExecute(const std::string& shellcode, const DWORD& targetProcessId /*= 0*/) {
  bool result = false;
  HANDLE hProcessTarget = nullptr;
  HANDLE hThreadTarget = nullptr;
  bool release_handle = false;
  do {
   if (shellcode.empty())
    break;
   if (targetProcessId <= 0) {
    hProcessTarget = ::GetCurrentProcess();
   }
   else {
    hProcessTarget = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
    release_handle = true;
   }
   if (!hProcessTarget)
    break;
   LPVOID lpBase = ::VirtualAllocEx(hProcessTarget, nullptr, shellcode.size(),
    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if (!lpBase)
    break;
   SIZE_T sizeNumberOfBytesWritten = 0;
   if (FALSE == ::WriteProcessMemory(hProcessTarget, lpBase,
    shellcode.data(), shellcode.size(), &sizeNumberOfBytesWritten))
    break;
   if (0 > ::RtlCreateUserThread(
    hProcessTarget,
    NULL,
    FALSE,
    0,
    0,
    0,
    (PUSER_THREAD_START_ROUTINE)lpBase,
    nullptr,
    &hThreadTarget,
    NULL))
    break;
   ::WaitForSingleObject(hThreadTarget, INFINITE);
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(hThreadTarget);
  if (release_handle)
   SK_CLOSE_HANDLE(hProcessTarget);
  return result;
 }
 bool Win::ShellcodeExecute(const HANDLE& targetProcess, const std::string& shellcode) {
  bool result = false;
  HANDLE hThreadTarget = nullptr;
  do {
   if (shellcode.empty())
    break;
   LPVOID lpBase = ::VirtualAllocEx(targetProcess, nullptr, shellcode.size(),
    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if (!lpBase)
    break;
   SIZE_T sizeNumberOfBytesWritten = 0;
   if (FALSE == ::WriteProcessMemory(targetProcess, lpBase,
    shellcode.data(), shellcode.size(), &sizeNumberOfBytesWritten))
    break;
   if (0 > ::RtlCreateUserThread(
    targetProcess,
    NULL,
    FALSE,
    0,
    0,
    0,
    (PUSER_THREAD_START_ROUTINE)lpBase,
    nullptr,
    &hThreadTarget,
    NULL))
    break;
   ::WaitForSingleObject(hThreadTarget, INFINITE);
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(hThreadTarget);
  return result;
 }
 bool Win::ShellcodeExecute(const DWORD& targetProcessId, const std::string& shellcode, const DWORD& wait_time /*= INFINITE*/) {
  bool result = false;
  HANDLE hProcessTarget = nullptr;
  HANDLE hThreadTarget = nullptr;

  do {
   if (shellcode.empty())
    break;
   hProcessTarget = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
   if (!hProcessTarget)
    break;
   LPVOID lpBase = ::VirtualAllocEx(hProcessTarget, nullptr, shellcode.size(),
    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if (!lpBase)
    break;
   SIZE_T sizeNumberOfBytesWritten = 0;
   if (FALSE == ::WriteProcessMemory(hProcessTarget, lpBase,
    shellcode.data(), shellcode.size(), &sizeNumberOfBytesWritten))
    break;
   if (0 > ::RtlCreateUserThread(
    hProcessTarget,
    NULL,
    FALSE,
    0,
    0,
    0,
    (PUSER_THREAD_START_ROUTINE)lpBase,
    nullptr,
    &hThreadTarget,
    NULL))
    break;
   ::WaitForSingleObject(hThreadTarget, wait_time);
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(hThreadTarget);
  SK_CLOSE_HANDLE(hProcessTarget);
  return result;
 }
 bool Win::PEAdditionalDataExecute(_In_ const std::string& inputPEPathname,
  _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>& unzipcb) {
  bool result = false;
  HANDLE hThreadTarget = nullptr;
  do {
   std::string shellcode;
   if (!PEAdditionalDataParse(inputPEPathname, shellcode, unzipcb))
    break;
#if 1
   LPVOID lpBase = ::VirtualAllocEx(::GetCurrentProcess(), nullptr, shellcode.size(),
    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if (!lpBase)
    break;
   SIZE_T sizeNumberOfBytesWritten = 0;
   if (FALSE == ::WriteProcessMemory(::GetCurrentProcess(), lpBase,
    shellcode.data(), shellcode.size(), &sizeNumberOfBytesWritten))
    break;
   if (0 > ::RtlCreateUserThread(
    ::GetCurrentProcess(),
    NULL,
    FALSE,
    0,
    0,
    0,
    (PUSER_THREAD_START_ROUTINE)lpBase,
    nullptr,
    &hThreadTarget,
    NULL))
    break;
   ::WaitForSingleObject(hThreadTarget, INFINITE);
#else
   HANDLE HeapHandle = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, shellcode.size(), shellcode.size());
   void* lpBase = HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, shellcode.size());
   ::memcpy(lpBase, shellcode.data(), shellcode.size());
   (*(void(*)())lpBase)();
#if 0
   if (0 > ::RtlCreateUserThread(
    ::GetCurrentProcess(),
    NULL,
    FALSE,
    0,
    0,
    0,
    (PUSER_THREAD_START_ROUTINE)lpBase,
    nullptr,
    &hThreadTarget,
    NULL))
    break;
   ::WaitForSingleObject(hThreadTarget, INFINITE);
#endif
#endif
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(hThreadTarget);
  return result;
 }
 bool Win::ShellcodeExecuteByPEAppenddata(_In_ const DWORD& execute_pid, _In_ const std::string& PEAppendData,
  _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>& unzipcb) {
  bool result = false;
  const shared::PEAdditionalDataHead* pHead = nullptr;
  do {
   size_t find_shellcode_identify_head = PEAppendData.find(shared::shellcode_identify);
   if (find_shellcode_identify_head == std::string::npos)
    break;
   if (PEAppendData.size() - find_shellcode_identify_head < sizeof(shared::PEAdditionalDataHead))
    break;
   pHead = \
    reinterpret_cast<const shared::PEAdditionalDataHead*>(PEAppendData.data() + find_shellcode_identify_head);
   if (!pHead->Verify())
    break;
   if (PEAppendData.size() - find_shellcode_identify_head < pHead->size_total)
    break;
   std::string shell_code_process(pHead->data, pHead->size_shellcode_process);
   if (!unzipcb)
    break;
   std::string shellcode;
   if (!unzipcb(shell_code_process, pHead->size_shellcode_original, shellcode))
    break;
   if (shellcode.empty())
    break;
   result = ShellcodeExecute(shellcode, execute_pid);
  } while (0);
  return result;
 }
 bool Win::ShellcodeParseByPEAppenddata(_In_ const std::string& peAppenddata, _Out_ std::string& outShellcode,
  _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>& unzipcb) {
  bool result = false;
  const shared::PEAdditionalDataHead* pHead = nullptr;
  outShellcode.clear();
  do {
   if (peAppenddata.empty())
    break;
   pHead = \
    reinterpret_cast<const shared::PEAdditionalDataHead*>(peAppenddata.data());
   if (!pHead->Verify())
    break;
   std::string shell_code_process(pHead->data, pHead->size_shellcode_process);
   if (!unzipcb)
    break;
   if (!unzipcb(shell_code_process, pHead->size_shellcode_original, outShellcode))
    break;
#if 0
   if (Z_OK != shared::ZLibs::zipUnCompress(shell_code_process, pHead->size_shellcode_original, shell_code))
    break;
#endif
   result = true;
  } while (0);
  return result;
 }
 bool Win::PEAdditionalDataParse(_In_ const std::string& inputPEPathname, \
  _Out_ std::string& shellcode,
  _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>& unzipcb) {
  bool result = false;
  shared::PEAdditionalDataHead* pHead = nullptr;
  shellcode.clear();
  do {
   std::string current_pe_buffer = shared::Win::File::Read(inputPEPathname);
   if (current_pe_buffer.empty())
    break;
   size_t find_shellcode_identify_head = current_pe_buffer.find(shared::shellcode_identify);
   if (find_shellcode_identify_head == std::string::npos)
    break;
   if (current_pe_buffer.size() - find_shellcode_identify_head < sizeof(shared::PEAdditionalDataHead))
    break;
   pHead = \
    reinterpret_cast<shared::PEAdditionalDataHead*>(current_pe_buffer.data() + find_shellcode_identify_head);
   if (!pHead->Verify())
    break;
   if (current_pe_buffer.size() - find_shellcode_identify_head < pHead->size_total)
    break;
   std::string shell_code_process(pHead->data, pHead->size_shellcode_process);
   if (!unzipcb)
    break;
   if (!unzipcb(shell_code_process, pHead->size_shellcode_original, shellcode))
    break;
#if 0
   if (Z_OK != shared::ZLibs::zipUnCompress(shell_code_process, pHead->size_shellcode_original, shell_code))
    break;
#endif
   result = true;
  } while (0);
  return result;
 }

 void* Win::PELoadPrivateDynamicLinkLibrary(const std::string& pebuffer, const std::string& route_data, void** lpuninit, const bool& unload /*= false*/) {
  void* result = nullptr;
  using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
  using tf_api_object_uninit = void(__stdcall*)(void);
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
  PE::HMEMORYMODULE hModule = nullptr;
  do {
   if (pebuffer.size() < sizeof(IMAGE_DOS_HEADER))
    break;
   if (pebuffer[0] != 'M' || pebuffer[1] != 'Z')
    break;
   hModule = PE::MemoryLoadLibrary(pebuffer.data(), pebuffer.size());
   if (!hModule)
    break;
   api_object_init = reinterpret_cast<decltype(api_object_init)>(PE::MemoryGetProcAddress(hModule, "api_object_init"));
   api_object_uninit = reinterpret_cast<decltype(api_object_uninit)>(PE::MemoryGetProcAddress(hModule, "api_object_uninit"));
   if (!api_object_init || !api_object_uninit)
    break;
   *lpuninit = api_object_uninit;
   result = api_object_init(route_data.empty() ? nullptr : route_data.data(), static_cast<unsigned long>(route_data.size()));
  } while (0);
  if (result && unload && api_object_uninit) {
   api_object_uninit();
   PE::MemoryFreeLibrary(hModule);
   hModule = nullptr;
  }
  return result;
 }
 bool Win::PEExecute(const std::string& pe_buffer, const std::string& route_data, const bool& unload /*= true*/) {
  bool result = false;
  PE::HMEMORYMODULE hModule = nullptr;
  using tf_api_object_init = void(__stdcall*)(const void*, unsigned long);
  using tf_api_object_uninit = void(__stdcall*)(void);
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
  do {
   if (pe_buffer.empty())
    break;
   hModule = PE::MemoryLoadLibrary(pe_buffer.data(), pe_buffer.size());
   if (!hModule)
    break;
   api_object_init = reinterpret_cast<decltype(api_object_init)>(PE::MemoryGetProcAddress(hModule, "api_object_init"));
   api_object_uninit = reinterpret_cast<decltype(api_object_uninit)>(PE::MemoryGetProcAddress(hModule, "api_object_uninit"));
   if (!api_object_init || !api_object_uninit)
    break;
   route_data.empty() ?
    api_object_init(nullptr, 0) :
    api_object_init(route_data.data(), static_cast<unsigned long>(route_data.size()));
   if (unload)
    api_object_uninit();
   result = true;
  } while (0);
  if (unload && hModule) {
   PE::MemoryFreeLibrary(hModule);
   hModule = nullptr;
  }
  return result;
 }
#if 0//!@ 获取文件安全描述符
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "accctrl.h"
#include "aclapi.h"
#pragma comment(lib, "advapi32.lib")

 int main(void)
 {
  DWORD dwRtnCode = 0;
  PSID pSidOwner = NULL;
  BOOL bRtnBool = TRUE;
  LPTSTR AcctName = NULL;
  LPTSTR DomainName = NULL;
  DWORD dwAcctName = 1, dwDomainName = 1;
  SID_NAME_USE eUse = SidTypeUnknown;
  HANDLE hFile;
  PSECURITY_DESCRIPTOR pSD = NULL;


  // Get the handle of the file object.
  hFile = CreateFile(
   TEXT("myfile.txt"),
   GENERIC_READ,
   FILE_SHARE_READ,
   NULL,
   OPEN_EXISTING,
   FILE_ATTRIBUTE_NORMAL,
   NULL);

  // Check GetLastError for CreateFile error code.
  if (hFile == INVALID_HANDLE_VALUE) {
   DWORD dwErrorCode = 0;

   dwErrorCode = GetLastError();
   _tprintf(TEXT("CreateFile error = %d\n"), dwErrorCode);
   return -1;
  }



  // Get the owner SID of the file.
  dwRtnCode = GetSecurityInfo(
   hFile,
   SE_FILE_OBJECT,
   OWNER_SECURITY_INFORMATION,
   &pSidOwner,
   NULL,
   NULL,
   NULL,
   &pSD);

  // Check GetLastError for GetSecurityInfo error condition.
  if (dwRtnCode != ERROR_SUCCESS) {
   DWORD dwErrorCode = 0;

   dwErrorCode = GetLastError();
   _tprintf(TEXT("GetSecurityInfo error = %d\n"), dwErrorCode);
   return -1;
  }

  // First call to LookupAccountSid to get the buffer sizes.
  bRtnBool = LookupAccountSid(
   NULL,           // local computer
   pSidOwner,
   AcctName,
   (LPDWORD)&dwAcctName,
   DomainName,
   (LPDWORD)&dwDomainName,
   &eUse);

  // Reallocate memory for the buffers.
  AcctName = (LPTSTR)GlobalAlloc(
   GMEM_FIXED,
   dwAcctName);

  // Check GetLastError for GlobalAlloc error condition.
  if (AcctName == NULL) {
   DWORD dwErrorCode = 0;

   dwErrorCode = GetLastError();
   _tprintf(TEXT("GlobalAlloc error = %d\n"), dwErrorCode);
   return -1;
  }

  DomainName = (LPTSTR)GlobalAlloc(
   GMEM_FIXED,
   dwDomainName);

  // Check GetLastError for GlobalAlloc error condition.
  if (DomainName == NULL) {
   DWORD dwErrorCode = 0;

   dwErrorCode = GetLastError();
   _tprintf(TEXT("GlobalAlloc error = %d\n"), dwErrorCode);
   return -1;

  }

  // Second call to LookupAccountSid to get the account name.
  bRtnBool = LookupAccountSid(
   NULL,                   // name of local or remote computer
   pSidOwner,              // security identifier
   AcctName,               // account name buffer
   (LPDWORD)&dwAcctName,   // size of account name buffer 
   DomainName,             // domain name
   (LPDWORD)&dwDomainName, // size of domain name buffer
   &eUse);                 // SID type

  // Check GetLastError for LookupAccountSid error condition.
  if (bRtnBool == FALSE) {
   DWORD dwErrorCode = 0;

   dwErrorCode = GetLastError();

   if (dwErrorCode == ERROR_NONE_MAPPED)
    _tprintf(TEXT
    ("Account owner not found for specified SID.\n"));
   else
    _tprintf(TEXT("Error in LookupAccountSid.\n"));
   return -1;

  }
  else if (bRtnBool == TRUE)

   // Print the account name.
   _tprintf(TEXT("Account owner = %s\n"), AcctName);

  return 0;
 }
#endif
 bool Win::CheckKnownDllSymbolicLink(_In_ LPCWSTR pwszDllName, _In_ LPCWSTR pwszTarget) {
  bool result = false;
  NTSTATUS status = 0;
  LPWSTR pwszLinkName = NULL;
  LPWSTR pwszTargetLocal = NULL;
  OBJECT_ATTRIBUTES oa = { 0 };
  UNICODE_STRING name = { 0 };
  UNICODE_STRING target = { 0 };
  HANDLE hLink = NULL;
  ULONG length = 0;
  do {
   pwszLinkName = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
   if (!pwszLinkName)
    break;
   pwszTargetLocal = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
   if (!pwszTargetLocal)
    break;
   StringCchPrintf(pwszLinkName, MAX_PATH, L"\\KnownDlls\\%ws", pwszDllName);
   RtlInitUnicodeString(&name, pwszLinkName);
   InitializeObjectAttributes(&oa, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
   status = NtOpenSymbolicLinkObject(&hLink, SYMBOLIC_LINK_QUERY | STANDARD_RIGHTS_READ, &oa);
   SetLastError(RtlNtStatusToDosError(status));
   if (status != 0)
    break;
   target.Buffer = pwszTargetLocal;
   target.Length = 0;
   target.MaximumLength = MAX_PATH * sizeof(WCHAR);
   status = NtQuerySymbolicLinkObject(hLink, &target, &length);
   SetLastError(RtlNtStatusToDosError(status));
   if (status != 0)
    break;
   result = ::_wcsicmp(target.Buffer, pwszTarget) == 0;
  } while (0);
  if (pwszLinkName)
   LocalFree(pwszLinkName);
  if (pwszTargetLocal)
   LocalFree(pwszTargetLocal);
  if (hLink)
   CloseHandle(hLink);
  return result;
 }

 bool Win::ObjectManagerCreateSymlink(LPCWSTR linkname, LPCWSTR targetname, HANDLE& hResult) {
  OBJECT_ATTRIBUTES oa = { 0 };
  UNICODE_STRING name = { 0 };
  UNICODE_STRING target = { 0 };
  hResult = NULL;
  NTSTATUS status = 0;
  RtlInitUnicodeString(&name, linkname);
  RtlInitUnicodeString(&target, targetname);
  InitializeObjectAttributes(&oa, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
  status = ::NtCreateSymbolicLinkObject(&hResult, SYMBOLIC_LINK_ALL_ACCESS, &oa, &target);
  return NT_SUCCESS(status) ? true : false;
 }

 bool Win::ObjectManagerCreateDirectory(LPCWSTR dirname, HANDLE& hResult) {
  OBJECT_ATTRIBUTES oa = { 0 };
  UNICODE_STRING name = { 0 };
  hResult = NULL;
  NTSTATUS status = 0;
  ::RtlInitUnicodeString(&name, dirname);
  InitializeObjectAttributes(&oa, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
  status = ::NtCreateDirectoryObjectEx(&hResult, DIRECTORY_ALL_ACCESS, &oa, NULL, FALSE);
  return NT_SUCCESS(status) ? true : false;
 }

 void Win::EnumProcessWindows(const DWORD& ProcessId, std::set<Win::WindowsInfo>& wndInfos) {
  wndInfos.clear();
  if (ProcessId <= 4)
   return;
  auto tieRoute = std::make_tuple(ProcessId, &wndInfos);
  ::EnumWindows([](HWND hwnd, LPARAM route)->BOOL {
   auto pRoute = reinterpret_cast<std::tuple<DWORD, std::set<Win::WindowsInfo>*>*>(route);
  DWORD dwProcessId = std::get<0>(*pRoute);
  std::set<Win::WindowsInfo>* pOutInfos = std::get<1>(*pRoute);
  DWORD thePid = 0;
  ::GetWindowThreadProcessId(hwnd, &thePid);
  if (thePid == dwProcessId) {
   Win::WindowsInfo info;
   info.hWnd = hwnd;
   ::GetWindowTextA(hwnd, info.Text, sizeof(info.Text));
   ::GetClassNameA(hwnd, info.Class, sizeof(info.Class));
   pOutInfos->emplace(info);
#if 0
   (*(HWND*)route) = hwnd;
   return FALSE;
#endif
  }
  return TRUE;
   },
   (LPARAM)&tieRoute);
 }
 void Win::EnumChildWindows(const HWND& hProcessWnd, std::set<Win::WindowsInfo>& wndInfos) {
  wndInfos.clear();
  if (!hProcessWnd)
   return;
  ::EnumChildWindows(hProcessWnd,
   [](HWND hwnd, LPARAM route)->BOOL {
    auto pOutInfos = reinterpret_cast<std::set<Win::WindowsInfo>*>(route);
  Win::WindowsInfo info;
  info.hWnd = hwnd;
  ::GetWindowTextA(hwnd, info.Text, sizeof(info.Text));
  ::GetClassNameA(hwnd, info.Class, sizeof(info.Class));
  pOutInfos->emplace(info);
  return TRUE;
   }, (LPARAM)&wndInfos);
 }
 bool Win::AdjustPrivilegeFromAdministrotorToSystem(const std::function<void(const HANDLE&, const DWORD&)>& call, const DWORD& inProcessId /*= 0*/) {
  bool result = false;
  HANDLE hTargetProcess = nullptr;
  HANDLE hTargetProcessAction = nullptr;
  HANDLE hTargetProcessToken = nullptr;
  bool bImpersonateLoggedOnUser = false;
  DWORD targetProcessId = inProcessId;
  do {
   if (targetProcessId <= 0) {
    std::set<DWORD> pids = Win::Process::Get("winlogon.exe");
    if (pids.empty())
     break;
    targetProcessId = *pids.begin();
   }
   hTargetProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, targetProcessId);
   if (!hTargetProcess)
    hTargetProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, targetProcessId);
   if (!hTargetProcess)
    break;
   if (TRUE != ::OpenProcessToken(hTargetProcess,
    TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &hTargetProcessToken))
    break;
   bImpersonateLoggedOnUser = ::ImpersonateLoggedOnUser(hTargetProcessToken);
   if (bImpersonateLoggedOnUser == FALSE)
    break;
   hTargetProcessAction = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
   if (call)
    call(hTargetProcessAction, targetProcessId);
#if 0//!@ Copy token.
   HANDLE hCopyToken = nullptr;
   BOOL bDuplicateTokenEx = ::DuplicateTokenEx(
    hWinlogonToken,
    TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID | TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, NULL,
    SECURITY_IMPERSONATION_LEVEL::SecurityImpersonation,
    TOKEN_TYPE::TokenPrimary, &hCopyToken);
   pLog->Info("DuplicateTokenEx({})", bDuplicateTokenEx ? "success" : "failed");

   STARTUPINFOW StartupInfo = { 0 };
   PROCESS_INFORMATION ProcessInformation = { 0 };

   BOOL bCreateProcessWithTokenW = ::CreateProcessWithTokenW(hCopyToken,
    LOGON_WITH_PROFILE,
    L"C:\\Windows\\System32\\cmd.exe", NULL, 0, NULL, NULL, &StartupInfo, &ProcessInformation);
   pLog->Info("CreateProcessWithTokenW({})", bCreateProcessWithTokenW ? "success" : "failed");
#endif
  } while (0);
  if (bImpersonateLoggedOnUser)
   result = ::RevertToSelf() == TRUE;
  SK_CLOSE_HANDLE(hTargetProcessAction);
  SK_CLOSE_HANDLE(hTargetProcess);
  SK_CLOSE_HANDLE(hTargetProcessToken);
  return result;
 }
 bool Win::CMD(const std::string& cmd, const DWORD& wait_time /*= INFINITE*/) {
  if (cmd.empty())
   return false;
  DWORD dwCmdProcessId = 0;
  STARTUPINFOA si = { 0 };
  PROCESS_INFORMATION pi = { 0 };
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  bool result = false;
  char* __cmd = new char[cmd.size() + 1];
  ::memset(__cmd, 0x00, cmd.size() + 1);
  ::memcpy(__cmd, cmd.data(), cmd.size());
  DWORD dwWaitResult = 0;
  do {
   // Start the child process. 
   if (0 == ::CreateProcessA(NULL,   // No module name (use command line)
    __cmd,        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    TRUE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi)           // Pointer to PROCESS_INFORMATION structure
    )
    break;
   // Wait until child process exits.
   dwWaitResult = ::WaitForSingleObject(pi.hProcess, wait_time);
  } while (0);
  dwCmdProcessId = pi.dwProcessId;
  SK_CLOSE_HANDLE(pi.hThread);
  SK_CLOSE_HANDLE(pi.hProcess);
  result = true;
#if 0
  switch (dwWaitResult) {
  case WAIT_OBJECT_0: {		// Close process and thread handles. 
  }break;
  default: {
   result = false;
  }break;
  }
#endif
  SK_DELETE_PTR_BUFFER(__cmd);
  if (dwCmdProcessId > 0)
   Win::Process::Terminate(dwCmdProcessId);
  return result;
 }
 bool Win::CompareTokenSid(const PSID& pSid1, const PSID& pSid2, const DWORD& foundPid /*= 0*/) {
  bool result = false;
  LPWSTR pwszSid1String = nullptr;
  LPWSTR pwszSid2String = nullptr;
  do {
   if (!pSid1 || !pSid2)
    break;
   if (FALSE == ::ConvertSidToStringSidW(pSid1, &pwszSid1String))
    break;
   if (FALSE == ::ConvertSidToStringSidW(pSid2, &pwszSid2String))
    break;
   //#ifdef _DEBUG
   //			std::wcout << std::format(L"Found ({}) in ({}) and pid({}),", pwszSid2String, pwszSid1String, foundPid) << std::endl;
   //#endif
   if (::_wcsicmp(pwszSid1String, pwszSid2String) != 0)
    break;
   result = true;
  } while (0);
  if (pwszSid1String) {
   ::LocalFree(pwszSid1String);
   pwszSid1String = nullptr;
  }
  if (pwszSid2String) {
   ::LocalFree(pwszSid2String);
   pwszSid2String = nullptr;
  }
  return result;
 }
 bool Win::IsTokenRestricted(const HANDLE& hToken) {
  bool result = true;
  DWORD dwSize = 0;
  PTOKEN_GROUPS pTokenGroups = nullptr;
  do {
   if (!hToken)
    break;
   if (FALSE == ::GetTokenInformation(hToken, TokenRestrictedSids, NULL, 0, &dwSize)) {
    if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
     break;
   }
   if (dwSize <= 0)
    break;
   pTokenGroups = (PTOKEN_GROUPS)::LocalAlloc(LPTR, dwSize);
   if (!pTokenGroups)
    break;
   if (FALSE == ::GetTokenInformation(hToken, TokenRestrictedSids, pTokenGroups, dwSize, &dwSize))
    break;
   if (pTokenGroups->GroupCount != 0)
    break;
   result = false;
  } while (0);
  if (pTokenGroups) {
   ::LocalFree(pTokenGroups);
   pTokenGroups = nullptr;
  }
  return result;
 }
 bool Win::GetSidByToken(const HANDLE& hToken, PSID& ppSid) {
  bool result = false;
  DWORD dwSize = 0;
  PTOKEN_USER pTokenUser = NULL;
  do {
   if (!hToken)
    break;
   ::GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
   if (dwSize <= 0)
    break;
   pTokenUser = (PTOKEN_USER)::LocalAlloc(LPTR, dwSize);
   if (FALSE == ::GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize))
    break;
   ppSid = (PSID)LocalAlloc(LPTR, SECURITY_MAX_SID_SIZE);
   if (!ppSid)
    break;
   if (FALSE == ::CopySid(SECURITY_MAX_SID_SIZE, ppSid, pTokenUser->User.Sid))
    break;
   result = true;
  } while (0);
  if (!result) {
   if (ppSid) {
    ::LocalFree(ppSid);
    ppSid = nullptr;
   }
  }
  if (pTokenUser) {
   ::LocalFree(pTokenUser);
   pTokenUser = nullptr;
  }
  return result;
 }
 bool Win::EnumProcessHandleByStringSid(const std::wstring& sidString, const std::function<bool(const HANDLE&)>& enum_cb) {
  bool result = false;
  do {
   NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
   PSID pSid = nullptr;
   PVOID pSystemHandleInformationBrffer = nullptr;
   ULONG SystemHandleInformationReturnLength = 0;
   do {
    if (sidString.empty())
     break;
    if (FALSE == ::ConvertStringSidToSidW(sidString.c_str(), &pSid))
     break;
    pSystemHandleInformationBrffer = ::LocalAlloc(LPTR, 0x1000);
    if (!pSystemHandleInformationBrffer)
     break;
    bool query_success = true;
    do {
     status = ::NtQuerySystemInformation(
      SYSTEM_INFORMATION_CLASS::SystemHandleInformation,
      pSystemHandleInformationBrffer,
      SystemHandleInformationReturnLength,
      &SystemHandleInformationReturnLength);
     if (STATUS_INFO_LENGTH_MISMATCH == status) {
      ::LocalFree(pSystemHandleInformationBrffer);
      pSystemHandleInformationBrffer = ::LocalAlloc(LPTR, SystemHandleInformationReturnLength);
      if (!pSystemHandleInformationBrffer) {
       query_success = false;
       break;
      }
      continue;
     }
     break;
    } while (1);
    if (!NT_SUCCESS(status) || !query_success)
     break;
    PSYSTEM_HANDLE_INFORMATION pSystemHandleInformation = (PSYSTEM_HANDLE_INFORMATION)pSystemHandleInformationBrffer;
    for (ULONG i = 0; i < pSystemHandleInformation->NumberOfHandles; ++i) {
     PSYSTEM_HANDLE_TABLE_ENTRY_INFO pSystemHandleTableEntryInfo = (PSYSTEM_HANDLE_TABLE_ENTRY_INFO)&pSystemHandleInformation->Handles[i];
     if (!pSystemHandleTableEntryInfo)
      continue;
     if (pSystemHandleTableEntryInfo->ObjectTypeIndex != SYSTEM_INFORMATION_CLASS::SystemProcessInformation)
      continue;
     if (pSystemHandleTableEntryInfo->UniqueProcessId <= 4)
      continue;
     HANDLE hProcess = nullptr;
     HANDLE hProcessToken = nullptr;
     HANDLE hProcessTokenCopy = nullptr;
     PSID pProcessTokenSid = nullptr;
     bool found = false;
     do {
      hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pSystemHandleTableEntryInfo->UniqueProcessId);
      if (!hProcess)
       hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, pSystemHandleTableEntryInfo->UniqueProcessId);
      if (!hProcess)
       break;
      if (FALSE == ::OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &hProcessToken))
       break;
      if (FALSE == ::DuplicateTokenEx(hProcessToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hProcessTokenCopy))
       break;
      if (!Win::GetSidByToken(hProcessTokenCopy, pProcessTokenSid))
       break;
      if (!Win::CompareTokenSid(pSid, pProcessTokenSid, pSystemHandleTableEntryInfo->UniqueProcessId))
       break;
      if (Win::IsTokenRestricted(hProcessTokenCopy))
       break;
      found = true;
     } while (0);
     if (pProcessTokenSid) {
      ::LocalFree(pProcessTokenSid);
      pProcessTokenSid = nullptr;
     }
     SK_CLOSE_HANDLE(hProcessToken);
     SK_CLOSE_HANDLE(hProcess);
     if (!found) {
      SK_CLOSE_HANDLE(hProcessTokenCopy);
     }
     else {
      /*hToken = hProcessTokenCopy;
      break;*/
      if (enum_cb) {
       if (!enum_cb(hProcessTokenCopy))
        break;
      }
      else {
       break;
      }
      if (!result)
       result = true;
     }
    }
   } while (0);
   if (pSystemHandleInformationBrffer) {
    ::LocalFree(pSystemHandleInformationBrffer);
    pSystemHandleInformationBrffer = nullptr;
   }
   if (pSid) {
    ::LocalFree(pSid);
    pSid = nullptr;
   }

  } while (0);
  return result;
 }
 bool Win::FindProcessToken(const wchar_t* SidString, HANDLE& hToken) {
  bool result = false;
  hToken = nullptr;

  NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
  PSID pSid = nullptr;
  PVOID pSystemHandleInformationBrffer = nullptr;
  ULONG SystemHandleInformationReturnLength = 0;
  do {
   if (!SidString)
    break;
   if (FALSE == ::ConvertStringSidToSidW(SidString, &pSid))
    break;
   pSystemHandleInformationBrffer = ::LocalAlloc(LPTR, 0x1000);
   if (!pSystemHandleInformationBrffer)
    break;
   bool query_success = true;
   do {
    status = ::NtQuerySystemInformation(
     SYSTEM_INFORMATION_CLASS::SystemHandleInformation,
     pSystemHandleInformationBrffer,
     SystemHandleInformationReturnLength,
     &SystemHandleInformationReturnLength);
    if (STATUS_INFO_LENGTH_MISMATCH == status) {
     ::LocalFree(pSystemHandleInformationBrffer);
     pSystemHandleInformationBrffer = ::LocalAlloc(LPTR, SystemHandleInformationReturnLength);
     if (!pSystemHandleInformationBrffer) {
      query_success = false;
      break;
     }
     continue;
    }
    break;
   } while (1);
   if (!NT_SUCCESS(status) || !query_success)
    break;
   PSYSTEM_HANDLE_INFORMATION pSystemHandleInformation = (PSYSTEM_HANDLE_INFORMATION)pSystemHandleInformationBrffer;
   for (decltype(pSystemHandleInformation->NumberOfHandles) i = 0; i < pSystemHandleInformation->NumberOfHandles; ++i) {
    PSYSTEM_HANDLE_TABLE_ENTRY_INFO pSystemHandleTableEntryInfo = (PSYSTEM_HANDLE_TABLE_ENTRY_INFO)&pSystemHandleInformation->Handles[i];
    if (!pSystemHandleTableEntryInfo)
     continue;
    if (pSystemHandleTableEntryInfo->ObjectTypeIndex != SYSTEM_INFORMATION_CLASS::SystemProcessInformation)
     continue;
    if (pSystemHandleTableEntryInfo->UniqueProcessId <= 4)
     continue;
    HANDLE hProcess = nullptr;
    HANDLE hProcessToken = nullptr;
    HANDLE hProcessTokenCopy = nullptr;
    PSID pProcessTokenSid = nullptr;
    bool found = false;
    do {
     hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pSystemHandleTableEntryInfo->UniqueProcessId);
     if (!hProcess)
      hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, pSystemHandleTableEntryInfo->UniqueProcessId);
     if (!hProcess)
      break;
     if (FALSE == ::OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &hProcessToken))
      break;
     if (FALSE == ::DuplicateTokenEx(hProcessToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hProcessTokenCopy))
      break;
     if (!Win::GetSidByToken(hProcessTokenCopy, pProcessTokenSid))
      break;
     if (!Win::CompareTokenSid(pSid, pProcessTokenSid, pSystemHandleTableEntryInfo->UniqueProcessId))
      break;
     if (Win::IsTokenRestricted(hProcessTokenCopy))
      break;
     found = true;
    } while (0);
    if (pProcessTokenSid) {
     ::LocalFree(pProcessTokenSid);
     pProcessTokenSid = nullptr;
    }
    SK_CLOSE_HANDLE(hProcessToken);
    SK_CLOSE_HANDLE(hProcess);
    if (!found) {
     SK_CLOSE_HANDLE(hProcessTokenCopy);
    }
    else {
     hToken = hProcessTokenCopy;
     break;
    }
   }
   result = hToken != nullptr;
  } while (0);
  if (pSystemHandleInformationBrffer) {
   ::LocalFree(pSystemHandleInformationBrffer);
   pSystemHandleInformationBrffer = nullptr;
  }
  if (pSid) {
   ::LocalFree(pSid);
   pSid = nullptr;
  }
  return result;
 }
 bool Win::AdjustTokenPrivilege(const HANDLE& hToken, const std::wstring& strPrivilege, const bool& Enable /*= true*/) {
  bool result = false;
  DWORD dwTokenPrivilegesSize = 0;
  PTOKEN_PRIVILEGES pTokenPrivileges = nullptr;
  LUID_AND_ATTRIBUTES luid_and_attributes = { 0 };
  do {
   if (!hToken || strPrivilege.empty())
    break;
   if (FALSE == ::GetTokenInformation(hToken, TokenPrivileges, NULL, dwTokenPrivilegesSize, &dwTokenPrivilegesSize)) {
    if (ERROR_INSUFFICIENT_BUFFER != ::GetLastError())
     break;
   }
   if (dwTokenPrivilegesSize <= 0)
    break;
   pTokenPrivileges = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, dwTokenPrivilegesSize);
   if (!pTokenPrivileges)
    break;
   if (FALSE == ::GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwTokenPrivilegesSize, &dwTokenPrivilegesSize))
    break;
   for (decltype(pTokenPrivileges->PrivilegeCount) i = 0; i < pTokenPrivileges->PrivilegeCount; i++) {
    luid_and_attributes = pTokenPrivileges->Privileges[i];
    DWORD dwPrivilegeNameLength = 0;
    if (!::LookupPrivilegeNameW(NULL, &luid_and_attributes.Luid, NULL, &dwPrivilegeNameLength))
     if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      break;
    wchar_t* pwszPrivilegeName = nullptr;
    bool found = false;
    do {
     if (dwPrivilegeNameLength <= 0)
      break;
     pwszPrivilegeName = (wchar_t*)::LocalAlloc(LPTR, dwPrivilegeNameLength * sizeof(wchar_t));
     if (!pwszPrivilegeName)
      break;
     if (!::LookupPrivilegeNameW(NULL, &luid_and_attributes.Luid, pwszPrivilegeName, &dwPrivilegeNameLength))
      break;
     if (::_wcsicmp(strPrivilege.c_str(), pwszPrivilegeName))
      break;
     found = true;
     TOKEN_PRIVILEGES token_privileges = { 0 };
     token_privileges.PrivilegeCount = 1;
     token_privileges.Privileges[0].Luid = luid_and_attributes.Luid;
     token_privileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
     if (FALSE == ::AdjustTokenPrivileges(hToken, FALSE, &token_privileges,
      sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
      break;
     result = true;
    } while (0);
    if (pwszPrivilegeName) {
     ::LocalFree(pwszPrivilegeName);
     pwszPrivilegeName = nullptr;
    }
    if (result || found)
     break;
   }///for
  } while (0);
  if (pTokenPrivileges) {
   ::LocalFree(pTokenPrivileges);
   pTokenPrivileges = nullptr;
  }
  return result;
 }
 bool Win::AdjustProcessOrThreadPrivilege(const HANDLE& hProcessOrThread, const std::wstring& strPrivilege, const bool& enabled /*= true*/) {
  bool result = false;
  HANDLE hToken = nullptr;
  bool IsCreateToken = false;
  do {
   if (!hProcessOrThread || strPrivilege.empty())
    break;
   if (FALSE == ::OpenProcessToken(hProcessOrThread,
    TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ADJUST_PRIVILEGES, &hToken))
    break;
   result = Win::AdjustTokenPrivilege(hToken, strPrivilege, enabled);
  } while (0);
  SK_CLOSE_HANDLE(hToken);
  return result;
 }
 bool Win::Impersonate(const DWORD& ProcessId, const std::function<void()>& ImpersonateCb) {
  bool result = false;
  HANDLE hProcess = nullptr;
  HANDLE hProcessToken = nullptr;
  do {
   hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
   if (!hProcess)
    break;
   if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hProcessToken) || !hProcessToken)
    break;
   if (!Impersonate(hProcessToken, ImpersonateCb))
    break;
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(hProcessToken);
  SK_CLOSE_HANDLE(hProcess);
  return result;
 }
 bool Win::Impersonate(const HANDLE& hToken, const std::function<void()>& ImpersonateCb) {
  bool result = false;
  do {
   if (!hToken)
    break;
   if (FALSE == ::ImpersonateLoggedOnUser(hToken))
    break;
   result = true;
   if (ImpersonateCb)
    ImpersonateCb();
  } while (0);
  if (result)
   ::RevertToSelf();
  return result;
 }
 void Win::ImpersonateLoggedOnUserCb(const DWORD& UserProcessId, const std::function<void()>& call) {
  BOOL bImpersonateLoggedOnUser = FALSE;
  HANDLE hTokenUser = nullptr;
  HANDLE hProcess = nullptr;
  do {
#if 0
#ifndef _DEBUG
   if (!CurrentUserIsLocalSystem())
    break;
#endif
#endif

   if (UserProcessId <= 0)
    break;
   hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, UserProcessId);
   if (!hProcess)
    break;
   if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hTokenUser))
    break;
   bImpersonateLoggedOnUser = ::ImpersonateLoggedOnUser(hTokenUser);
   if (bImpersonateLoggedOnUser)
    call();
  } while (0);
  SK_CLOSE_HANDLE(hTokenUser);
  SK_CLOSE_HANDLE(hProcess);
  if (bImpersonateLoggedOnUser == TRUE)
   ::RevertToSelf();
 }
 void Win::ImpersonateLoggedOnUserCb(const std::function<void(const DWORD&)>& call) {
  BOOL bImpersonateLoggedOnUser = FALSE;
  HANDLE hTokenUser = nullptr;
  HANDLE hProcess = nullptr;
  do {
   DWORD dwCurrentUserProcessId = Win::Process::GetCurrentUserProcessId();
   if (dwCurrentUserProcessId <= 0)
    break;
   hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCurrentUserProcessId);
   if (!hProcess)
    break;
   if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hTokenUser))
    break;
   bImpersonateLoggedOnUser = ::ImpersonateLoggedOnUser(hTokenUser);
   if (bImpersonateLoggedOnUser) {
    call(dwCurrentUserProcessId);
   }
  } while (0);
  SK_CLOSE_HANDLE(hTokenUser);
  SK_CLOSE_HANDLE(hProcess);
  if (bImpersonateLoggedOnUser == TRUE)
   ::RevertToSelf();
 }
 bool Win::CurrentUserIsUseradmin() {
  bool result = false;
  PSID psidUseradmin = nullptr;
  do {
   SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
   if (!::AllocateAndInitializeSid(
    &ntAuthority,
    2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &psidUseradmin))
    break;
   BOOL bIsUseradmin = FALSE;
   if (!::CheckTokenMembership(NULL, psidUseradmin, &bIsUseradmin))
    break;
   if (bIsUseradmin == FALSE)
    break;
   result = true;
  } while (0);
  if (psidUseradmin) {
   ::FreeSid(psidUseradmin);
   psidUseradmin = nullptr;
  }
  return result;
 }
 bool Win::CurrentUserIsLocalSystem() {
  bool result = false;
  PSID psidLocalSystem = nullptr;
  do {
   SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
   if (!::AllocateAndInitializeSid(
    &ntAuthority,
    1,
    SECURITY_LOCAL_SYSTEM_RID,
    0, 0, 0, 0, 0, 0, 0,
    &psidLocalSystem))
    break;
   BOOL bIsLocalSystem = FALSE;
   if (!::CheckTokenMembership(NULL, psidLocalSystem, &bIsLocalSystem))
    break;
   if (bIsLocalSystem == FALSE)
    break;
   result = true;
  } while (0);
  if (psidLocalSystem) {
   ::FreeSid(psidLocalSystem);
   psidLocalSystem = nullptr;
  }
  return result;
 }
 bool Win::IsPathA(const std::string& obj) {
  bool result = false;
  do {
   if (obj.empty())
    break;
   if (*std::prev(obj.end()) != '\\' && *std::prev(obj.end()) != '/')
    break;
   result = true;
  } while (0);
  return result;
 }
 bool Win::IsPathW(const std::wstring& obj) {
  bool result = false;
  do {
   if (obj.empty())
    break;
   if (*std::prev(obj.end()) != L'\\' && *std::prev(obj.end()) != L'/')
    break;
   result = true;
  } while (0);
  return result;
 }
 bool Win::IsPathnameA(const std::string& obj) {
  bool result = false;
  do {
   if (obj.empty())
    break;
   if (*std::prev(obj.end()) == '\\' || *std::prev(obj.end()) == '/')
    break;
   result = true;
  } while (0);
  return result;
 }
 bool Win::IsPathnameW(const std::wstring& obj) {
  bool result = false;
  do {
   if (obj.empty())
    break;
   if (*std::prev(obj.end()) == L'\\' || *std::prev(obj.end()) == L'/')
    break;
   result = true;
  } while (0);
  return result;
 }
 std::string Win::PathnameToPathA(const std::string& path) {
  std::string result{ path };
  do {
   if (result.empty())
    break;
   if (*std::prev(result.end()) == '\\' || *std::prev(result.end()) == '/')
    break;
   result = PathFixedA(path + "\\");
  } while (0);
  return result;
 }
 std::wstring Win::PathnameToPathW(const std::wstring& path) {
  std::wstring result{ path };
  do {
   if (result.empty())
    break;
   if (*std::prev(result.end()) == L'\\' || *std::prev(result.end()) == L'/')
    break;
   result = PathFixedW(path + L"\\");
  } while (0);
  return result;
 }
 std::string Win::PathToPathnameA(const std::string& pathname) {
  std::string result{ pathname };
  do {
   if (result.empty())
    break;
   do {
    if (*std::prev(result.end()) != '\\' && *std::prev(result.end()) != '/')
     break;
    result.pop_back();
   } while (1);
  } while (0);
  return result;
 }
 std::wstring Win::PathToPathnameW(const std::wstring& pathname) {
  std::wstring result{ pathname };
  do {
   if (result.empty())
    break;
   do {
    if (*std::prev(result.end()) != L'\\' && *std::prev(result.end()) != L'/')
     break;
    result.pop_back();
   } while (1);
  } while (0);
  return result;
 }

 bool Win::SetLnkInformation(const std::string& lnkPathname, const std::string& WorkingDirectory, const std::string& ShellPathname, const std::string& Arguments) {
  bool result = false;
  HRESULT hr = S_FALSE;
  IPersistFile* pIPersistFile = NULL;
  IShellLink* pIShellLink = NULL;
  do {
   if (!AccessA(lnkPathname))
    break;
   if (WorkingDirectory.empty() && Arguments.empty() && ShellPathname.empty())
    break;
   hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IPersistFile, (void**)(&pIPersistFile));
   if (hr != S_OK)
    break;
   hr = pIPersistFile->QueryInterface(IID_IShellLink, (void**)(&pIShellLink));
   if (hr != S_OK)
    break;
   hr = pIPersistFile->Load(IConv::MBytesToWString(lnkPathname).c_str(), STGM_READWRITE);
   if (hr != S_OK)
    break;

   if (!WorkingDirectory.empty()) {
    pIShellLink->SetWorkingDirectory(IConv::MBytesToWString(WorkingDirectory).c_str());
   }

   if (!ShellPathname.empty()) {
    pIShellLink->SetPath(IConv::MBytesToWString(ShellPathname).c_str());
   }

   if (!Arguments.empty()) {
    pIShellLink->SetArguments(IConv::MBytesToWString(Arguments).c_str());
   }

   hr = pIPersistFile->Save(IConv::MBytesToWString(lnkPathname).c_str(), TRUE);
   if (hr != S_OK)
    break;
   result = true;
  } while (0);
  SK_COM_RELEASE(pIShellLink);
  SK_COM_RELEASE(pIPersistFile);
  return result;
 }
 bool Win::GetLnkInformation(const std::string& lnkPathname, std::string& WorkingDirectory, std::string& ShellPathname) {
  bool result = false;
  WorkingDirectory.clear();
  ShellPathname.clear();
  HRESULT hr = S_FALSE;
  IPersistFile* pIPersistFile = NULL;
  IShellLink* pIShellLink = NULL;
  do {
   if (!AccessA(lnkPathname))
    break;
   if (hr != S_OK)
    break;
   hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IPersistFile, (void**)(&pIPersistFile));
   if (hr != S_OK)
    break;
   hr = pIPersistFile->QueryInterface(IID_IShellLink, (void**)(&pIShellLink));
   if (hr != S_OK)
    break;
   hr = pIPersistFile->Load(IConv::MBytesToWString(lnkPathname).c_str(), STGM_READ);
   if (hr != S_OK)
    break;
   WIN32_FIND_DATA wfd = { 0 };
   TCHAR ShellPath[_MAX_PATH] = { 0 };
   hr = pIShellLink->GetPath(ShellPath, _MAX_PATH, NULL, NULL);/*(WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);*/
   if (hr == S_OK)
    ShellPathname = IConv::WStringToMBytes(ShellPath);

   TCHAR wWorkingDirectory[_MAX_PATH] = { 0 };
   hr = pIShellLink->GetWorkingDirectory(wWorkingDirectory, _MAX_PATH);
   if (hr == S_OK)
    WorkingDirectory = IConv::WStringToMBytes(wWorkingDirectory);

   if (!WorkingDirectory.empty()) {
    if (*std::prev(WorkingDirectory.end()) != '\\' || *std::prev(WorkingDirectory.end()) != '/')
     WorkingDirectory.push_back('\\');
   }
   result = !ShellPathname.empty() || !WorkingDirectory.empty();
  } while (0);
  SK_COM_RELEASE(pIShellLink);
  SK_COM_RELEASE(pIPersistFile);
  return result;
 }

 std::string Win::GenerateHexBuffer(const std::string& buffer, const std::string& variable /*= "scbuffer"*/, const char* separator /*= nullptr*/) {
  std::string result{};
  do {
   if (buffer.empty())
    break;
   std::stringstream stream;
   for (auto it = buffer.begin(); it != buffer.end(); ++it) {
    if (separator)
     stream << "0x";
    stream << std::format("{:02X}", static_cast<std::uint8_t>(*it));
    if (separator)
     stream << separator;
   }
   result = stream.str();
   result.insert(0, "{");
   result.insert(0, std::format("static const unsigned char {}[]=", variable));
   result.append("};");
  } while (0);
  return result;
 }


 std::string Win::BufferToHexBuffer(const std::string& buffer, const char* separator /*= nullptr*/) {
  std::string result;
  do {
   if (buffer.empty())
    break;
   std::stringstream stream;
   for (auto it = buffer.begin(); it != buffer.end(); ++it) {
    if (separator)
     stream << "0x";
    stream << std::format("{:02X}", static_cast<std::uint8_t>(*it));
    if (separator)
     stream << separator;
   }
   result = stream.str();
  } while (0);
  return result;
 }
 std::vector<std::string> Win::EmptyFolder(const std::string& Path, const bool& clearSubDir /*= true*/, const bool& clearRootDir /*= false*/) {
  std::vector<std::string> result;
  if (Path.empty())
   return result;
  if (!AccessA(Path))
   return result;
  std::map<std::string, std::string> dirs, files;
  EnumFoldersAndFiles(Path, dirs, files, "*.*", clearSubDir);
  for (auto& file : files) {
   result.emplace_back(file.first);
   ::remove((Path + file.first).c_str());
  }
  std::vector<std::string> sortdirs;
  for (auto& dir : dirs)
   sortdirs.emplace_back(dir.first);
  std::sort(sortdirs.begin(), sortdirs.end(),
   [&](const auto& obj1, const auto& obj2)->bool {
    return obj1.size() > obj2.size();
   });
  for (auto& dir : sortdirs) {
   std::string temp{ dir };
   if (temp.empty())
    continue;
   if (*std::prev(Path.end()) == '\\' || *std::prev(Path.end()) == '/') {
    while (*temp.begin() == '\\' || *temp.begin() == '/') {
     temp.erase(temp.begin());
     if (temp.empty())
      break;
    }
    ::_rmdir((Path + temp).c_str());
    continue;
   }
   ::_rmdir((Path + dir).c_str());
  }
  if (clearRootDir)
   ::_rmdir(Path.c_str());
  return result;
 }

 std::shared_ptr<char> Win::SharedAlloc(const size_t& size) {
  std::shared_ptr<char> result;
  do {
   if (size <= 0)
    break;
   result = std::shared_ptr<char>(new char[size], std::default_delete<char[]>());
   ::memset(result.get(), 0x00, size);
  } while (0);
  return result;
 }
 std::string Win::GetAppDataPathA() {
  return Win::PathFixedA(Win::PathPrevA(Win::GetSpecialFolderLocationA(CSIDL_APPDATA)) + "\\");
 }
 std::wstring Win::GetAppDataPathW() {
  return Win::PathFixedW(Win::PathPrevW(Win::GetSpecialFolderLocationW(CSIDL_APPDATA)) + L"\\");
 }
 std::string Win::GetTempPathA() {
  std::string result;
  result.resize(_MAX_PATH, 0x00);
  result.resize(::GetTempPathA(_MAX_PATH, &result[0]));
  return result;
 }
 std::wstring Win::GetTempPathW() {
  std::wstring result;
  result.resize(_MAX_PATH, 0x00);
  result.resize(::GetTempPathW(_MAX_PATH, &result[0]));
  return result;
 }
 void Win::MainParse(int argc, char** argv, std::vector<std::string>& outs) {
  outs.clear();
  do {
   if (argc <= 0 || !argv)
    break;
   for (int i = 0; i < argc; ++i) {
    outs.emplace_back(std::string(argv[i] ? argv[i] : ""));
   }
  } while (0);
 }
 bool Win::IsSystem64Bit() {
  bool result = false;
  do {
   auto hKernel32 = ::GetModuleHandleW(L"kernel32");
   if (!hKernel32)
    break;
   typedef void(__stdcall* LPFN_GetNativeSystemInfo)(__out LPSYSTEM_INFO lpSystemInfo);
   LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = \
    reinterpret_cast<LPFN_GetNativeSystemInfo>(::GetProcAddress(hKernel32, "GetNativeSystemInfo"));
   if (!fnGetNativeSystemInfo)
    break;
   SYSTEM_INFO system_info = { 0 };
   fnGetNativeSystemInfo(&system_info);
   if (system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 \
    || system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    result = true;
  } while (0);
  return result;
 }
 bool Win::IsProcess64Bit(const DWORD& process_id) {
  bool current_system_64bit = IsSystem64Bit();
  bool result = false;
  HANDLE openProcess = nullptr;
  using LPFN_ISWOW64PROCESS = BOOL(__stdcall*)(HANDLE, PBOOL);
  LPFN_ISWOW64PROCESS lpfnIsWow64Process = nullptr;
  HMODULE hKernel32 = ::GetModuleHandleW(L"kernel32");
  do {
   if (!hKernel32)
    break;
   if (process_id <= 0)
    break;
   lpfnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(::GetProcAddress(hKernel32, "IsWow64Process"));
   if (!lpfnIsWow64Process)
    break;
   openProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_id);
   if (!openProcess)
    break;
   BOOL is64 = 0;
   if (!lpfnIsWow64Process(openProcess, &is64))
    break;
   result = is64 == 0 && current_system_64bit;
  } while (0);
  SK_CLOSE_HANDLE(openProcess);
  return result;
 }
 bool Win::CreateDirectoryW(const std::wstring& create_directory_path) {
  return CreateDirectoryA(IConv::WStringToMBytes(create_directory_path));
 }
 bool Win::CreateDirectoryA(const std::string& create_directory_path) {
  bool result = false;
  do {
   if (create_directory_path.empty())
    break;
   std::string temp_directory_path{ create_directory_path };
   do {
    if (::_access(temp_directory_path.c_str(), 0) == 0) {
     result = true;//Directory already exists
     break;
    }
    for (auto it = temp_directory_path.begin(); it != temp_directory_path.end(); ++it) {
     if (*it == '/')
      *it = '\\';
    }
    do {
     auto found = temp_directory_path.find("\\\\");
     if (found == std::string::npos)
      break;
     temp_directory_path.replace(found, ::strlen("\\\\"), "\\");
    } while (1);

    std::vector<std::string> levels;
    do {
     if (::_access(temp_directory_path.c_str(), 0) == 0)
      break;
     if (temp_directory_path.empty())
      break;
     levels.emplace_back(temp_directory_path);
     temp_directory_path.pop_back();
     temp_directory_path = temp_directory_path.substr(0, temp_directory_path.rfind("\\"));
    } while (1);

    std::sort(levels.begin(), levels.end(),
     [&](const auto& obj1, const auto& obj2)->bool {
      return obj1.size() < obj2.size();
     });

    result = true;
    for (auto& level : levels) {
     if (::_mkdir(level.c_str()) != 0) {
      result = false;
      break;
     }
    }
   } while (0);
  } while (0);
  return result;
 }
 std::string Win::GetModuleNameA(const bool& RemoveSuffix /*= false*/, const HINSTANCE& hModule /*= nullptr*/) {
  std::string result;
  do {
   char szModuleFileFullPath[_MAX_PATH] = { 0 };
   const DWORD dwModuleFileNameLength = \
    ::GetModuleFileNameA(hModule, szModuleFileFullPath, _MAX_PATH);
   if (dwModuleFileNameLength > 0) {
    for (DWORD i = dwModuleFileNameLength; i > 0; --i) {
     if (szModuleFileFullPath[i] == '/' || szModuleFileFullPath[i] == '\\') {
      for (DWORD j = i + sizeof(char), k = 0; j < dwModuleFileNameLength; ++j, ++k)
       result.push_back(szModuleFileFullPath[j]);
      break;
     }
    }
   }
   if (RemoveSuffix)
    for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
     if (*rit == '.') {
      *rit = 0;
      result.resize(std::distance(result.begin(), rit.base()) - 1);
      break;
     }
    }
  } while (0);
  return result;
 }
 std::wstring Win::GetModuleNameW(const bool& RemoveSuffix /*= false*/, const HINSTANCE& hModule /*= nullptr*/) {
  std::wstring result;
  do {
   wchar_t wszModuleFileFullPath[_MAX_PATH] = { 0 };
   const DWORD dwModuleFileNameLength = \
    ::GetModuleFileNameW(hModule, wszModuleFileFullPath, _MAX_PATH);
   if (dwModuleFileNameLength > 0) {
    for (DWORD i = dwModuleFileNameLength; i > 0; --i) {
     if (wszModuleFileFullPath[i] == L'/' || wszModuleFileFullPath[i] == L'\\') {
      for (DWORD j = i + sizeof(char), k = 0; j < dwModuleFileNameLength; ++j, ++k)
       result.push_back(wszModuleFileFullPath[j]);
      break;
     }
    }
   }
   if (RemoveSuffix)
    for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
     if (*rit == L'.') {
      *rit = 0;
      result.resize(std::distance(result.begin(), rit.base()) - 1);
      break;
     }
    }
  } while (0);
  return result;
 }
 std::string Win::GetSpecialFolderLocationA(const int& csidl) {
  std::string result;
  LPITEMIDLIST ppidl = nullptr;
  do {
   if (::SHGetSpecialFolderLocation(nullptr, csidl, &ppidl) != S_OK)
    break;
   char pszPath[_MAX_PATH] = { 0 };
   if (!::SHGetPathFromIDListA(ppidl, pszPath))
    break;
   result = pszPath;
   if (result.empty())
    break;
   auto end = std::prev(result.end());
   if (*end != '\\' && *end != '/')
    result.append("\\");
  } while (0);
  if (ppidl) {
   ::CoTaskMemFree(ppidl);
   ppidl = nullptr;
  }
  return result;
 }

 std::wstring Win::GetSpecialFolderLocationW(const int& csidl) {
  std::wstring result;
  LPITEMIDLIST ppidl = nullptr;
  do {
   if (::SHGetSpecialFolderLocation(nullptr, csidl, &ppidl) != S_OK)
    break;
   wchar_t pszPath[_MAX_PATH] = { 0 };
   if (!::SHGetPathFromIDListW(ppidl, pszPath))
    break;
   result = pszPath;
   if (result.empty())
    break;
   auto end = std::prev(result.end());
   if (*end != '\\' && *end != '/')
    result.push_back('\\');
  } while (0);
  if (ppidl) {
   ::CoTaskMemFree(ppidl);
   ppidl = nullptr;
  }
  return result;
 }

 bool Win::AccessA(const std::string& PathOrPathname) {
  bool result = false;
  do {
   if (PathOrPathname.empty())
    break;
   result = ::_access(PathOrPathname.c_str(), 0) == 0;
  } while (0);
  return result;
 }

 bool Win::AccessW(const std::wstring& PathOrPathname) {
  bool result = false;
  do {
   if (PathOrPathname.empty())
    break;
   result = ::_waccess(PathOrPathname.c_str(), 0) == 0;
  } while (0);
  return result;
 }

 std::string Win::GetModulePathA(const HINSTANCE& hModule) {
  std::string result;
  do {
   char Filename[MAX_PATH] = { 0 };
   if (::GetModuleFileNameA(hModule, Filename, MAX_PATH) <= 0)
    break;
   if (FALSE == ::PathRemoveFileSpecA(Filename))
    break;
   result = Filename;
   auto end = std::prev(result.end());
   if (*end != '\\' && *end != '/')
    result.push_back('\\');
  } while (0);
  return result;
 }
 std::wstring Win::GetModulePathW(const HINSTANCE& hModule) {
  std::wstring result;
  do {
   wchar_t Filename[MAX_PATH] = { 0 };
   if (::GetModuleFileNameW(hModule, Filename, MAX_PATH) <= 0)
    break;
   if (FALSE == ::PathRemoveFileSpecW(Filename))
    break;
   result = Filename;
   auto end = std::prev(result.end());
   if (*end != '\\' && *end != '/')
    result.push_back('\\');
  } while (0);
  return result;
 }
 std::string Win::GetModulePathnameA(const HINSTANCE& hModule) {
  std::string result;
  do {
   char Filename[MAX_PATH] = { 0 };
   if (::GetModuleFileNameA(hModule, Filename, MAX_PATH) <= 0)
    break;
   result = Filename;
  } while (0);
  return result;
 }
 std::wstring Win::GetModulePathnameW(const HINSTANCE& hModule) {
  std::wstring result;
  do {
   wchar_t Filename[MAX_PATH] = { 0 };
   if (::GetModuleFileNameW(hModule, Filename, MAX_PATH) <= 0)
    break;
   result = Filename;
  } while (0);
  return result;
 }
 std::wstring Win::RemoveProcessNameSuffixW(const std::wstring& processName) {
  std::wstring result(processName);
  for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
   if (*rit == '.')
   {
    *rit = 0;
    result.resize(std::distance(result.begin(), rit.base()) - 1);
    break;
   }
  }
  return result;
 }
 std::string Win::RemoveProcessNameSuffixA(const std::string& processName) {
  std::string result(processName);
  for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
   if (*rit == '.')
   {
    *rit = 0;
    result.resize(std::distance(result.begin(), rit.base()) - 1);
    break;
   }
  }
  return result;
 }
 std::string Win::PathnamePathAppendA(const std::string& pathname, const std::string& filename) {
  std::string result;
  do {
   if (pathname.empty() || filename.empty())
    break;
   char temp[_MAX_PATH] = { 0 };
   strcpy_s(temp, pathname.c_str());
   if (FALSE == ::PathRemoveFileSpecA(temp))
    break;
   if (FALSE == ::PathAppendA(temp, filename.c_str()))
    break;
   result = temp;
  } while (0);
  return result;
 }
 std::wstring Win::PathnamePathAppendW(const std::wstring& pathname, const std::wstring& filename) {
  std::wstring result;
  do {
   if (pathname.empty() || filename.empty())
    break;
   wchar_t temp[_MAX_PATH] = { 0 };
   wcscpy_s(temp, pathname.c_str());
   if (FALSE == ::PathRemoveFileSpecW(temp))
    break;
   if (FALSE == ::PathAppendW(temp, filename.c_str()))
    break;
   result = temp;
  } while (0);
  return result;
 }
 bool Win::GetFileNameAndFormat(_In_ const std::string& pathname, _Out_ std::string& out_name, _Out_ std::string& out_format) {
  bool result = false;
  out_name.clear();
  out_format.clear();
  do {
   if (pathname.empty())
    break;
   out_name = GetNameByPathnameA(pathname);
   if (out_name.empty())
    break;
   auto rfind = out_name.rfind('.');
   if (rfind == std::string::npos)
    break;
   out_format = out_name.substr(rfind, out_name.size());
   if (rfind > 0)
    out_name.erase(out_name.begin() + rfind, out_name.end());
   else
    out_name.clear();
   result = true;
  } while (0);
  return result;
 }
 std::string Win::GetNameByPathnameA(const std::string& pathname) {
  std::string result;
  for (auto rit = pathname.rbegin(); rit != pathname.rend(); ++rit) {
   result.append(1, *rit);
   if (*rit == '/' || *rit == '\\') {
    if (!result.empty())
     result.pop_back();
    break;
   }
  }
  std::reverse(result.begin(), result.end());
  return result;
 }

 std::wstring Win::GetNameByPathnameW(const std::wstring& pathname) {
  std::wstring result;
  for (auto rit = pathname.rbegin(); rit != pathname.rend(); ++rit) {
   result.append(1, *rit);
   if (*rit == '/' || *rit == '\\') {
    if (!result.empty())
     result.pop_back();
    break;
   }
  }
  std::reverse(result.begin(), result.end());
  return result;
 }
 std::string Win::GetPathByPathnameA(const std::string& pathname) {
  std::string result;
  do {
   if (pathname.empty())
    break;
   char temp[_MAX_PATH] = { 0 };
   strcpy_s(temp, pathname.c_str());
   if (FALSE == ::PathRemoveFileSpecA(temp))
    break;
   result = temp;
   if (result.empty())
    break;
   auto end = std::prev(result.end());
   if (*end != '\\' && *end != '/')
    result.push_back('\\');
  } while (0);
  return result;
 }
 std::wstring Win::GetPathByPathnameW(const std::wstring& pathname) {
  std::wstring result;
  do {
   if (pathname.empty())
    break;
   wchar_t temp[_MAX_PATH] = { 0 };
   wcscpy_s(temp, pathname.c_str());
   if (FALSE == ::PathRemoveFileSpecW(temp))
    break;
   result = temp;
   if (result.empty())
    break;
   auto end = std::prev(result.end());
   if (*end != '\\' && *end != '/')
    result.push_back('\\');
  } while (0);
  return result;
 }
 std::string Win::UrlFixedA(const std::string& inputUrlOrPath) {
  std::string result{ inputUrlOrPath };
  if (result.empty())
   return result;
  for (auto it = result.begin(); it != result.end(); ++it) {
   if (*it == '\\')
    *it = '/';
  }
  do {
   auto found = result.find("//");
   if (found == std::string::npos)
    break;
   result.replace(found, ::strlen("//"), "/");
  } while (1);

  do {
   auto found = result.find(":/");
   if (found == std::string::npos)
    break;
   result.replace(found, ::strlen(":/"), "://");
  } while (0);
  return result;
 }
 std::string Win::PathFixedA(const std::string& PathOrPathname) {
  std::string result{ PathOrPathname };
  if (result.empty())
   return result;
  for (auto it = result.begin(); it != result.end(); ++it) {
   if (*it == '/')
    *it = '\\';
  }
  do {
   auto found = result.find("\\\\");
   if (found == std::string::npos)
    break;
   result.replace(found, ::strlen("\\\\"), "\\");
  } while (1);
  return result;
 }
 std::wstring Win::PathFixedW(const std::wstring& PathOrPathname) {
  std::wstring result{ PathOrPathname };
  if (PathOrPathname.empty())
   return result;
  for (auto it = result.begin(); it != result.end(); ++it) {
   if (*it == '/')
    *it = '\\';
  }
  do {
   auto found = result.find(L"\\\\");
   if (found == std::wstring::npos)
    break;
   result.replace(found, ::wcslen(L"\\\\"), L"\\");
  } while (1);
  return result;
 }
 std::string Win::PathPrevA(const std::string& inPath) {
  std::string result;
  do {
   if (inPath.empty())
    break;
   result = inPath;
   do {
    if (result.empty())
     break;
    if (*std::prev(result.end()) == '\\' || *std::prev(result.end()) == '/')
     result.pop_back();
    else
     break;
   } while (1);

   do {
    if (result.empty())
     break;
    if (*std::prev(result.end()) != '\\' && *std::prev(result.end()) != '/')
     result.pop_back();
    else
     break;
   } while (1);
  } while (0);
  return result;
 }
 std::wstring Win::PathPrevW(const std::wstring& inPath) {
  std::wstring result;
  do {
   if (inPath.empty())
    break;
   result = inPath;
   do {
    if (result.empty())
     break;
    if (*std::prev(result.end()) == L'\\' || *std::prev(result.end()) == L'/')
     result.pop_back();
    else
     break;
   } while (1);

   do {
    if (result.empty())
     break;
    if (*std::prev(result.end()) != L'\\' && *std::prev(result.end()) != L'/')
     result.pop_back();
    else
     break;
   } while (1);
  } while (0);
  return result;
 }
 bool Win::PathOrNameEqualA(const std::string& path1, const std::string& path2) {
  bool result = false;
  do {
   auto obj1 = IConv::ToLowerA(PathFixedA(path1));
   auto obj2 = IConv::ToLowerA(PathFixedA(path2));
   result = obj1.compare(obj2) == 0;
  } while (0);
  return result;
 }
 bool Win::EnableDebugPrivilege(const TCHAR* privilegeName, const bool& bEnable /*= true*/) {
  bool result = false;
  HANDLE hToken = nullptr;
  do {
   LUID luid = { 0 };
   if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    break;
   if (!::LookupPrivilegeValue(nullptr, privilegeName, &luid))
    break;
   TOKEN_PRIVILEGES tokenPriv;
   tokenPriv.PrivilegeCount = 1;
   tokenPriv.Privileges[0].Luid = luid;
   tokenPriv.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
   if (0 == ::AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
    break;
   result = ::GetLastError() == ERROR_SUCCESS;
  } while (0);
  SK_CLOSE_HANDLE(hToken);
  return result;
 }
 bool Win::EnableDebugPrivilege(const bool& bEnable /*= true*/) {
  bool result = false;
  HANDLE hToken = nullptr;
  do {
   LUID luid = { 0 };
   if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    break;
   if (!::LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid))
    break;
   TOKEN_PRIVILEGES tokenPriv;
   tokenPriv.PrivilegeCount = 1;
   tokenPriv.Privileges[0].Luid = luid;
   tokenPriv.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
   if (0 == ::AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
    break;
   result = ::GetLastError() == ERROR_SUCCESS;
  } while (0);
  SK_CLOSE_HANDLE(hToken);
  return result;
 }
 bool Win::EnableImpersonatePrivilege(const bool& bEnable /*= true*/) {
  bool result = false;
  HANDLE hToken = nullptr;
  do {
   LUID luid = { 0 };
   if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    break;
   if (!::LookupPrivilegeValue(nullptr, SE_IMPERSONATE_NAME, &luid))
    break;
   TOKEN_PRIVILEGES tokenPriv;
   tokenPriv.PrivilegeCount = 1;
   tokenPriv.Privileges[0].Luid = luid;
   tokenPriv.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
   if (0 == ::AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
    break;
   result = ::GetLastError() == ERROR_SUCCESS;
  } while (0);
  SK_CLOSE_HANDLE(hToken);
  return result;
 }
 std::string Win::EnumCorrectPath(const std::string& in, const char& replace_at /*= '\\'*/, const bool& isFolder /*= true*/) {
  std::string result{ in };
  if (result.empty())
   return result;
  do {
   auto found = result.find(replace_at == '\\' ? '/' : '\\');
   if (found != std::string::npos) {
    result[found] = replace_at;
    continue;
   }
   found = result.find("\\/");
   if (found != std::string::npos) {
    result.replace(found, strlen("\\/"), sizeof(replace_at), replace_at);
    continue;
   }
   found = result.find("/\\");
   if (found != std::string::npos) {
    result.replace(found, strlen("/\\"), sizeof(replace_at), replace_at);
    continue;
   }
   found = result.find(replace_at == '\\' ? "\\\\" : "//");
   if (found != std::string::npos) {
    result.replace(found, strlen(replace_at == '\\' ? "//" : "\\\\"), sizeof(replace_at), replace_at);
    continue;
   }
   found = result.find(replace_at == '\\' ? "\\\\\\" : "///");
   if (found != std::string::npos) {
    result.replace(found, strlen(replace_at == '\\' ? "\\\\\\" : "///"), sizeof(replace_at), replace_at);
    continue;
   }
   found = result.find(replace_at == '\\' ? "\\\\\\\\" : "////");
   if (found != std::string::npos) {
    result.replace(found, strlen(replace_at == '\\' ? "\\\\\\\\" : "////"), sizeof(replace_at), replace_at);
    continue;
   }
   found = result.find({ replace_at ,replace_at });
   if (found != std::string::npos) {
    result.replace(found, sizeof(replace_at) * 2, sizeof(replace_at), replace_at);
    continue;
   }
   found = result.find({ replace_at ,replace_at,replace_at });
   if (found != std::string::npos) {
    result.replace(found, sizeof(replace_at) * 3, sizeof(replace_at), replace_at);
    continue;
   }
   break;
  } while (1);
  do {
   if (isFolder) {
    if (result.empty()) {
     result.append(sizeof(replace_at), replace_at);
     break;
    }
    if (*std::prev(result.end()) != replace_at) {
     result.push_back(replace_at);
    }
   }
   else {
    if (*result.begin() == replace_at) {
     result.erase(0, sizeof(replace_at));
    }
   }
  } while (0);
  return result;
 }
 void Win::FindFileAssignPathOnce(
  const std::string& path,
  const std::vector<std::string>& ffname_s,
  tfEnumFolderNode& found_s) {
  found_s.clear();
  do {
   tfEnumFolderNode folders, files;
   shared::Win::EnumFolder(path, folders, files, "*.*", true,
    [&](const std::string& pathname, const std::string& identify, const bool& is_directory) {
     bool just_is = false;
   do {
    if (is_directory)
     break;
    const std::string current_target_fullpathname = shared::Win::PathFixedA(path + "\\" + pathname);
    for (const auto& target_fname : ffname_s) {
     if (::_memicmp(target_fname.c_str(), identify.c_str(), target_fname.size()) == 0) {
      if (found_s.find(identify) == found_s.end())
       found_s.emplace(identify, current_target_fullpathname);
     }
    }
   } while (0);
    });
  } while (0);
 }
 void Win::FindFileAssignPath(
  const std::string& path,
  const std::vector<std::string>& ffname_s,
  tfEnumFolderNode& found_s) {
  found_s.clear();
  do {
   tfEnumFolderNode folders, files;
   shared::Win::EnumFolder(path, folders, files, "*.*", true,
    [&](const std::string& pathname, const std::string& identify, const bool& is_directory) {
     bool just_is = false;
   do {
    if (is_directory)
     break;
    const std::string current_target_fullpathname = shared::Win::PathFixedA(path + "\\" + pathname);
    for (const auto& target_fname : ffname_s) {
     if (::_memicmp(target_fname.c_str(), identify.c_str(), target_fname.size()) == 0)
      found_s.emplace(current_target_fullpathname, identify);
    }
   } while (0);
    });
  } while (0);
 }
 void Win::EnumFolder(const std::string& Path,
  tfEnumFolderNode& Folders,
  tfEnumFolderNode& Files,
  const char* FileFilter /*= "*.*"*/,
  bool bSleepDirect /*= false*/,
  const tfEnumFolderCb& enumcb /*= nullptr*/) {
  EnumFoldersAndFiles(Path, Folders, Files, FileFilter, bSleepDirect,
   [&](const auto& pathname, const auto& identify, const auto& findData) {
    if (enumcb)
    enumcb(pathname, identify, (findData.attrib & _A_SUBDIR));
   });
 }
 void Win::EnumFoldersAndFiles(
  const std::string& Path,
  tfEnumFolderNode& Folders,
  tfEnumFolderNode& Files,
  const char* FileFilter /*= "*.*"*/,
  bool bSleepDirect /*= false*/,
  const std::function<void(const std::string&, const std::string&, const _finddata_t&)>& enumcb /*= nullptr*/) {
  if (Path.empty())
   return;
  std::vector<std::string> Paths{ Path };
  std::intptr_t firstIndes{ -1 };
  std::string path_pos;
  std::set<std::string> havedone;
  do {
   if (Paths.empty())
    break;
   auto begin = Paths.begin();
   std::string TargetPath(*begin);
   Paths.erase(begin);
   if (*std::prev(TargetPath.end()) != '\\' && *std::prev(TargetPath.end()) != '/') {
    TargetPath.push_back('\\');
   }
   path_pos = TargetPath;
   havedone.insert(path_pos);
   TargetPath.append(FileFilter);
   _finddata_t  FindData = { 0 };
   firstIndes = _findfirst(TargetPath.c_str(), &FindData);
   do {
    if (firstIndes < 0)
     break;
    bool _break = false;
    do {
     if (!::strcmp(FindData.name, ".") || !::strcmp(FindData.name, ".."))
      continue;
     if (bSleepDirect) {
      if (FindData.attrib & _A_SUBDIR) {
       auto the = path_pos + "\\" + FindData.name + "\\";
       if (havedone.find(the) == havedone.end()) {
        Paths.emplace_back(the);
        the.erase(the.find(Path), Path.size());
        Folders.emplace(std::make_pair(EnumCorrectPath(the), FindData.name));
        if (enumcb) {
         enumcb(the, FindData.name, FindData);
        }
        break;
       }
       else
        _break = true;
      }
      else {
       auto the = path_pos + "\\" + FindData.name;
       the.erase(the.find(Path), Path.size());
       Files.emplace(std::make_pair(EnumCorrectPath(the, '\\', false), FindData.name));
       if (enumcb) {
        enumcb(the, FindData.name, FindData);
       }
      }
      break;
     }
     else {
      if (FindData.attrib & _A_SUBDIR) {
       auto the = path_pos + "\\" + FindData.name + "\\";
       the.erase(the.find(Path), Path.size());
       Folders.emplace(std::make_pair(EnumCorrectPath(the), FindData.name));
       if (enumcb) {
        enumcb(the, FindData.name, FindData);
       }
       break;
      }
      auto the = path_pos + "\\" + FindData.name;
      the.erase(the.find(Path), Path.size());
      Files.emplace(std::make_pair(EnumCorrectPath(the, '\\', false), FindData.name));
      if (enumcb) {
       enumcb(the, FindData.name, FindData);
      }
     }
    } while (0);
    if (_break)
     break;
    ::memset(&FindData, 0x00, sizeof(FindData));
    if (_findnext(firstIndes, &FindData) < 0)
     break;
   } while (1);
  } while (1);
  if (firstIndes >= 0) {
   ::_findclose(firstIndes);
  }
 }
 std::string Win::BinToHex(const std::string& s) {
  std::ostringstream oss;
  for (unsigned char ch : s)
   oss << std::hex << std::setw(2) << std::setfill('0') << (int)ch;
  return oss.str();
 }
 std::string Win::HexToBin(const std::string& s) {
  std::string result;
  do {
   if ((s.length() % 2) != 0)
    break;
   result.reserve(s.length() / 2);
   std::string extract;
   for (std::string::const_iterator pos = s.begin(); pos < s.end(); pos += 2) {
    extract.assign(pos, pos + 2);
    result.push_back(std::stoi(extract, nullptr, 16));
   }
  } while (0);
  return result;
 }
 HWND Win::ConsoleHwndGet() {
  HWND result = nullptr;
  bool load = false;
  HMODULE hKernel32 = nullptr;
  do {
   hKernel32 = ::GetModuleHandle(_T("kernel32"));
   if (!hKernel32) {
    hKernel32 = ::LoadLibraryA("kernel32.dll");
    load = true;
   }
   if (!hKernel32)
    break;
   typedef HWND(WINAPI* PROCGETCONSOLEWINDOW)();
   PROCGETCONSOLEWINDOW GetConsoleWindow;
   GetConsoleWindow = (PROCGETCONSOLEWINDOW)::GetProcAddress(hKernel32, "GetConsoleWindow");
   if (!GetConsoleWindow)
    break;
   result = GetConsoleWindow();
  } while (0);
  if (hKernel32 && load)
   SK_FREE_LIBRARY(hKernel32);
  return result;
 }
 bool Win::WriteConsoleInput(const int& input_char) {
  bool result = false;
  do {
   HANDLE hConIn = ::GetStdHandle(STD_INPUT_HANDLE);
   if (!hConIn)
    break;
   INPUT_RECORD ir[3] = { 0 };

   ir[0].EventType = KEY_EVENT;
   ir[0].Event.KeyEvent.bKeyDown = TRUE;
   ir[0].Event.KeyEvent.dwControlKeyState = 0;
   ir[0].Event.KeyEvent.uChar.UnicodeChar = input_char;
   ir[0].Event.KeyEvent.wRepeatCount = 1;
   ir[0].Event.KeyEvent.wVirtualKeyCode = input_char;
   ir[0].Event.KeyEvent.wVirtualScanCode = ::MapVirtualKey(input_char, MAPVK_VK_TO_VSC);

   ir[1].EventType = KEY_EVENT;
   ir[1].Event.KeyEvent.bKeyDown = TRUE;
   ir[1].Event.KeyEvent.dwControlKeyState = 0;
   ir[1].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
   ir[1].Event.KeyEvent.wRepeatCount = 1;
   ir[1].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
   ir[1].Event.KeyEvent.wVirtualScanCode = ::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);

   ir[2].EventType = KEY_EVENT;
   ir[2].Event.KeyEvent.bKeyDown = FALSE;
   ir[2].Event.KeyEvent.dwControlKeyState = 0;
   ir[2].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
   ir[2].Event.KeyEvent.wRepeatCount = 1;
   ir[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
   ir[2].Event.KeyEvent.wVirtualScanCode = ::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);

   DWORD nLength = 0;
   result = ::WriteConsoleInput(hConIn, ir, 3, &nLength) != FALSE;
  } while (0);
  return result;
 }
 bool Win::ProgramInstanceMark(const std::string& ProgramName) {
  bool result = false;
  HANDLE hMutex = nullptr;
  do {
   if (ProgramName.empty())
    break;
   auto hMutex = ::CreateMutexA(NULL, FALSE, ProgramName.c_str());
   if (hMutex && (::GetLastError() == ERROR_ALREADY_EXISTS))
    break;
   result = true;
  } while (0);
  SK_CLOSE_HANDLE(hMutex);
  return result;
 }
 bool Win::ComparePathOrPathname(const std::string& path1, const std::string& path2) {
  bool result = false;
  do {
   if (path1.empty() && path2.empty()) {
    result = true;
    break;
   }
   std::string tmpPath1{ path1 };
   std::string tmpPath2{ path2 };
   for (auto it = tmpPath1.begin(); it != tmpPath1.end();) {
    if (*it == '\\' || *it == '/')
     it = tmpPath1.erase(it);
    else
     ++it;
   }
   for (auto it = tmpPath2.begin(); it != tmpPath2.end();) {
    if (*it == '\\' || *it == '/')
     it = tmpPath2.erase(it);
    else
     ++it;
   }
   if (tmpPath2.size() != tmpPath1.size())
    break;
   if (::_stricmp(tmpPath1.c_str(), tmpPath2.c_str()))
    break;
   result = true;
  } while (0);
  return result;
 }
 bool Win::CompareUrlEqual(const std::string& url1, const std::string& url2) {
  bool equal = false;
  do {
   std::string fix_url1{ url1 }, fix_url2{ url2 };
   for (auto it = fix_url1.begin(); it != fix_url1.end();) {
    if (*it == '\\' || *it == '/')
     it = fix_url1.erase(it);
    else
     ++it;
   }
   for (auto it = fix_url2.begin(); it != fix_url2.end();) {
    if (*it == '\\' || *it == '/')
     it = fix_url2.erase(it);
    else
     ++it;
   }
   if (fix_url1.size() != fix_url2.size())
    break;
   if (::_stricmp(fix_url1.c_str(), fix_url2.c_str()))
    break;
   equal = true;
  } while (0);
  return equal;
 }
 int Win::CompareVersion(const std::string& ver1, const std::string& ver2) {
  int result = 0;
  do {
   if (ver1.empty() && ver2.empty())
    break;
   if (ver1.empty() && !ver2.empty()) {
    result = -1;
    break;
   }
   if (!ver1.empty() && ver2.empty()) {
    result = 1;
    break;
   }

   std::vector<std::string> par1, par2;
   File::ParseA(ver1, '.', par1);
   File::ParseA(ver2, '.', par2);

   if (par1.empty() || par2.empty())
    break;

   //!@ Fill in the missing version numbers with zeros
   if (par1.size() != par2.size()) {
    if (par1.size() > par2.size()) {
     for (size_t i = 0; i < par1.size() - par2.size(); ++i)
      par2.emplace_back("0");
    }
    else {
     for (size_t i = 0; i < par2.size() - par1.size(); ++i)
      par1.emplace_back("0");
    }
   }

   for (size_t i = 0; i < par1.size(); ++i) {
    auto v1 = ::atol(par1[i].c_str());
    auto v2 = ::atol(par2[i].c_str());
    if (v1 == v2)
     continue;
    result = (v1 > v2) ? 1 : -1;
    break;
   }
  } while (0);
  return result;
 }

 std::wstring Win::GuidW() {
  std::wstring result;
  wchar_t* buffer = new wchar_t[512];
  LPGUID pGuid = nullptr;
  do {
   pGuid = (LPGUID)::LocalAlloc(LPTR, sizeof(GUID));
   if (!pGuid)
    break;
   if (S_OK != ::CoCreateGuid(pGuid))
    break;
   int nLen = ::swprintf_s(buffer, 512,
    L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
    pGuid->Data1, pGuid->Data2, pGuid->Data3,
    pGuid->Data4[0], pGuid->Data4[1], pGuid->Data4[2],
    pGuid->Data4[3], pGuid->Data4[4], pGuid->Data4[5],
    pGuid->Data4[6], pGuid->Data4[7]);
   if (nLen <= 0)
    break;
   result.append(buffer, nLen);
  } while (0);
  if (pGuid) {
   ::LocalFree(pGuid);
   pGuid = nullptr;
  }
  SK_DELETE_PTR_BUFFER(buffer);
  return result;
 }

 std::string Win::GuidA() {
  std::string result;
  char* buffer = new char[512];
  LPGUID pGuid = nullptr;
  do {
   pGuid = (LPGUID)::LocalAlloc(LPTR, sizeof(GUID));
   if (!pGuid)
    break;
   if (S_OK != ::CoCreateGuid(pGuid))
    break;
   int nLen = ::sprintf_s(buffer, 512,
    "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
    pGuid->Data1, pGuid->Data2, pGuid->Data3,
    pGuid->Data4[0], pGuid->Data4[1], pGuid->Data4[2],
    pGuid->Data4[3], pGuid->Data4[4], pGuid->Data4[5],
    pGuid->Data4[6], pGuid->Data4[7]);
   if (nLen <= 0)
    break;
   result.append(buffer, nLen);
  } while (0);
  if (pGuid) {
   ::LocalFree(pGuid);
   pGuid = nullptr;
  }
  SK_DELETE_PTR_BUFFER(buffer);
  return result;
 }

 std::wstring Win::UuidW() {
  std::wstring result;
  LPGUID pUuid = nullptr;
  RPC_WSTR wstrGuid = nullptr;
  do {
   pUuid = (LPGUID)::LocalAlloc(LPTR, sizeof(GUID));
   if (!pUuid)
    break;
   if (::UuidCreate(pUuid) != RPC_S_OK)
    break;
   if (::UuidToStringW(pUuid, &wstrGuid) != RPC_S_OK)
    break;
   result = (LPCWSTR)wstrGuid;
  } while (0);
  if (wstrGuid) {
   if (RPC_S_OK == ::RpcStringFreeW(&wstrGuid))
    wstrGuid = nullptr;
  }
  if (pUuid) {
   ::LocalFree(pUuid);
   pUuid = nullptr;
  }
  if (!result.empty())
   result = IConv::ToUpperW(result);
  return result;
 }

 std::string Win::UuidA() {
  std::string result;
  LPGUID pUuid = nullptr;
  RPC_CSTR strGuid = nullptr;
  do {
   pUuid = (LPGUID)::LocalAlloc(LPTR, sizeof(GUID));
   if (!pUuid)
    break;
   if (::UuidCreate(pUuid) != RPC_S_OK)
    break;
   if (::UuidToStringA(pUuid, &strGuid) != RPC_S_OK)
    break;
   result = (LPCSTR)strGuid;
  } while (0);
  if (strGuid) {
   if (RPC_S_OK == ::RpcStringFreeA(&strGuid))
    strGuid = nullptr;
  }
  if (pUuid) {
   ::LocalFree(pUuid);
   pUuid = nullptr;
  }
  if (!result.empty())
   result = IConv::ToUpperA(result);
  return result;
 }

#if 0
 // InstructionSet.cpp
// Compile by using: cl /EHsc /W4 InstructionSet.cpp
// processor: x86, x64
// Uses the __cpuid intrinsic to get information about
// CPU extended instruction set support.

#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>

 class InstructionSet
 {
  // forward declarations
  class InstructionSet_Internal;

 public:
  // getters
  static std::string Vendor(void) { return CPU_Rep.vendor_; }
  static std::string Brand(void) { return CPU_Rep.brand_; }

  static bool SSE3(void) { return CPU_Rep.f_1_ECX_[0]; }
  static bool PCLMULQDQ(void) { return CPU_Rep.f_1_ECX_[1]; }
  static bool MONITOR(void) { return CPU_Rep.f_1_ECX_[3]; }
  static bool SSSE3(void) { return CPU_Rep.f_1_ECX_[9]; }
  static bool FMA(void) { return CPU_Rep.f_1_ECX_[12]; }
  static bool CMPXCHG16B(void) { return CPU_Rep.f_1_ECX_[13]; }
  static bool SSE41(void) { return CPU_Rep.f_1_ECX_[19]; }
  static bool SSE42(void) { return CPU_Rep.f_1_ECX_[20]; }
  static bool MOVBE(void) { return CPU_Rep.f_1_ECX_[22]; }
  static bool POPCNT(void) { return CPU_Rep.f_1_ECX_[23]; }
  static bool AES(void) { return CPU_Rep.f_1_ECX_[25]; }
  static bool XSAVE(void) { return CPU_Rep.f_1_ECX_[26]; }
  static bool OSXSAVE(void) { return CPU_Rep.f_1_ECX_[27]; }
  static bool AVX(void) { return CPU_Rep.f_1_ECX_[28]; }
  static bool F16C(void) { return CPU_Rep.f_1_ECX_[29]; }
  static bool RDRAND(void) { return CPU_Rep.f_1_ECX_[30]; }

  static bool MSR(void) { return CPU_Rep.f_1_EDX_[5]; }
  static bool CX8(void) { return CPU_Rep.f_1_EDX_[8]; }
  static bool SEP(void) { return CPU_Rep.f_1_EDX_[11]; }
  static bool CMOV(void) { return CPU_Rep.f_1_EDX_[15]; }
  static bool CLFSH(void) { return CPU_Rep.f_1_EDX_[19]; }
  static bool MMX(void) { return CPU_Rep.f_1_EDX_[23]; }
  static bool FXSR(void) { return CPU_Rep.f_1_EDX_[24]; }
  static bool SSE(void) { return CPU_Rep.f_1_EDX_[25]; }
  static bool SSE2(void) { return CPU_Rep.f_1_EDX_[26]; }

  static bool FSGSBASE(void) { return CPU_Rep.f_7_EBX_[0]; }
  static bool BMI1(void) { return CPU_Rep.f_7_EBX_[3]; }
  static bool HLE(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[4]; }
  static bool AVX2(void) { return CPU_Rep.f_7_EBX_[5]; }
  static bool BMI2(void) { return CPU_Rep.f_7_EBX_[8]; }
  static bool ERMS(void) { return CPU_Rep.f_7_EBX_[9]; }
  static bool INVPCID(void) { return CPU_Rep.f_7_EBX_[10]; }
  static bool RTM(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[11]; }
  static bool AVX512F(void) { return CPU_Rep.f_7_EBX_[16]; }
  static bool RDSEED(void) { return CPU_Rep.f_7_EBX_[18]; }
  static bool ADX(void) { return CPU_Rep.f_7_EBX_[19]; }
  static bool AVX512PF(void) { return CPU_Rep.f_7_EBX_[26]; }
  static bool AVX512ER(void) { return CPU_Rep.f_7_EBX_[27]; }
  static bool AVX512CD(void) { return CPU_Rep.f_7_EBX_[28]; }
  static bool SHA(void) { return CPU_Rep.f_7_EBX_[29]; }

  static bool PREFETCHWT1(void) { return CPU_Rep.f_7_ECX_[0]; }

  static bool LAHF(void) { return CPU_Rep.f_81_ECX_[0]; }
  static bool LZCNT(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_ECX_[5]; }
  static bool ABM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[5]; }
  static bool SSE4a(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[6]; }
  static bool XOP(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[11]; }
  static bool TBM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[21]; }

  static bool SYSCALL(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[11]; }
  static bool MMXEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[22]; }
  static bool RDTSCP(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[27]; }
  static bool _3DNOWEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[30]; }
  static bool _3DNOW(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[31]; }

 private:
  static const InstructionSet_Internal CPU_Rep;

  class InstructionSet_Internal
  {
  public:
   InstructionSet_Internal()
    : nIds_{ 0 },
    nExIds_{ 0 },
    isIntel_{ false },
    isAMD_{ false },
    f_1_ECX_{ 0 },
    f_1_EDX_{ 0 },
    f_7_EBX_{ 0 },
    f_7_ECX_{ 0 },
    f_81_ECX_{ 0 },
    f_81_EDX_{ 0 },
    data_{},
    extdata_{}
   {
    //int cpuInfo[4] = {-1};
    std::array<int, 4> cpui;

    // Calling __cpuid with 0x0 as the function_id argument
    // gets the number of the highest valid function ID.
    __cpuid(cpui.data(), 0);
    nIds_ = cpui[0];

    for (int i = 0; i <= nIds_; ++i)
    {
     __cpuidex(cpui.data(), i, 0);
     data_.push_back(cpui);
    }

    // Capture vendor string
    char vendor[0x20];
    memset(vendor, 0, sizeof(vendor));
    *reinterpret_cast<int*>(vendor) = data_[0][1];
    *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
    *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
    vendor_ = vendor;
    if (vendor_ == "GenuineIntel")
    {
     isIntel_ = true;
    }
    else if (vendor_ == "AuthenticAMD")
    {
     isAMD_ = true;
    }

    // load bitset with flags for function 0x00000001
    if (nIds_ >= 1)
    {
     f_1_ECX_ = data_[1][2];
     f_1_EDX_ = data_[1][3];
    }

    // load bitset with flags for function 0x00000007
    if (nIds_ >= 7)
    {
     f_7_EBX_ = data_[7][1];
     f_7_ECX_ = data_[7][2];
    }

    // Calling __cpuid with 0x80000000 as the function_id argument
    // gets the number of the highest valid extended ID.
    __cpuid(cpui.data(), 0x80000000);
    nExIds_ = cpui[0];

    char brand[0x40];
    memset(brand, 0, sizeof(brand));

    for (int i = 0x80000000; i <= nExIds_; ++i)
    {
     __cpuidex(cpui.data(), i, 0);
     extdata_.push_back(cpui);
    }

    // load bitset with flags for function 0x80000001
    if (nExIds_ >= 0x80000001)
    {
     f_81_ECX_ = extdata_[1][2];
     f_81_EDX_ = extdata_[1][3];
    }

    // Interpret CPU brand string if reported
    if (nExIds_ >= 0x80000004)
    {
     memcpy(brand, extdata_[2].data(), sizeof(cpui));
     memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
     memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
     brand_ = brand;
    }
   };

   int nIds_;
   int nExIds_;
   std::string vendor_;
   std::string brand_;
   bool isIntel_;
   bool isAMD_;
   std::bitset<32> f_1_ECX_;
   std::bitset<32> f_1_EDX_;
   std::bitset<32> f_7_EBX_;
   std::bitset<32> f_7_ECX_;
   std::bitset<32> f_81_ECX_;
   std::bitset<32> f_81_EDX_;
   std::vector<std::array<int, 4>> data_;
   std::vector<std::array<int, 4>> extdata_;
  };
 };

 // Initialize static member data
 const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;

 // Print out supported instruction set extensions
 int main()
 {
  auto& outstream = std::cout;

  auto support_message = [&outstream](std::string isa_feature, bool is_supported) {
   outstream << isa_feature << (is_supported ? " supported" : " not supported") << std::endl;
  };

  std::cout << InstructionSet::Vendor() << std::endl;
  std::cout << InstructionSet::Brand() << std::endl;

  support_message("3DNOW", InstructionSet::_3DNOW());
  support_message("3DNOWEXT", InstructionSet::_3DNOWEXT());
  support_message("ABM", InstructionSet::ABM());
  support_message("ADX", InstructionSet::ADX());
  support_message("AES", InstructionSet::AES());
  support_message("AVX", InstructionSet::AVX());
  support_message("AVX2", InstructionSet::AVX2());
  support_message("AVX512CD", InstructionSet::AVX512CD());
  support_message("AVX512ER", InstructionSet::AVX512ER());
  support_message("AVX512F", InstructionSet::AVX512F());
  support_message("AVX512PF", InstructionSet::AVX512PF());
  support_message("BMI1", InstructionSet::BMI1());
  support_message("BMI2", InstructionSet::BMI2());
  support_message("CLFSH", InstructionSet::CLFSH());
  support_message("CMPXCHG16B", InstructionSet::CMPXCHG16B());
  support_message("CX8", InstructionSet::CX8());
  support_message("ERMS", InstructionSet::ERMS());
  support_message("F16C", InstructionSet::F16C());
  support_message("FMA", InstructionSet::FMA());
  support_message("FSGSBASE", InstructionSet::FSGSBASE());
  support_message("FXSR", InstructionSet::FXSR());
  support_message("HLE", InstructionSet::HLE());
  support_message("INVPCID", InstructionSet::INVPCID());
  support_message("LAHF", InstructionSet::LAHF());
  support_message("LZCNT", InstructionSet::LZCNT());
  support_message("MMX", InstructionSet::MMX());
  support_message("MMXEXT", InstructionSet::MMXEXT());
  support_message("MONITOR", InstructionSet::MONITOR());
  support_message("MOVBE", InstructionSet::MOVBE());
  support_message("MSR", InstructionSet::MSR());
  support_message("OSXSAVE", InstructionSet::OSXSAVE());
  support_message("PCLMULQDQ", InstructionSet::PCLMULQDQ());
  support_message("POPCNT", InstructionSet::POPCNT());
  support_message("PREFETCHWT1", InstructionSet::PREFETCHWT1());
  support_message("RDRAND", InstructionSet::RDRAND());
  support_message("RDSEED", InstructionSet::RDSEED());
  support_message("RDTSCP", InstructionSet::RDTSCP());
  support_message("RTM", InstructionSet::RTM());
  support_message("SEP", InstructionSet::SEP());
  support_message("SHA", InstructionSet::SHA());
  support_message("SSE", InstructionSet::SSE());
  support_message("SSE2", InstructionSet::SSE2());
  support_message("SSE3", InstructionSet::SSE3());
  support_message("SSE4.1", InstructionSet::SSE41());
  support_message("SSE4.2", InstructionSet::SSE42());
  support_message("SSE4a", InstructionSet::SSE4a());
  support_message("SSSE3", InstructionSet::SSSE3());
  support_message("SYSCALL", InstructionSet::SYSCALL());
  support_message("TBM", InstructionSet::TBM());
  support_message("XOP", InstructionSet::XOP());
  support_message("XSAVE", InstructionSet::XSAVE());
 }
#endif
 void Win::CPUView(std::string& cpu_vendor, std::string& cpu_brand) {
  cpu_vendor.clear();
  cpu_brand.clear();
  int nIds_ = 0;
  int nExIds_ = 0;
  std::bitset<32> f_1_ECX_ = {};
  std::bitset<32> f_1_EDX_ = {};
  std::bitset<32> f_7_EBX_ = {};
  std::bitset<32> f_7_ECX_ = {};
  std::bitset<32> f_81_ECX_ = {};
  std::bitset<32> f_81_EDX_ = {};
  std::vector<std::array<int, 4>> data_ = {};
  std::vector<std::array<int, 4>> extdata_ = {};
  //int cpuInfo[4] = {-1};
  std::array<int, 4> cpui = {};
  // Calling __cpuid with 0x0 as the function_id argument
  // gets the number of the highest valid function ID.	__cpuid(cpui.data(), 0);
  nIds_ = cpui[0];
  for (int i = 0; i <= nIds_; ++i) {
   __cpuidex(cpui.data(), i, 0);
   data_.push_back(cpui);
  }
  // Capture vendor string
  char vendor[0x20];
  memset(vendor, 0, sizeof(vendor));
  *reinterpret_cast<int*>(vendor) = data_[0][1];
  *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
  *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
  cpu_vendor = vendor;
  // load bitset with flags for function 0x00000001
  if (nIds_ >= 1) {
   f_1_ECX_ = data_[1][2];
   f_1_EDX_ = data_[1][3];
  }
  // load bitset with flags for function 0x00000007
  if (nIds_ >= 7) {
   f_7_EBX_ = data_[7][1];
   f_7_ECX_ = data_[7][2];
  }
  // Calling __cpuid with 0x80000000 as the function_id argument
  // gets the number of the highest valid extended ID.
  __cpuid(cpui.data(), 0x80000000);
  nExIds_ = cpui[0];
  char brand[0x40];
  memset(brand, 0, sizeof(brand));
  for (int i = 0x80000000; i <= nExIds_; ++i) {
   __cpuidex(cpui.data(), i, 0);
   extdata_.push_back(cpui);
  }
  // load bitset with flags for function 0x80000001
  if (nExIds_ >= 0x80000001) {
   f_81_ECX_ = extdata_[1][2];
   f_81_EDX_ = extdata_[1][3];
  }
  // Interpret CPU brand string if reported
  if (nExIds_ >= 0x80000004) {
   memcpy(brand, extdata_[2].data(), sizeof(cpui));
   memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
   memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
   cpu_brand = brand;
  }
#if 0
  if (!cpu_vendor.empty()) {
   for (auto it = cpu_vendor.begin(); it != cpu_vendor.end();) {
    if (*it == ' ')
     it = cpu_vendor.erase(it);
    else
     break;
   }
  }

  if (!cpu_brand.empty()) {
   for (auto it = cpu_brand.begin(); it != cpu_brand.end();) {
    if (*it == ' ')
     it = cpu_brand.erase(it);
    else
     break;
   }
  }
#endif
 }
 bool Win::RemoveStrlW(const std::wstring& src, const std::wstring& dest, std::wstring& out) {
  bool result = false;
  out.clear();
  do {
   if (src.empty() || dest.empty())
    break;
   auto found = ::StrStrIW(src.c_str(), dest.c_str());
   size_t found_pos = found - src.c_str();
   size_t found_size = dest.size();
   out = src;
   out.erase(found_pos, found_size);
   result = true;
  } while (0);
  return result;
 }
 bool Win::RemoveStrlA(const std::string& src, const std::string& dest, std::string& out) {
  bool result = false;
  out.clear();
  do {
   if (src.empty() || dest.empty())
    break;
   auto found = ::StrStrIA(src.c_str(), dest.c_str());
   size_t found_pos = found - src.c_str();
   size_t found_size = dest.size();
   out = src;
   out.erase(found_pos, found_size);
   result = true;
  } while (0);
  return result;
 }
 void Win::CommandLineArgs(std::map<std::string, std::string>& out_param_map) {
  out_param_map.clear();
  for (int i = 0; i < __argc; ++i) {
   std::string the = __argv[i];
   do {
    auto k = ::StrStrIA(the.c_str(), "--");
    auto v = ::StrStrIA(the.c_str(), "=");
    if (!k || !v)
     continue;
    auto key = shared::IConv::ToLowerA(std::string(k, v - k));
    auto value = std::string(v + ::strlen("="));
    if (key.empty() || value.empty())
     continue;
    out_param_map.emplace(shared::IConv::ToLowerA(std::string(k, v - k)), std::string(v + ::strlen("=")));
   } while (0);
  }
 }

#if 0//!@ by --- https://stackoverflow.com/questions/47315191/how-to-check-group-membership-for-a-process-that-is-not-self
 void check_group_membership(HANDLE hToken, LPCTSTR pStrName, DWORD nSubauthority, const DWORD& onPid = 0)
 {
  BOOL bIsMember;
  SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
  wprintf(L"Local %s: ", pStrName);
  PSID pSID = NULL;
  if (AllocateAndInitializeSid(&NtAuthority, 2,
   SECURITY_BUILTIN_DOMAIN_RID,
   nSubauthority,
   0, 0, 0, 0, 0, 0, &pSID))
  {
   bIsMember = FALSE;
   if (CheckTokenMembership(hToken, pSID, &bIsMember))
   {
    wprintf(L"%s on pid(%ld).", bIsMember ? L"Yes" : L"No", onPid);
   }
   else
   {
    wprintf(L"<CheckErr: (%d)>", GetLastError());
   }
  }
  else
  {
   wprintf(L"<SidErr: (%d)>", GetLastError());
  }
  wprintf(L"\n");
  if (pSID) {
   FreeSid(pSID);
   pSID = NULL;
  }
 }

 void check_membership(const DWORD& pid)
 {
  HANDLE hToken = NULL;
  HANDLE hProcess = NULL;
  HANDLE hImpToken = NULL;
  do {
   hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
   if (!hProcess)
    break;
   ::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
   if (!::DuplicateTokenEx(hToken, TOKEN_QUERY, NULL, SecurityImpersonation, TokenImpersonation, &hImpToken))
    break;
   check_group_membership(hImpToken, L"Local Group", DOMAIN_ALIAS_RID_USERS, pid);
   check_group_membership(hImpToken, L"Admin Group", DOMAIN_ALIAS_RID_ADMINS, pid);
   check_group_membership(hImpToken, L"Domain Admin Group", DOMAIN_GROUP_RID_ADMINS, pid);
   check_group_membership(hImpToken, L"Schema Admin Group", DOMAIN_GROUP_RID_SCHEMA_ADMINS, pid);
   check_group_membership(hImpToken, L"Enterprise Admin Group", DOMAIN_GROUP_RID_ENTERPRISE_ADMINS, pid);
  } while (0);
  SK_CLOSE_HANDLE(hToken);
  SK_CLOSE_HANDLE(hImpToken);
  SK_CLOSE_HANDLE(hProcess);
 }
#endif
 bool Win::IsProcessHighIntegrity(const DWORD& pid) {
  bool result = false;
  PSID pSID = NULL;
  HANDLE hProcess = NULL;
  HANDLE hToken = NULL;
  HANDLE hTokenCopy = NULL;
  do {
   hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
   if (!hProcess)
    break;
   ::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
   if (!::DuplicateTokenEx(hToken, TOKEN_QUERY, NULL, SecurityImpersonation, TokenImpersonation, &hTokenCopy))
    break;
   if (!hTokenCopy)
    break;
   SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
   if (!::AllocateAndInitializeSid(&NtAuthority, 2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0, &pSID))
    break;
   BOOL bIsMember = FALSE;
   if (FALSE == ::CheckTokenMembership(hTokenCopy, pSID, &bIsMember))
    break;
   if (FALSE == bIsMember)
    break;
   result = true;
  } while (0);

  if (pSID) {
   ::FreeSid(pSID);
   pSID = NULL;
  }
  SK_CLOSE_HANDLE(hToken);
  SK_CLOSE_HANDLE(hProcess);
  SK_CLOSE_HANDLE(hTokenCopy);
  return result;
 }
 bool Win::ParseCommandLineParameters(const std::string& commandline, tfCommandLineNode& out) {
  bool result = false;
  out.clear();
  do {
   if (commandline.empty())
    break;
   std::vector<std::string> argvs;
   File::ParseA(commandline, ' ', argvs);
   if (argvs.empty())
    break;
   for (auto& argv : argvs) {
    auto separator = ::StrStrIA(argv.c_str(), "=");
    auto node_begin = ::StrStrIA(argv.c_str(), "--");
    if (!separator || !node_begin)
     continue;
    std::vector<std::string> parses;
    shared::Win::File::ParseA(node_begin, '=', parses);
    if (parses.size() != 2)
     continue;
    out.emplace(IConv::ToLowerA(parses[0]), parses[1]);
   }
   result = !out.empty();
  } while (0);
  return result;
 }
 bool Win::ParseCommandLineParameters(const int& argc, char** argv, tfCommandLineNode& out) {
  bool result = false;
  out.clear();
  do {
   if (argc <= 0 || !argv)
    break;
   for (int i = 0; i < argc; ++i) {
    auto separator = ::StrStrIA(argv[i], "=");
    auto node_begin = ::StrStrIA(argv[i], "--");
    if (!separator || !node_begin)
     continue;
    std::vector<std::string> parses;
    shared::Win::File::ParseA(node_begin, '=', parses);
    if (parses.size() != 2)
     continue;
    out.emplace(IConv::ToLowerA(parses[0]), parses[1]);
   }
   result = !out.empty();
  } while (0);
  return result;
 }
 /*
 * {"Identify":"0\\objects.bin","DiskFileSize":14092,"MD5":"6cf2ff9af76750bca7e38145e201e98a","Version":"0.0.0.0"}
 [{"Identify":"0\\objects.bin","DiskFileSize":14092,"MD5":"6cf2ff9af76750bca7e38145e201e98a","Version":"0.0.0.0"},{"Identify":"0\\objects\\500001.bin","DiskFileSize":827239,"MD5":"08049fef66a11f95ee682ec8642215c1","Version":"0.0.0.0"},{"Identify":"0\\objects\\50000A.bin","DiskFileSize":1227222,"MD5":"ba2de1dec163550403baeaf60a679da3","Version":"0.0.0.0"},{"Identify":"0\\objects\\50000B.bin","DiskFileSize":741491,"MD5":"bc7f62a02af24ed6ed546246de5ac63b","Version":"0.0.0.0"},{"Identify":"0\\objects\\A00001.bin","DiskFileSize":4743287,"MD5":"7038bfb185210025a7b69a97aec760f1","Version":"0.0.0.0"},{"Identify":"main.bin","DiskFileSize":1049,"MD5":"12c7acc2d83a46d0f8807f2d1cb0b94d","Version":"0.0.0.0"}]
 */
 bool FileUpdateObj::operator<<(const std::string& jsonObjNode) {
  bool result = false;
  do {
   if (jsonObjNode.empty())
    break;
   rapidjson::Document doc;
   if (doc.Parse(jsonObjNode.data(), jsonObjNode.size()).HasParseError())
    break;
   if (!doc.IsObject())
    break;
   if (doc.ObjectEmpty())
    break;
   if (!doc.HasMember("Identify") || !doc["Identify"].IsString() || doc["Identify"].GetStringLength() <= 0)
    break;
   Identify = doc["Identify"].GetString();
   if (doc.HasMember("DiskFileSize") && doc["DiskFileSize"].IsNumber())
    DiskFileSize = doc["DiskFileSize"].GetUint();
   if (doc.HasMember("MD5") && doc["MD5"].IsString())
    MD5 = doc["MD5"].GetString();
   if (doc.HasMember("Version") && doc["Version"].IsString())
    Version = doc["Version"].GetString();
   if (doc.HasMember("OriginalFileName") && doc["OriginalFileName"].IsString())
    OriginalFileName = doc["OriginalFileName"].GetString();
   if (doc.HasMember("OriginalFileFormat") && doc["OriginalFileFormat"].IsString())
    OriginalFileFormat = doc["OriginalFileFormat"].GetString();

   result = true;
  } while (0);
  return result;
 }
 void FileUpdateObj::operator>>(std::string& jsonObjectString) const {
  jsonObjectString.clear();
  rapidjson::Document doc{ rapidjson::Type::kObjectType };
  doc.AddMember(\
   rapidjson::Value().SetString("Identify", doc.GetAllocator()).Move(),
   rapidjson::Value().SetString(Identify.empty() ? "" : Identify.c_str(), doc.GetAllocator()).Move(),
   doc.GetAllocator());
  doc.AddMember(\
   rapidjson::Value().SetString("DiskFileSize", doc.GetAllocator()).Move(),
   rapidjson::Value().SetUint(DiskFileSize).Move(),
   doc.GetAllocator());
  doc.AddMember(\
   rapidjson::Value().SetString("MD5", doc.GetAllocator()).Move(),
   rapidjson::Value().SetString(MD5.empty() ? "" : MD5.c_str(), doc.GetAllocator()).Move(),
   doc.GetAllocator());
  doc.AddMember(\
   rapidjson::Value().SetString("Version", doc.GetAllocator()).Move(),
   rapidjson::Value().SetString(Version.empty() ? "" : Version.c_str(), doc.GetAllocator()).Move(),
   doc.GetAllocator());
  doc.AddMember(\
   rapidjson::Value().SetString("OriginalFileFormat", doc.GetAllocator()).Move(),
   rapidjson::Value().SetString(OriginalFileFormat.empty() ? "" : OriginalFileFormat.c_str(), doc.GetAllocator()).Move(),
   doc.GetAllocator());
  doc.AddMember(\
   rapidjson::Value().SetString("OriginalFileName", doc.GetAllocator()).Move(),
   rapidjson::Value().SetString(OriginalFileName.empty() ? "" : OriginalFileName.c_str(), doc.GetAllocator()).Move(),
   doc.GetAllocator());
  jsonObjectString = shared::Json::toString(doc);
 }
 bool FileUpdateObj::Verify() const {
  bool result = false;
  do {
   if (Identify.empty())
    break;
   if (MD5.empty())
    break;
   if (DiskFileSize <= 0)
    break;
   result = true;
  } while (0);
  return result;
 }
 void FileUpdateObj::operator=(const FileUpdateObj& obj) {
  Identify = obj.Identify;
  DiskFileSize = obj.DiskFileSize;
  MD5 = obj.MD5;
  Version = obj.Version;
  FileBuffer = obj.FileBuffer;
  FilePathname = obj.FilePathname;
  MD5Base64 = obj.MD5Base64;
  ResultFlag = obj.ResultFlag;
  OriginalFileFormat = obj.OriginalFileFormat;
  OriginalFileName = obj.OriginalFileName;
 }
 bool FileUpdateObj::operator<(const FileUpdateObj& obj) const {
  if (DiskFileSize != obj.DiskFileSize)
   return DiskFileSize < obj.DiskFileSize;
  return Identify.compare(obj.Identify) < 0;
 }
 bool FileUpdateObj::operator>(const FileUpdateObj& obj) const {
  if (DiskFileSize != obj.DiskFileSize)
   return DiskFileSize > obj.DiskFileSize;
  return Identify.compare(obj.Identify) > 0;
 }
 bool FileUpdateObj::operator==(const FileUpdateObj& obj) const {
  bool result = false;
  do {
   if (Identify.compare(obj.Identify))
    break;
   if (DiskFileSize != obj.DiskFileSize)
    break;
   if (::_stricmp(MD5.c_str(), obj.MD5.c_str()))
    break;
   if (::_stricmp(MD5Base64.c_str(), obj.MD5Base64.c_str()))
    break;
   if (::_stricmp(OriginalFileFormat.c_str(), obj.OriginalFileFormat.c_str()))
    break;
   if (::_stricmp(OriginalFileName.c_str(), obj.OriginalFileName.c_str()))
    break;
   if (!Version.empty()) {
    if (Version.compare(obj.Version))
     break;
   }
   result = true;
  } while (0);
  return result;
 }
 bool FileUpdateObj::operator!=(const FileUpdateObj& obj) const {
  return !(*this == obj);
 }
 bool Win::GetAssignUserSid(const std::string& user_name, std::string& outSid) {
  bool result = false;
  outSid.clear();
  char UserSid[_MAX_PATH] = { 0 };
  DWORD dwUserSid = _MAX_PATH;
  char UserDomain[_MAX_PATH] = { 0 };
  DWORD dwUserDomain = _MAX_PATH;
  do {
   if (user_name.empty())
    break;
   SID_NAME_USE SidNameUser;
   if (FALSE == ::LookupAccountNameA(nullptr, user_name.c_str(), UserSid, &dwUserSid, UserDomain, &dwUserDomain, &SidNameUser))
    break;
   PSID_IDENTIFIER_AUTHORITY pSID_IDENTIFIER_AUTHORITY = ::GetSidIdentifierAuthority(UserSid);
   if (!pSID_IDENTIFIER_AUTHORITY)
    break;
   outSid = std::format("S-{}-{}", SID_REVISION, pSID_IDENTIFIER_AUTHORITY->Value[5]);
   auto pGetSidSubAuthorityCount = ::GetSidSubAuthorityCount(UserSid);
   if (!pGetSidSubAuthorityCount)
    break;
   const int SidSubAuthorityCount = *pGetSidSubAuthorityCount;
   if (SidSubAuthorityCount <= 0)
    break;
   for (int i = 0; i < SidSubAuthorityCount; ++i) {
    auto pGetSidSubAuthority = ::GetSidSubAuthority(UserSid, i);
    if (!pGetSidSubAuthority)
     break;
    outSid.append("-").append(std::format("{}", *pGetSidSubAuthority));
   }
   result = !outSid.empty();
  } while (0);
  return result;
 }
 std::string Win::GetCurrentUserSid() {
  std::string result;
  GetCurrentUserSid(result);
  return result;
 }
 bool Win::GetCurrentUserSid(std::string& outSid) {
  bool result = false;
  outSid.clear();
  char UserName[_MAX_PATH] = { 0 };
  DWORD dwUserName = _MAX_PATH;
  char UserSid[_MAX_PATH] = { 0 };
  DWORD dwUserSid = _MAX_PATH;
  char UserDomain[_MAX_PATH] = { 0 };
  DWORD dwUserDomain = _MAX_PATH;
  do {
   if (FALSE == ::GetUserNameA(UserName, &dwUserName))
    break;
   SID_NAME_USE SidNameUser;
   if (FALSE == ::LookupAccountNameA(nullptr, UserName, UserSid, &dwUserSid, UserDomain, &dwUserDomain, &SidNameUser))
    break;
   PSID_IDENTIFIER_AUTHORITY pSID_IDENTIFIER_AUTHORITY = ::GetSidIdentifierAuthority(UserSid);
   if (!pSID_IDENTIFIER_AUTHORITY)
    break;
   outSid = std::format("S-{}-{}", SID_REVISION, pSID_IDENTIFIER_AUTHORITY->Value[5]);
   auto pGetSidSubAuthorityCount = ::GetSidSubAuthorityCount(UserSid);
   if (!pGetSidSubAuthorityCount)
    break;
   const int SidSubAuthorityCount = *pGetSidSubAuthorityCount;
   if (SidSubAuthorityCount <= 0)
    break;
   for (int i = 0; i < SidSubAuthorityCount; ++i) {
    auto pGetSidSubAuthority = ::GetSidSubAuthority(UserSid, i);
    if (!pGetSidSubAuthority)
     break;
    outSid.append("-").append(std::format("{}", *pGetSidSubAuthority));
   }
   result = !outSid.empty();
  } while (0);
  return result;
 }


 bool Win::BrowseForFolder(const HWND& hOwner, const std::string& title, std::string& sel_path) {
  sel_path.clear();
  char selPath[MAX_PATH] = { 0 };
  BROWSEINFOA bi;
  bi.hwndOwner = hOwner;
  bi.pidlRoot = nullptr;
  bi.pszDisplayName = nullptr;
  bi.lpszTitle = title.c_str();
  bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
  bi.lpfn = \
   [](HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)->int {
   switch (uMsg)
   {
   case BFFM_INITIALIZED:  //选择文件夹对话框初始化  
    //设置默认路径为lpData即'D:\'   
    ::SendMessage(hwnd, BFFM_SETSELECTIONA, TRUE, lpData);
    //在STATUSTEXT区域显示当前路径   
    ::SendMessage(hwnd, BFFM_SETSTATUSTEXTA, 0, lpData);
    //设置选择文件夹对话框的标题   
    //::SetWindowTextA(hwnd, "请先设置个工作目录");
    break;
   case BFFM_SELCHANGED:   //选择文件夹变更时  
   {
    char pszPath[_MAX_PATH] = { 0 };
    //获取当前选择路径   
    ::SHGetPathFromIDListA((LPCITEMIDLIST)lParam, pszPath);
    //在STATUSTEXT区域显示当前路径  
    //::SendMessage(hwnd, BFFM_SETSTATUSTEXTA, TRUE, (LPARAM)pszPath);
   }
   break;
   }
   return 0;
  };
  bi.lParam = (LPARAM)"C:\\Program Files (x86)";
  bi.iImage = 0;

  LPITEMIDLIST pidl = ::SHBrowseForFolderA(&bi);
  if (!pidl)
   return false;
  if (!::SHGetPathFromIDListA(pidl, selPath) || !selPath[0])
   return false;
  sel_path.append(selPath, __min(sizeof(selPath), ::strlen(selPath)));
  return true;
 }
 DWORD Win::GetCurrentThreadId() {
  DWORD result = 0;
  do {
   std::thread::id tid = std::this_thread::get_id();
   std::stringstream in;
   in << tid;
   result = ::atol(in.str().c_str());
  } while (0);
  return result;
 }
 std::string Win::DownVolToString(const double& volume) {
  return RealtimeSpeed(static_cast<long long>(volume), true);
 }
 std::string Win::DownVolToString(const long long& volume) {
  return RealtimeSpeed(static_cast<long long>(volume), true);
 }
 std::string Win::DownVolToString(const size_t& volume) {
  return RealtimeSpeed(static_cast<long long>(volume), true);
 }
 std::string Win::RealtimeSpeed(const long long& speed_bype_size, const bool& divide /*= true*/) {
  std::string result = divide ? "0.0 B" : "0.0B";
  do {
   if (speed_bype_size <= 0)
    break;
   result.clear();
   result.resize(_MAX_PATH, 0x00);
   int nSprintf = 0;
   if (speed_bype_size >= 1024 * 1024 * 1024) {//!@ GB
    nSprintf = ::sprintf_s(&result[0], _MAX_PATH, "%.2f", (speed_bype_size * 1.0) / (1024.0 * 1024.0 * 1024.0));
    result.resize(nSprintf);
    result.append(divide ? " GB" : "GB");
   }
   else if (speed_bype_size >= 1024 * 1024) {//!@ MB
    nSprintf = ::sprintf_s(&result[0], _MAX_PATH, "%.2f", (speed_bype_size * 1.0) / (1024.0 * 1024.0));
    result.resize(nSprintf);
    result.append(divide ? " MB" : "MB");
   }
   else if (speed_bype_size >= 1024) {//!@ KB
    nSprintf = ::sprintf_s(&result[0], _MAX_PATH, "%.2f", (speed_bype_size * 1.0) / 1024.0);
    result.resize(nSprintf);
    result.append(divide ? " KB" : "KB");
   }
   else {//!@ B
    nSprintf = ::sprintf_s(&result[0], _MAX_PATH, "%.2f", __max(0.0, (speed_bype_size * 1.0) / 1.0));
    result.resize(nSprintf);
    result.append(divide ? " B" : "B");
   }
  } while (0);
  return result;
 }
 //!@移动路径后缀"// or \\"
 static std::string RemovePathSuffix(const std::string& path) {
  std::string result{ path };
  for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
   if (*rit == '\\' || *rit == '/') {
    result.resize(std::distance(result.begin(), rit.base()) - 1);
    continue;
   }
   break;
  }
  return result;
 }
 bool Win::OpenTargetPathnameLockedFile(const std::string& targetPathname) {
  bool result = false;
  do {
   if (targetPathname.empty())
    break;
   std::string parameters{ "/select," };
   parameters.append(targetPathname);
   result = nullptr != ::ShellExecuteA(NULL, "open", "explorer.exe", parameters.c_str(),
    RemovePathSuffix(GetPathByPathnameA(targetPathname)).c_str(),
    SW_SHOWNORMAL);
  } while (0);
  return result;
 }
 std::string Win::UrlFixedByDowntool(const std::string& utf8_url) {
  static const std::string urlstd = R"(abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.?=&%:/\)";
  std::string result;
  do {
   if (utf8_url.empty())
    break;
   std::vector<std::string> src1;
   std::for_each(utf8_url.begin(), utf8_url.end(),
    [&](const auto& ch) {
     src1.emplace_back(std::string(1, ch));
    });
   if (src1.empty())
    break;
   std::string src2{ shared::IConv::ToUpperA(shared::Win::BinToHex(utf8_url)) };
   if (src2.empty())
    break;
   std::vector<std::string> res;
   for (size_t i = 0; i < src2.size(); i += 2) {
    res.emplace_back(std::string("%").append(1, src2[i]).append(1, src2[i + 1]));
   }
   if (res.empty())
    break;
   for (size_t i = 0; i < src1.size(); ++i) {
    if (urlstd.find(src1[i]) == std::string::npos) {
     src1[i] = res[i];
    }
   }
   std::for_each(src1.begin(), src1.end(),
    [&](const auto& node) {
     result.append(node);
    });
  } while (0);
  return result;
 }

 enum WindowsOS {
  NotFind,
  Win2000,
  WinXP,
  WinVista,
  Win7,
  Win8,
  Win10
 };
 static WindowsOS GetOsVersionQuick() {
  double ret = 0.0;
  NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
  OSVERSIONINFOEXW osInfo;
  *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");
  if (NULL != RtlGetVersion) {
   osInfo.dwOSVersionInfoSize = sizeof(osInfo);
   RtlGetVersion(&osInfo);
   ret = (double)osInfo.dwMajorVersion;
  }
  if (osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0) {
   return Win10;//!@10
  }
  else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 3) {
   return Win8;//!@8.1
  }
  else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 2) {
   return Win8;//!@8
  }
  else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 1) {
   return Win7;//!@windows 7 or Windows Server 2008 R2
  }
  return NotFind;
 }
 namespace taskbarpinwin10 {
#pragma pack(push,1)
  enum PLMC
  {
   PLMC_EXPLORER = 4
  };
  struct tagIPinnedList3Vtbl;
  typedef struct tagIPinnedList3 {
   tagIPinnedList3Vtbl* vtbl;
   tagIPinnedList3() {
    ::memset(this, 0x00, sizeof(*this));
   }
  }IPinnedList3;
  using tfReleaseFun = unsigned long(__stdcall*) (IPinnedList3*);
  using tfModifyFun = long(__stdcall*) (IPinnedList3*, LPCITEMIDLIST, LPCITEMIDLIST, PLMC);
  typedef struct tagIPinnedList3Vtbl {
   void* QueryInterface;
   void* AddRef;
   tfReleaseFun Release;
   void* MethodSlot4;
   void* MethodSlot5;
   void* MethodSlot6;
   void* MethodSlot7;
   void* MethodSlot8;
   void* MethodSlot9;
   void* MethodSlot10;
   void* MethodSlot11;
   void* MethodSlot12;
   void* MethodSlot13;
   void* MethodSlot14;
   void* MethodSlot15;
   void* MethodSlot16;
   tfModifyFun Modify;
   tagIPinnedList3Vtbl() { ::memset(this, 0x00, sizeof(*this)); }
  }IPinnedList3Vtbl;
  const GUID iid_ipinnedlist3 = { 0x0dd79ae2, 0xd156, 0x45d4, {0x9e, 0xeb, 0x3b, 0x54, 0x97, 0x69, 0xe9, 0x40} };
  const GUID clsid_taskbandpin = { 0x90aa3a4e, 0x1cba, 0x4233, {0xb8, 0xbb, 0x53, 0x57, 0x73, 0xd4, 0x84, 0x49} };
#pragma pack(pop)
 }///taskbarpinwin10
 bool Win::taskbarpin(const std::string exe_or_lnk_pathname, const bool& pin /*= true*/, const std::string& lnk_pathname /*= ""*/) {
  if (exe_or_lnk_pathname.empty())
   return false;
  bool result = true;
  switch (GetOsVersionQuick()) {
  case WindowsOS::Win10: {
   {
    HRESULT comret = S_FALSE;
    bool result = false;
    LPITEMIDLIST pItemidList = nullptr;
    taskbarpinwin10::IPinnedList3* pinnedList = nullptr;
    do {
     pItemidList = ::ILCreateFromPathA(exe_or_lnk_pathname.c_str());
     if (!pItemidList)
      break;
     comret = ::CoCreateInstance(taskbarpinwin10::clsid_taskbandpin, NULL, CLSCTX_ALL,
      taskbarpinwin10::iid_ipinnedlist3, (LPVOID*)(&pinnedList));
     if (comret != S_OK || !pinnedList)
      break;
     comret = pinnedList->vtbl->Modify(pinnedList, pin ? NULL : pItemidList, pin ?
      pItemidList : NULL, taskbarpinwin10::PLMC::PLMC_EXPLORER);
     if (comret != S_OK)
      break;
     result = true;
    } while (0);
    if (pinnedList) {
     pinnedList->vtbl->Release(pinnedList);
     pinnedList = nullptr;
    }
    if (pItemidList) {
     ::ILFree(pItemidList);
     pItemidList = nullptr;
    }
   }

   std::wstring setup_name, setup_path;
   setup_name = shared::IConv::MBytesToWString(shared::Win::GetNameByPathnameA(exe_or_lnk_pathname));
   setup_path = shared::IConv::MBytesToWString(shared::Win::PathFixedA(shared::Win::GetPathByPathnameA(exe_or_lnk_pathname)));
   if (setup_path.empty() || setup_name.empty())
    return false;
   bool exists = false;
   const std::string TaskBarPath = shared::Win::GetSpecialFolderLocationA(CSIDL_APPDATA) + \
    R"(\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\)";
   std::map<std::string, std::string> enum_files{}, enum_dirs{};
   shared::Win::EnumFoldersAndFiles(TaskBarPath, enum_dirs, enum_files, "*.lnk", false,
    [&](const auto& path, const auto& name, const auto& data) {
     std::string find_name = shared::IConv::WStringToMBytes(setup_name);
   auto rfdot1 = find_name.rfind(".");
   if (rfdot1 != std::string::npos)
    find_name[rfdot1] = 0;
   auto lnkpath = shared::Win::GetLnkFormPathW(shared::IConv::MBytesToWString(TaskBarPath + name));
#ifdef _UNICODE
   std::string find_exepath = shared::Win::GetNameByPathnameA(shared::IConv::WStringToMBytes(lnkpath));
#else
   std::string find_exepath = shared::Windows::GetNameByPathname(lnkpath);
#endif
   auto rfdot2 = find_exepath.rfind(".");
   if (rfdot2 != std::string::npos)
    find_exepath[rfdot2] = 0;
   if (!find_name.compare(find_exepath))
    exists = true;
    });

   if (pin) {
    if (!exists)
     result = false;
   }
   else {
    if (exists)
     result = false;
   }

   if (!result) {
    if (pin) {
     if (exists)
      return true;
    }
    else {
     if (!exists)
      return true;
    }
    HRESULT comret = S_FALSE;
    IShellDispatch* pShellDisp = nullptr;
    Folder* pFolder = nullptr;
    FolderItem* pFolderItem = nullptr;
    do {
     TCHAR  szVal[MAX_PATH] = { 0 };
     ULONG  uValSize = MAX_PATH;
     CRegKey  regKey;
     LSTATUS regResult = regKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CommandStore\\shell\\Windows.taskbarpin"), KEY_READ);
     if (ERROR_SUCCESS != regResult)
      break;
     regResult = regKey.QueryStringValue(_T("ExplorerCommandHandler"), szVal, &uValSize);
     if (ERROR_SUCCESS != regResult) {
      regKey.Close();
      break;
     }
     regResult = regKey.Create(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\*\\shell\\{:}"));
     if (ERROR_SUCCESS != regResult) {
      regKey.Close();
      break;
     }
     regResult = regKey.SetStringValue(_T("ExplorerCommandHandler"), szVal);
     regKey.Close();
     if (ERROR_SUCCESS != regResult) {
      break;
     }
     comret = ::CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IShellDispatch, (LPVOID*)&pShellDisp);
     if (!SUCCEEDED(comret))
      break;
     comret = pShellDisp->NameSpace(CComVariant(setup_path.c_str()), &pFolder);
     if (!SUCCEEDED(comret))
      break;
     comret = pFolder->ParseName(CComBSTR(setup_name.c_str()), &pFolderItem);
     if (!SUCCEEDED(comret))
      break;
     comret = pFolderItem->InvokeVerb(CComVariant(_T("{:}")));
     if (!SUCCEEDED(comret))
      break;
     regResult = regKey.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\*\\shell"), KEY_READ | KEY_WRITE);
     if (ERROR_SUCCESS != regResult)
      break;
     regResult = regKey.RecurseDeleteKey(_T("{:}"));
     regKey.Close();
     if (ERROR_SUCCESS != regResult)
      break;
     result = true;
    } while (0);
    SK_COM_RELEASE(pFolderItem);
    SK_COM_RELEASE(pFolder);
    SK_COM_RELEASE(pShellDisp);
   }
  }break;
  default: {
   if (lnk_pathname.empty())
    break;
   do {
    if (pin)
     ShellExecuteA(NULL, "taskbarpin", shared::Win::PathFixedA(lnk_pathname).c_str(), NULL, NULL, SW_HIDE);
    else
     ShellExecuteA(NULL, "taskbarunpin", shared::Win::PathFixedA(lnk_pathname).c_str(), NULL, NULL, SW_HIDE);
   } while (0);
   result = true;
  }break;
  }
  return result;
 }
 std::wstring Win::GetLnkFormPathW(const std::wstring& lnkPath) {
  std::wstring result;
  HRESULT hr = S_FALSE;
  IPersistFile* pIPersistFile = NULL;
  IShellLink* pIShellLink = NULL;
  do {
   hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IPersistFile, (void**)(&pIPersistFile));
   if (hr != S_OK)
    break;
   hr = pIPersistFile->QueryInterface(IID_IShellLink, (void**)(&pIShellLink));
   if (hr != S_OK)
    break;
   hr = pIPersistFile->Load(lnkPath.c_str(), STGM_READ);
   if (hr != S_OK)
    break;
   WIN32_FIND_DATA wfd = { 0 };
   TCHAR ShellPath[_MAX_PATH] = { 0 };
   hr = pIShellLink->GetPath(ShellPath, _MAX_PATH, NULL, NULL);/*(WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);*/
   if (hr != S_OK)
    break;
   TCHAR wWorkingDirectory[_MAX_PATH] = { 0 };
   hr = pIShellLink->GetWorkingDirectory(wWorkingDirectory, _MAX_PATH);
   if (hr != S_OK)
    break;
   if (wWorkingDirectory[0] == 0)
    result = ShellPath;
   else {
    result = wWorkingDirectory;
    if (*std::prev(result.end()) != '\\' && *std::prev(result.end()) != '/')
     result.push_back('\\');
   }
  } while (0);
  SK_COM_RELEASE(pIShellLink);
  SK_COM_RELEASE(pIPersistFile);
  return result;
 }
 bool Win::CreateShortcutUrl(
  const std::string& BindLnkUrl,
  const std::string& BindLnkPathname, /*xxx.lnk*/
  const std::string& BindLnkIcoPathname/*xxx.ico | xxx.png*/) {
  if (BindLnkUrl.empty() || BindLnkIcoPathname.empty() || BindLnkPathname.empty())
   return false;
  HRESULT		comret = S_FALSE;
  bool result = false;
  IShellLinkW* pIShellLinkW = nullptr;
  IPersistFile* pIPersistFile = nullptr;
  do {
   comret = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pIShellLinkW);
   if (S_OK != comret)
    break;
   comret = pIShellLinkW->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
   if (S_OK != comret)
    break;
   comret = pIShellLinkW->SetPath(IConv::MBytesToWString(BindLnkUrl).c_str());
   if (S_OK != comret)
    break;
   comret = pIShellLinkW->SetIconLocation(IConv::MBytesToWString(PathFixedA(BindLnkIcoPathname)).c_str(), 0);
   if (S_OK != comret)
    break;
   comret = pIPersistFile->Save(IConv::MBytesToWString(PathFixedA(BindLnkPathname)).c_str(), TRUE);
   if (S_OK != comret)
    break;
   result = true;
  } while (0);
  SK_COM_RELEASE(pIPersistFile);
  SK_COM_RELEASE(pIShellLinkW);
  return result;
 }
 bool Win::CreateShortcut(const std::string& szPath, const std::string& szLink, const std::string& szIcoPath /*= ""*/, const std::string& szArgument /*= ""*/) {
  HRESULT		comret = S_FALSE;
  IShellLinkW* pIShellLinkW = nullptr;
  IPersistFile* pIPersistFile = nullptr;
  bool result = false;
  do {
   comret = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pIShellLinkW);
   if (comret != S_OK)
    break;
   comret = pIShellLinkW->SetPath(shared::IConv::MBytesToWString(szPath).c_str());
   if (comret != S_OK)
    break;
   comret = pIShellLinkW->SetWorkingDirectory(shared::IConv::MBytesToWString(PathFixedA(GetPathByPathnameA(szPath))).c_str());
   if (comret != S_OK)
    break;
   if (!szIcoPath.empty())
    comret = pIShellLinkW->SetIconLocation(shared::IConv::MBytesToWString(szIcoPath).c_str(), 0);
   if (comret != S_OK)
    break;
   if (!szArgument.empty())
    comret = pIShellLinkW->SetArguments(shared::IConv::MBytesToWString(szArgument).c_str());
   if (comret != S_OK)
    break;
   comret = pIShellLinkW->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
   if (comret != S_OK)
    break;
   comret = pIPersistFile->Save(shared::IConv::MBytesToWString(szLink).c_str(), TRUE);
   if (comret != S_OK)
    break;
   /*CopyFile(tempPath, szLink, FALSE);*/
   result = true;
  } while (0);
  SK_COM_RELEASE(pIPersistFile);
  SK_COM_RELEASE(pIShellLinkW);
  return result;
 }
 bool Win::GdiplusGetEncoderClsid(const std::wstring& MimeTypeFormat, CLSID& outClsid) {
  bool result = false;
#if !defined(WINLIB_DISABLE_GDIPLUS)
  outClsid = CLSID();
  Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;
  do {
   UINT  num = 0;
   UINT  size = 0;
   if (Gdiplus::Status::Ok != Gdiplus::GetImageEncodersSize(&num, &size) || size == 0)
    break;
   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)::malloc(size);
   if (!pImageCodecInfo)
    break;
   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
   for (UINT j = 0; j < num; ++j)
    if (!::_wcsicmp(pImageCodecInfo[j].MimeType, MimeTypeFormat.c_str())) {
     outClsid = pImageCodecInfo[j].Clsid;
     break;
    }
   result = true;
  } while (0);
  SK_DELETE_PTR_C(pImageCodecInfo);
#endif
  return result;
 }
 bool Win::GdiplusCaptureScreen(std::string& out_image_buffer, const std::wstring& MimeTypeFormat /*= LR"(image/png)"*/) {
  bool result = false;
#if !defined(WINLIB_DISABLE_GDIPLUS)
  out_image_buffer.clear();
  IStream* pIStream = nullptr;
  HWND hDesktopWnd = nullptr;
  HDC hDesktopDC = nullptr;
  HDC hCaptureDC = nullptr;
  HBITMAP hCaptureBitmap = nullptr;
  Gdiplus::Bitmap* pbmSrc = nullptr;
  do {
   SIZE sizeWindow;
   shared::Win::FullScreenSize(sizeWindow);
   hDesktopWnd = ::GetDesktopWindow();
   hDesktopDC = ::GetDC(hDesktopWnd);
   hCaptureDC = ::CreateCompatibleDC(hDesktopDC);
   hCaptureBitmap = ::CreateCompatibleBitmap(hDesktopDC, sizeWindow.cx, sizeWindow.cy);
   ::SelectObject(hCaptureDC, hCaptureBitmap);
   if (FALSE == ::BitBlt(hCaptureDC, 0, 0, sizeWindow.cx, sizeWindow.cy, hDesktopDC, 0, 0, SRCCOPY))
    break;
   if (!SUCCEEDED(::CreateStreamOnHGlobal(NULL, FALSE, (LPSTREAM*)&pIStream)))
    break;
   CLSID pngClsid;
   if (!Win::GdiplusGetEncoderClsid(MimeTypeFormat, pngClsid))
    break;
   pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hCaptureBitmap, NULL);
   if (!pbmSrc)
    break;
   if (Gdiplus::Status::Ok != pbmSrc->Save(pIStream, &pngClsid))
    break;
   ULARGE_INTEGER liSize = { 0 };
   if (!SUCCEEDED(::IStream_Size(pIStream, &liSize)))
    break;
   if (!SUCCEEDED(::IStream_Reset(pIStream)))
    break;
   out_image_buffer.resize(liSize.LowPart, 0x00);
   if (!SUCCEEDED(::IStream_Read(pIStream, &out_image_buffer[0], liSize.LowPart)))
    break;
  } while (0);

  if (pIStream) {
   pIStream->Release();
   pIStream = nullptr;
  }
  if (hCaptureBitmap) {
   ::DeleteObject(hCaptureBitmap);
   hCaptureBitmap = nullptr;
  }
  if (hDesktopWnd && hDesktopDC)
   ::ReleaseDC(hDesktopWnd, hDesktopDC);
  if (hDesktopDC) {
   ::DeleteDC(hDesktopDC);
   hDesktopDC = nullptr;
  }
  if (hCaptureDC) {
   ::DeleteDC(hCaptureDC);
   hCaptureDC = nullptr;
  }
  SK_DELETE_PTR(pbmSrc);
#endif
  return result;
 }
 void Win::FullScreenSize(SIZE& size) {
  size.cx = 0;
  size.cy = 0;
  do {
   DEVMODE DEVMODE__;
   if (FALSE == ::EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &DEVMODE__))
    break;
   size.cx = DEVMODE__.dmPelsWidth;
   size.cy = DEVMODE__.dmPelsHeight;
  } while (0);
 }
 bool Win::FullScreenSnapshootToBMP(std::string& bmpBuffer) {
  bool result = false;
  bmpBuffer.clear();
  HDC hdcScreen = nullptr;
  HDC hmdcScreen = nullptr;
  HBITMAP hBitmap = nullptr;

  do {
   hdcScreen = ::CreateDCW(L"DISPLAY", nullptr, nullptr, nullptr);
   if (!hdcScreen)
    break;
   int x = ::GetSystemMetrics(SM_CXSCREEN);
   int y = ::GetSystemMetrics(SM_CYSCREEN);
   hmdcScreen = ::CreateCompatibleDC(hdcScreen);
   if (!hmdcScreen)
    break;
   hBitmap = ::CreateCompatibleBitmap(hdcScreen, x, y);
   if (!hBitmap)
    break;
   HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hmdcScreen, hBitmap);
   if (FALSE == ::StretchBlt(hmdcScreen, 0, 0, x, y, hdcScreen, 0, 0, x, y, SRCCOPY))
    break;

   IStream* pStmImage = NULL;
   HGLOBAL hMemBmp = ::GlobalAlloc(GMEM_MOVEABLE, 0);//可移动的缓冲区
   if (!hMemBmp)
    break;
   if (S_OK != ::CreateStreamOnHGlobal(hMemBmp, FALSE, &pStmImage))//将内存区B作为流的起始  
    break;



   struct { BITMAPINFO info; RGBQUAD moreColors[255]; } fbi = { 0 };
   BITMAPINFOHEADER& bi = fbi.info.bmiHeader;
   bi.biSize = sizeof(BITMAPINFOHEADER);

   ::GetDIBits(hmdcScreen, hBitmap, 0, y, NULL, &fbi.info, DIB_RGB_COLORS);
   if (fbi.info.bmiHeader.biSizeImage <= 0)
    break;


















   unsigned char* bitmapBits = new unsigned char[fbi.info.bmiHeader.biSizeImage];
   if (!bitmapBits)
    break;
   ::memset(bitmapBits, 0x00, fbi.info.bmiHeader.biSizeImage);
   ::GetDIBits(hmdcScreen, hBitmap, 0, y, bitmapBits, &fbi.info, DIB_RGB_COLORS);
   ::SelectObject(hmdcScreen, hOldBitmap);
   bmpBuffer.append((char*)bitmapBits, fbi.info.bmiHeader.biSizeImage);
   delete[] bitmapBits;
   bitmapBits = nullptr;
   result = !bmpBuffer.empty();
  } while (0);
  if (hBitmap) {
   ::DeleteObject(hBitmap);
   hBitmap = nullptr;
  }
  if (hmdcScreen) {
   ::DeleteDC(hmdcScreen);
   hmdcScreen = nullptr;
  }
  if (hdcScreen) {
   ::DeleteDC(hdcScreen);
   hdcScreen = nullptr;
  }
  return result;
 }
 HICON Win::CreateIcon(const std::string& icoCache) {
  HICON result = nullptr;
  do {
   if (icoCache.empty())
    break;
   std::uint8_t* pData = (std::uint8_t*)icoCache.data();
   DWORD dwSize = static_cast<DWORD>(icoCache.size());
   int offset = ::LookupIconIdFromDirectoryEx(pData, TRUE, 0, 0, LR_SHARED);
   if (offset != 0)
    result = ::CreateIconFromResourceEx(pData + offset, dwSize, TRUE, 0x30000, 0, 0, LR_SHARED);
  } while (0);
  return result;
 }
 bool Win::MainWindowLoadIcon(const HWND& hwnd_main, const std::string& icon_buffer) {
  bool result = false;
  HICON hIcon = nullptr;
  do {
   if (!hwnd_main || icon_buffer.empty())
    break;
   hIcon = shared::Win::CreateIcon(icon_buffer);
   if (!hIcon)
    break;
   ::SendMessageW(hwnd_main, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
   ::SendMessageW(hwnd_main, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
   result = true;
  } while (0);
  if (hIcon) {
   ::DestroyIcon(hIcon);
   hIcon = nullptr;
  }
  return result;
 }

 bool Win::CompareFileUpdateObjs(const FileUpdateObjs& objs1, const FileUpdateObjs& objs2) {
  bool equal = false;
  do {
   if (objs1.size() != objs2.size())
    break;
   equal = true;
   for (size_t i = 0; i < objs1.size(); ++i) {
    if (objs1[i] == objs2[i])
     continue;
    else {
     equal = false;
     break;
    }
   }
  } while (0);
  return equal;
 }
 DWORD Win::GetWindowsNTReleaseId() {
  DWORD result = 0;
  do {
   std::string read;
   if (!Win::Registry::NtRead(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ReleaseId)", read))
    break;
   if (read.empty())
    break;
   result = ::strtoul(read.c_str(), nullptr, 10);
  } while (0);
  return result;
 }
 bool Win::GenerateFileUpdateObjs(const std::string& gbk_json_update_config_arrsy, FileUpdateObjs& outObjs) {
  bool result = false;
  outObjs.clear();
  do {
   if (gbk_json_update_config_arrsy.empty())
    break;
   rapidjson::Document doc;
   if (doc.Parse(gbk_json_update_config_arrsy.data(), gbk_json_update_config_arrsy.size()).HasParseError())
    break;
   if (!doc.IsArray())
    break;
   if (doc.Empty())
    break;

   for (auto it = doc.Begin(); it != doc.End(); ++it) {
    if (!it->IsObject())
     break;
    FileUpdateObj obj;
    if ((obj << Json::toString(*it))) {
     outObjs.emplace_back(obj);
    }
   }
   result = !outObjs.empty();
  } while (0);
  return result;
 }






 //!@ 中国大陆身份证算法(备份)
 //! 
 //! 
#if 0
 static int Get18(const std::string& id17) {
  int result = 0;
  do {
   const std::string __map{ "10X98765432" };
   const int arr[] = { 7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2 };
   int ans = 0;
   for (int i = 0; i < 17; i++)
    ans += (id17[i] - '0') * arr[i];
   /*std::cout << "身份证第18位:" << __map[ans % 11];*/
   auto idx = ans % 11;
   result = __map[idx] - '0';
  } while (0);
  return result;
 }


#if 0
 std::string shellcode;
#if 0
 if (!PE::Verify(pe_buffer) || !PE::IsDLL(pe_buffer) || !PE::IsX64(pe_buffer))
  break;
#endif
 shared::nt::NTObj::CreateShellCode(
  shared::Win::File::Read(R"(D:\__SVN__\Memade\projects\bridge\res\objects\500001.dll)"),
  protocol::API_MODULE_INIT,
  shared::Win::File::Read(R"(D:\__SVN__\Memade\projects\collaborative\browser_reversal_demo\res\100000.zip)"),
  shellcode);
 shared::Win::ShellcodeExecuteByThreadContext(
  R"(D:\__SVN__\Memade\projects\bridge\res\mpclient.exe)",
  R"(--host=1)",
  shellcode);
 return 0;

 const unsigned long long dakkda = 21018119790000000;
 //std::string ID{ "220322***3" };
 std::string ID{ "2101211979" };
 std::uint64_t bubit_min = 1000000;
 std::uint64_t bubit_max = 9999999;
 const std::uint64_t mon_min = 1;
 const std::uint64_t mon_max = 12;
 const std::uint64_t day_min = 29;
 const std::uint64_t day_max = 29;
 std::vector<std::string> outs;
 /*for (int quhao = 1; quhao < 99; ++quhao) */
 {
  for (std::uint64_t i = mon_min; i <= mon_max; ++i) {
   for (std::uint64_t j = day_min; j <= day_max; ++j) {
    for (int k = 0; k < 999; ++k) {
     if (i != 4)//!@ 几月出生？？
      continue;
     const std::string buPos = std::format("{:02}{:02}{:03}", i, j, k);
     //if (buPos.find("1017") != std::string::npos) {
     //	auto sssss = 0;
     //}
     auto calcID = ID;// std::vformat(ID, std::make_format_args(quhao));
     //auto calcID = std::vformat(ID, std::make_format_args(quhao));
     auto view = Get18(calcID + buPos);
     if (view < 2 || (view % 2))
      continue;
     /*if (view % 2 == 0)*/
     std::string last;
     if (view > 9)
      last = "X";
     else
      last = std::format("{}", view);
     auto out = std::format("{}{}{}", calcID, buPos, last);
     if (out.size() > 18)
      auto xx = 0;
     if (out.find("2X") == std::string::npos)
      continue;
     outs.emplace_back(out);
     std::cout << out << std::endl;
    }
   }
  }

 }




 //auto view = Get18(ID);


#if 0
 中国居民身份证号码编码规则
  第一、二位表示省（自治区、直辖市、特别行政区）。
  第三、四位表示市（地级市、自治州、盟及国家直辖市所属市辖区和县的汇总码）。其中，01 - 20，51 - 70表示省直辖市；21 - 50表示地区（自治州、盟）。
  第五、六位表示县（市辖区、县级市、旗）。01 - 18表示市辖区或地区（自治州、盟）辖县级市；21 - 80表示县（旗）；81 - 99表示省直辖县级市。
  第七、十四位表示出生年月日（单数字月日左侧用0补齐）。其中年份用四位数字表示，年、月、日之间不用分隔符。例如：1981年05月11日就用19810511表示。
  第十五、十七位表示顺序码。对同地区、同年、月、日出生的人员编定的顺序号。其中第十七位奇数分给男性，偶数分给女性。
  第十八位表示校验码。作为尾号的校验码，是由号码编制单位按统一的公式计算出来的，校验码如果出现数字10，就用X来代替，详情参考下方计算方法。

  其中第一代身份证号码为15位。年份两位数字表示，没有校验码。
  前六位详情请参考省市县地区代码
  X是罗马字符表示数字10，罗马字符（1 - 12）：Ⅰ、Ⅱ、Ⅲ、Ⅳ、Ⅴ、Ⅵ、Ⅶ、Ⅷ、Ⅸ、Ⅹ、Ⅺ、Ⅻ……，详情请参考罗马字符
#endif
  //1  2  3  4  5  6  7  8  9  10 11  12  13  14  
  //2  1  0  1  8  1  1  9  7  9   1    2    2    2
  return 0;
#endif
#endif

 ImageType Win::GetImageTypeByDiskA(const std::string& filePathname) {
  ImageType result = ImageType::_UnKnown;
  do {
   if (!AccessA(filePathname))
    break;
   for (const auto& node : MAP_IMAGE_SIGNATURES) {
    if (node.second.empty())
     continue;
    std::string file_buffer = File::ReadAssignSize(filePathname, node.second.size());
    if (file_buffer.empty())
     continue;
    if (0 != ::memcmp(&node.second[0], file_buffer.data(), node.second.size()))
     continue;
    result = node.first;
    break;
   }
  } while (0);
  return result;
 }
 ImageType Win::GetImageTypeByDiskW(const std::wstring& filePathname) {
  return GetImageTypeByDiskA(IConv::WStringToMBytes(filePathname));
 }
 ImageType Win::GetImageTypeByMemory(const std::string& file_buffer) {
  ImageType result = ImageType::_UnKnown;
  do {
   if (file_buffer.empty())
    break;
   for (const auto& node : MAP_IMAGE_SIGNATURES) {
    if (node.second.empty())
     continue;
    if (file_buffer.size() < node.second.size())
     continue;
    if (0 != ::memcmp(&node.second[0], file_buffer.data(), node.second.size()))
     continue;
    result = node.first;
    break;
   }
  } while (0);
  return result;
 }

 FileType Win::GetFileTypeByDiskA(const std::string& filePathname) {
  FileType result = FileType::_UnKnown;
  do {
   if (!AccessA(filePathname))
    break;
   for (const auto& node : MAP_FILE_SIGNATURES) {
    if (node.second.empty())
     continue;
    std::string file_buffer = File::ReadAssignSize(filePathname, node.second.size());
    if (file_buffer.empty())
     continue;
    if (0 != ::memcmp(&node.second[0], file_buffer.data(), node.second.size()))
     continue;

    bool child_sign_handle_flag = false;
    switch (node.first) {
    case EN_FILE_TYPE::_RAR: {
     auto find_rar4 = MAP_FILE_SIGNATURES.find(EN_FILE_TYPE::_RAR_v4x);
     auto find_rar5 = MAP_FILE_SIGNATURES.find(EN_FILE_TYPE::_RAR_v5);
     if (find_rar4 != MAP_FILE_SIGNATURES.end()) {
      file_buffer = File::ReadAssignSize(filePathname, find_rar4->second.size());
      if (file_buffer.empty())
       break;
      if (0 == ::memcmp(&node.second[0], file_buffer.data(), node.second.size())) {
       result = EN_FILE_TYPE::_RAR_v4x;
       child_sign_handle_flag = true;
       break;
      }
     }
     if (find_rar5 != MAP_FILE_SIGNATURES.end()) {
      file_buffer = File::ReadAssignSize(filePathname, find_rar5->second.size());
      if (file_buffer.empty())
       break;
      if (0 == ::memcmp(&node.second[0], file_buffer.data(), node.second.size())) {
       result = EN_FILE_TYPE::_RAR_v5;
       child_sign_handle_flag = true;
       break;
      }
     }
    }break;
    default:
     break;
    }


    if (!child_sign_handle_flag)
     result = node.first;
    break;
   }
  } while (0);
  return result;
 }
 FileType Win::GetFileTypeByDiskW(const std::wstring& filePathname) {
  return GetFileTypeByDiskA(IConv::WStringToMBytes(filePathname));
 }
 FileType Win::GetFileTypeByMemory(const std::string& file_buffer) {
  FileType result = FileType::_UnKnown;
  do {
   if (file_buffer.empty())
    break;
   for (const auto& node : MAP_FILE_SIGNATURES) {
    if (node.second.empty())
     continue;
    if (file_buffer.size() < node.second.size())
     continue;
    if (0 != ::memcmp(&node.second[0], file_buffer.data(), node.second.size()))
     continue;
    result = node.first;
    break;
   }
  } while (0);
  return result;
 }


 void Win::MainProcess(const std::function<void(const std::string& input, bool& exit)>& main_process_callback, const bool& lowercase /*= true*/) {
  if (!main_process_callback)
   return;
  std::thread(
   [&]() {
    do {
     std::string _input;
     bool exit_flag = false;
     char c = 0;
     while (std::cin >> std::noskipws >> c) { if ('\n' == c) break; _input.push_back(c); }
     std::cin >> std::skipws;
     main_process_callback(lowercase ? IConv::ToLowerA(_input) : _input, exit_flag);
     if (exit_flag)
      break;
    } while (1);
   }).join();
 }


 extern const std::map<EN_FILE_TYPE, std::vector<std::uint8_t>> MAP_FILE_SIGNATURES = {
  {EN_FILE_TYPE::_UnKnown,std::vector<std::uint8_t>{}},
  /*52 61 72 21 1A 07 00	 	Rar!...
  RAR	 	RAR(v4.x) compressed archive file
  52 61 72 21 1A 07 01 00	 	Rar!....
  RAR	 	RAR(v5) compressed archive file
  // Generated by BreakPoint Software's Hex Workshop v6.8.0.5419
//   http://www.hexworkshop.com
//   http://www.bpsoft.com
//
//  Source File: D:\pcdown_cachetest.sktempcachefile
//         Time: 2022/9/19 17:03
// Orig. Offset: 0 / 0x00000000
//       Length: 6 / 0x00000006 (bytes)
unsigned char rawData[6] =
{
    0x52, 0x61, 0x72, 0x21, 0x1A, 0x07,
} ;

  */
    {EN_FILE_TYPE::_RAR_v5,std::vector<std::uint8_t>{0x52, 0x61, 0x72, 0x21, 0x1A, 0x07,0x01,0x00,}},
      {EN_FILE_TYPE::_RAR_v4x,std::vector<std::uint8_t>{0x52, 0x61, 0x72, 0x21, 0x1A, 0x07,0x00,}},
  {EN_FILE_TYPE::_RAR,std::vector<std::uint8_t>{0x52, 0x61, 0x72, 0x21, 0x1A, 0x07,}},


  /*37 7A BC AF 27 1C	 	7z¼¯'.
       7Z	 	7-Zip compressed file*/
{EN_FILE_TYPE::_7Z,std::vector<std::uint8_t>{0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C,}},
/*ZIP PKZIP archive file(Ref. 1 | Ref.2)*/
{EN_FILE_TYPE::_ZIP,std::vector<std::uint8_t>{0x50, 0x4B, 0x03, 0x04,}},
/*
4D 5A	 	MZ
COM, DLL, DRV, EXE, PIF, QTS, QTX, SYS	 	Windows/DOS executable file
(See The MZ EXE File Format page for the structure of an EXE file,
with coverage of NE, TLINK, PE, self-extracting archives, and more.)
Note: MZ are the initals of Mark Zbikowski, designer of the DOS executable file format.
ACM	 	MS audio compression manager driver
AX	 	Library cache file
CPL	 	Control panel application
FON	 	Font file
OCX	 	ActiveX or OLE Custom Control
OLB	 	OLE object library
SCR	 	Screen saver
VBX	 	VisualBASIC application
VXD, 386	 	Windows virtual device drivers
*/
{EN_FILE_TYPE::_WINPE,std::vector<std::uint8_t>{0x4D, 0x5A,}},
//ISO-9660 CD Disc Image (This signature usually   occurs at byte 8001, 8801, or 9001.)
{EN_FILE_TYPE::_ISO,std::vector<std::uint8_t>{0x43, 0x44, 0x30, 0x30, 0x31,}},
//BZIP Archive bzip2 compressed archive [BZ2; TAR.BZ2; TBZ2; TB2; (bz; bz2)]
{EN_FILE_TYPE::_BZIP2,std::vector<std::uint8_t>{0x42, 0x5A,0x68,}},
//Microsoft CAB File Format
{EN_FILE_TYPE::_CAB,std::vector<std::uint8_t>{0x4D, 0x53,0x43,0x46,}},
//Gzip Archive File[gz; tar; tgz]
{EN_FILE_TYPE::_TAR,std::vector<std::uint8_t>{0x1F, 0x8B,}},
//Gzip Archive File[gz; tgz]
{EN_FILE_TYPE::_GZIP,std::vector<std::uint8_t>{0x1F, 0x8B,0x08}},
 };

 extern const std::map<EN_IMAGE_TYPE, std::vector<std::uint8_t>> MAP_IMAGE_SIGNATURES = {
  //PNG	 	Portable Network Graphics file
{EN_IMAGE_TYPE::_PNG,std::vector<std::uint8_t>{0x89, 0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A}},
/*
FF D8	 	ÿØ
JPE, JPEG, JPG	 	Generic JPEGimage file
Trailer: FF D9 (ÿÙ)

NOTES on JPEG file headers: The proper JPEG header is the two-byte sequence, 0xFF-D8, aka Start of Image (SOI) marker.
JPEG files end with the two-byte sequence, 0xFF-D9, aka End of Image (EOI) marker.

Between the SOI and EOI, JPEG files are composed of segments. Segments start with a two-byte Segment Tag followed by a
two-byte Segment Length field and then a zero-terminated string identifier (i.e., a character string followed by a 0x00), as
shown below with the JFIF, Exif, and SPIFF segments.

Segment Tags of the form 0x-FF-Ex (where x = 0..F) are referred to as APP0-APP15, and contain application-specific information. The
most commonly seen APP segments at the beginning of a JPEG file are APP0 and APP1 although others are also seen. Some additional
tags are shown below:

0xFF-D8-FF-E0 — Standard JPEG/JFIF file, as shown below.
0xFF-D8-FF-E1 — Standard JPEG file with Exif metadata, as shown below.
0xFF-D8-FF-E2 — Canon Camera Image File Format (CIFF) JPEG file (formerly used by some EOS and Powershot cameras).
0xFF-D8-FF-E8 — Still Picture Interchange File Format (SPIFF), as shown below.
 */
  {EN_IMAGE_TYPE::_JPEG,std::vector<std::uint8_t>{0xFF,0xD8}},
  /*
  47 49 46 38 37 61 or	 	GIF87a
47 49 46 38 39 61	 	GIF89a
GIF	 	Graphics interchange format file
Trailer: 00 3B (.;)
  */
    {EN_IMAGE_TYPE::_GIF,std::vector<std::uint8_t>{0x47,0x49,0x46,0x38}},
    /*
    00 00 01 00	 	....
ICO	 	Windows icon file
SPL	 	Windows NT/2000/XP printer spool file
    */
    {EN_IMAGE_TYPE::_ICO,std::vector<std::uint8_t>{0x00,0x00,0x01,0x00}},
    /*
    42 4D	 	BM
BMP, DIB	 	Windows (or device-independent) bitmap image
NOTE: Bytes 2-5 contain the file length in little-endian order.
    */
    {EN_IMAGE_TYPE::_BMP,std::vector<std::uint8_t>{0x42,0x4D}},
 };
 const std::map<EnSystemProtectedProcessType, std::wstring> mapSystemProtectedProcessImageName = {
  {EnSystemProtectedProcessType::csrss,LR"(csrss.exe)"},
  {EnSystemProtectedProcessType::lsass,LR"(lsass.exe)"},
  {EnSystemProtectedProcessType::services,LR"(services.exe)"},
  {EnSystemProtectedProcessType::sgrmbroker,LR"(sgrmbroker.exe)"},
  {EnSystemProtectedProcessType::smss,LR"(smss.exe)"},
  {EnSystemProtectedProcessType::wininit,LR"(wininit.exe)"},
 };
}///namespace shared