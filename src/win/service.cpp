#include "stdafx.h"

namespace shared {
 bool Win::Service::Create(const std::string& service_name, const std::string& PEPathname, const std::string& display_name, const std::string& Description, const DWORD& dwStartType) {
  bool result = false;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  do {
   if (!hServiceManager)
    break;
   if ((hService = ::CreateServiceA(
    hServiceManager,
    service_name.c_str(),
    display_name.c_str(),
    SERVICE_ALL_ACCESS,
    SERVICE_WIN32_OWN_PROCESS,
    dwStartType,
    SERVICE_ERROR_IGNORE,
    PEPathname.c_str(),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL)) == NULL) {
    auto error_code = ::GetLastError();
    switch (error_code) {
    case ERROR_SERVICE_EXISTS: {
     result = true;
    }break;
    default:
     break;
    }
    break;
   }

   /*char lpDescription[] = "A Sample Service , Test Example";*/
   SERVICE_DESCRIPTIONA sd = { 0 };
   sd.lpDescription = (char*)&Description[0];
   ::ChangeServiceConfig2A(hService, SERVICE_CONFIG_DESCRIPTION, &sd);

   result = true;
  } while (0);
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }
 bool Win::Service::DeleteW(const std::wstring& service_name) {
  bool result = false;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = nullptr;
  do {
   if (service_name.empty())
    break;
   hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hServiceManager)
    break;
   hService = ::OpenServiceW(hServiceManager, service_name.c_str(), SERVICE_ALL_ACCESS);
   if (!hService) {
    auto error_code = ::GetLastError();
    switch (error_code) {
    case ERROR_SERVICE_DOES_NOT_EXIST: {
     result = true;
    }break;
    default:
     break;
    }
    break;
   }
   if (!::DeleteService(hService))
    break;
   result = true;
  } while (0);
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }
 bool Win::Service::DeleteA(const std::string& service_name) {
  return DeleteW(IConv::MBytesToWString(service_name));
 }
 bool Win::Service::QueryConfigStartName(const std::string& service_name, std::string& start_name) {
  bool result = false;
  start_name.clear();
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = nullptr;
  LPQUERY_SERVICE_CONFIGA pQUERY_SERVICE_CONFIGA = nullptr;
  do {
   if (service_name.empty())
    break;
   hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hServiceManager)
    break;
   hService = ::OpenServiceA(hServiceManager, service_name.c_str(), SERVICE_ALL_ACCESS);
   if (!hService)
    break;

   DWORD dwBytesNeeded = 0;
   DWORD cbBufSize = 0;
   DWORD dwError = 0;
   if (!::QueryServiceConfigA(hService, pQUERY_SERVICE_CONFIGA, 0, &dwBytesNeeded)) {
    dwError = ::GetLastError();
    if (ERROR_INSUFFICIENT_BUFFER == dwError) {
     cbBufSize = dwBytesNeeded;
     pQUERY_SERVICE_CONFIGA = (LPQUERY_SERVICE_CONFIGA)::LocalAlloc(LMEM_FIXED, cbBufSize);
    }
    else
     break;
   }
   if (!::QueryServiceConfigA(hService, pQUERY_SERVICE_CONFIGA, cbBufSize, &dwBytesNeeded))
    break;
   if (!pQUERY_SERVICE_CONFIGA)
    break;
   start_name = pQUERY_SERVICE_CONFIGA->lpServiceStartName ? pQUERY_SERVICE_CONFIGA->lpServiceStartName : "";
   result = !start_name.empty();
  } while (0);
  if (pQUERY_SERVICE_CONFIGA) {
   ::LocalFree(pQUERY_SERVICE_CONFIGA);
   pQUERY_SERVICE_CONFIGA = nullptr;
  }
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }
 bool Win::Service::QueryConfig(const std::string& service_name, const std::function<void(const QUERY_SERVICE_CONFIGA&)>& query_cb) {
  bool result = false;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = nullptr;
  LPQUERY_SERVICE_CONFIGA pQUERY_SERVICE_CONFIGA = nullptr;
  do {
   if (service_name.empty())
    break;
   hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hServiceManager)
    break;
   hService = ::OpenServiceA(hServiceManager, service_name.c_str(), SERVICE_ALL_ACCESS);
   if (!hService)
    break;
   DWORD dwBytesNeeded = 0;
   DWORD cbBufSize = 0;
   DWORD dwError = 0;
   if (!::QueryServiceConfigA(hService, pQUERY_SERVICE_CONFIGA, 0, &dwBytesNeeded)) {
    dwError = ::GetLastError();
    if (ERROR_INSUFFICIENT_BUFFER == dwError) {
     cbBufSize = dwBytesNeeded;
     pQUERY_SERVICE_CONFIGA = (LPQUERY_SERVICE_CONFIGA)::LocalAlloc(LMEM_FIXED, cbBufSize);
    }
    else
     break;
   }
   if (!::QueryServiceConfigA(hService, pQUERY_SERVICE_CONFIGA, cbBufSize, &dwBytesNeeded))
    break;
   query_cb(*pQUERY_SERVICE_CONFIGA);
   result = true;
  } while (0);
  if (pQUERY_SERVICE_CONFIGA) {
   ::LocalFree(pQUERY_SERVICE_CONFIGA);
   pQUERY_SERVICE_CONFIGA = nullptr;
  }
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }
 bool Win::Service::QueryConfigStartType(const std::string& service_name, DWORD& startType) {
  bool result = false;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = nullptr;
  LPQUERY_SERVICE_CONFIGA pQUERY_SERVICE_CONFIGA = nullptr;
  startType = SERVICE_DISABLED;
  do {
   if (service_name.empty())
    break;
   hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hServiceManager)
    break;
   hService = ::OpenServiceA(hServiceManager, service_name.c_str(), SERVICE_ALL_ACCESS);
   if (!hService)
    break;

   DWORD dwBytesNeeded = 0;
   DWORD cbBufSize = 0;
   DWORD dwError = 0;
   if (!::QueryServiceConfigA(hService, pQUERY_SERVICE_CONFIGA, 0, &dwBytesNeeded)) {
    dwError = ::GetLastError();
    if (ERROR_INSUFFICIENT_BUFFER == dwError) {
     cbBufSize = dwBytesNeeded;
     pQUERY_SERVICE_CONFIGA = (LPQUERY_SERVICE_CONFIGA)::LocalAlloc(LMEM_FIXED, cbBufSize);
    }
    else
     break;
   }
   if (!::QueryServiceConfigA(hService, pQUERY_SERVICE_CONFIGA, cbBufSize, &dwBytesNeeded))
    break;
   if (!pQUERY_SERVICE_CONFIGA)
    break;
   startType = pQUERY_SERVICE_CONFIGA->dwStartType;
  } while (0);
  if (pQUERY_SERVICE_CONFIGA) {
   ::LocalFree(pQUERY_SERVICE_CONFIGA);
   pQUERY_SERVICE_CONFIGA = nullptr;
  }
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }
 bool Win::Service::ConfigStartName(const std::string& service_name, const std::string& account_name /*= ""*/) {
  bool result = false;
  std::string account(account_name.empty() ? ".\\LocalSystem" : account_name);

  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = nullptr;
  do {
   if (service_name.empty())
    break;
   hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hServiceManager)
    break;
   hService = ::OpenServiceA(hServiceManager, service_name.c_str(), SERVICE_ALL_ACCESS);
   if (!hService)
    break;
   if (!ChangeServiceConfigA(
    hService,        // handle of service 
    /*SERVICE_NO_CHANGE*/SERVICE_WIN32_OWN_PROCESS, // service type: no change 
    SERVICE_NO_CHANGE,  // service start type 
    SERVICE_NO_CHANGE, // error control: no change 
    NULL,              // binary path: no change 
    NULL,              // load order group: no change 
    NULL,              // tag ID: no change 
    NULL,              // dependencies: no change 
    account.c_str(),              // account name: no change 
    NULL,              // password: no change 
    NULL))            // display name: no change
    break;
   result = true;
  } while (0);
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }

 bool Win::Service::ConfigStartNameIsLocalSystem(const std::string& service_name) {
  bool result = false;
  do {
   if (service_name.empty())
    break;
   std::string query_result;
   if (!Service::QueryConfigStartName(service_name, query_result))
    break;
   result = ::_stricmp(query_result.c_str(), "localsystem") == 0;
  } while (0);
  return result;
 }
 bool Win::Service::ConfigStartType(const std::string& service_name, const DWORD& startType /*= SERVICE_AUTO_START*/) {
  bool result = false;

  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = nullptr;
  do {
   if (service_name.empty())
    break;
   hServiceManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hServiceManager)
    break;
   hService = ::OpenServiceA(hServiceManager, service_name.c_str(), SERVICE_ALL_ACCESS);
   if (!hService)
    break;
   if (!ChangeServiceConfigA(
    hService,        // handle of service 
    SERVICE_NO_CHANGE, // service type: no change 
    startType,  // service start type 
    SERVICE_NO_CHANGE, // error control: no change 
    NULL,              // binary path: no change 
    NULL,              // load order group: no change 
    NULL,              // tag ID: no change 
    NULL,              // dependencies: no change 
    NULL,              // account name: no change 
    NULL,              // password: no change 
    NULL))            // display name: no change
    break;

   result = true;
  } while (0);
  if (hService) {
   ::CloseServiceHandle(hService);
   hService = nullptr;
  }
  if (hServiceManager) {
   ::CloseServiceHandle(hServiceManager);
   hServiceManager = nullptr;
  }
  return result;
 }
 bool Win::Service::StartA(const std::string& service_name, const std::int64_t& wait_time_ms) {
  return StartW(IConv::MBytesToWString(service_name), wait_time_ms);
 }
 bool Win::Service::StartW(const std::wstring& service_name, const std::int64_t& wait_time_ms) {
  bool result = false;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = ::OpenSCManagerW(NULL, NULL, GENERIC_EXECUTE);
  do {
   if (!hServiceManager)
    break;
   hService = ::OpenServiceW(hServiceManager, service_name.c_str(),
    SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
   if (!hService)
    break;
   SERVICE_STATUS service_status = { 0 };
   if (!::QueryServiceStatus(hService, &service_status))
    break;
   bool __Continue = false;
   switch (service_status.dwCurrentState) {
   case SERVICE_RUNNING: {
   }break;
   case SERVICE_START_PENDING: {
   }break;
   default: {
    __Continue = true;
   }break;
   }
   if (!__Continue) {
    result = true;
    break;
   }
   if (!::StartService(hService, NULL, NULL))
    break;
   if (wait_time_ms <= 0) {
    result = true;
    break;
   }
   auto total_ms = wait_time_ms;
   do {
    if (total_ms < 100)
     break;
    if (!::QueryServiceStatus(hService, &service_status))
     break;
    if (service_status.dwCurrentState == SERVICE_RUNNING) {
     result = true;
     break;
    }
    total_ms -= 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
   } while (1);
  } while (0);
  if (hService) {
   if (::CloseServiceHandle(hService))
    hService = nullptr;
  }
  if (hServiceManager) {
   if (::CloseServiceHandle(hServiceManager))
    hServiceManager = nullptr;
  }
  return result;
 }

 bool Win::Service::StopA(const std::string& service_name, const std::int64_t& wait_time_ms) {
  return StopW(IConv::MBytesToWString(service_name), wait_time_ms);
 }

 static BOOL __stdcall StopDependentServices(const SC_HANDLE& schSCManager, const SC_HANDLE& schService)
 {
  DWORD i;
  DWORD dwBytesNeeded;
  DWORD dwCount;

  LPENUM_SERVICE_STATUS   lpDependencies = NULL;
  ENUM_SERVICE_STATUS     ess;
  SC_HANDLE               hDepService;
  SERVICE_STATUS_PROCESS  ssp;

  DWORD dwStartTime = ::GetTickCount();
  DWORD dwTimeout = 30000; // 30-second time-out

  // Pass a zero-length buffer to get the required buffer size.
  if (::EnumDependentServicesW(schService, SERVICE_ACTIVE,
   lpDependencies, 0, &dwBytesNeeded, &dwCount))
  {
   // If the Enum call succeeds, then there are no dependent
   // services, so do nothing.
   return TRUE;
  }
  else
  {
   if (GetLastError() != ERROR_MORE_DATA)
    return FALSE; // Unexpected error

   // Allocate a buffer for the dependencies.
   lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
    GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

   if (!lpDependencies)
    return FALSE;

   __try {
    // Enumerate the dependencies.
    if (!EnumDependentServices(schService, SERVICE_ACTIVE,
     lpDependencies, dwBytesNeeded, &dwBytesNeeded,
     &dwCount))
     return FALSE;

    for (i = 0; i < dwCount; i++)
    {
     ess = *(lpDependencies + i);
     // Open the service.
     hDepService = ::OpenServiceW(schSCManager,
      ess.lpServiceName,
      SERVICE_STOP | SERVICE_QUERY_STATUS);

     if (!hDepService)
      return FALSE;

     __try {
      // Send a stop code.
      if (!ControlService(hDepService,
       SERVICE_CONTROL_STOP,
       (LPSERVICE_STATUS)&ssp))
       return FALSE;

      // Wait for the service to stop.
      while (ssp.dwCurrentState != SERVICE_STOPPED)
      {
       Sleep(ssp.dwWaitHint);
       if (!QueryServiceStatusEx(
        hDepService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded))
        return FALSE;

       if (ssp.dwCurrentState == SERVICE_STOPPED)
        break;

       if (GetTickCount() - dwStartTime > dwTimeout)
        return FALSE;
      }
     }
     __finally
     {
      // Always release the service handle.
      CloseServiceHandle(hDepService);
     }
    }
   }
   __finally
   {
    // Always free the enumeration buffer.
    HeapFree(GetProcessHeap(), 0, lpDependencies);
   }
  }
  return TRUE;
 }
 bool Win::Service::StopW(const std::wstring& service_name, const std::int64_t& wait_time_ms) {
  bool result = false;
#if 1
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = ::OpenSCManagerW(NULL, NULL, GENERIC_EXECUTE);
  do {
   if (!hServiceManager)
    break;
   hService = ::OpenServiceW(hServiceManager, service_name.c_str(),
    SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
   if (!hService) {
    auto error_code = ::GetLastError();
    switch (error_code) {
    case ERROR_SERVICE_DOES_NOT_EXIST: {
     result = true;
    }break;
    default:
     break;
    }
    break;
   }
   SERVICE_STATUS service_status = { 0 };
   if (!::QueryServiceStatus(hService, &service_status))
    break;
   bool __Continue = false;
   switch (service_status.dwCurrentState) {
   case SERVICE_STOPPED: {
    result = true;
   }break;
   case SERVICE_STOP_PENDING: {
   }break;
   default: {
    __Continue = true;
   }break;
   }
   if (!__Continue) {
    result = true;
    break;
   }
   if (!::ControlService(hService, SERVICE_CONTROL_STOP, &service_status))
    break;
   //ChangeServiceConfig()
   if (wait_time_ms <= 0) {
    result = true;
    break;
   }
   auto total_ms = wait_time_ms;
   do {
    if (total_ms < 100)
     break;
    if (!::QueryServiceStatus(hService, &service_status))
     break;
    if (service_status.dwCurrentState == SERVICE_STOPPED) {
     result = true;
     break;
    }
    total_ms -= 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
   } while (1);

   result = true;
  } while (0);
  SK_CLOSE_SERVICE_HANDLE(hService);
  SK_CLOSE_SERVICE_HANDLE(hServiceManager);
#else
  //
  // Purpose: 
  //   Stops the service.
  //
  // Parameters:
  //   None
  // 
  // Return value:
  //   None
  //
  SERVICE_STATUS_PROCESS ssp = { 0 };
  DWORD dwStartTime = GetTickCount();
  DWORD dwBytesNeeded = 0;
  DWORD dwTimeout = 30000; // 30-second time-out
  DWORD dwWaitTime = 0;
  SC_HANDLE schSCManager = nullptr;
  SC_HANDLE schService = nullptr;
  do {
   // Get a handle to the SCM database. 
   schSCManager = ::OpenSCManagerW(
    NULL,                    // local computer
    NULL,                    // ServicesActive database 
    SC_MANAGER_ALL_ACCESS);  // full access rights 
   if (NULL == schSCManager)
    break;
   // Get a handle to the service.
   schService = ::OpenServiceW(
    schSCManager,         // SCM database 
    service_name.c_str(),            // name of service 
    SERVICE_STOP |
    SERVICE_QUERY_STATUS |
    SERVICE_ENUMERATE_DEPENDENTS);
   if (schService == NULL)
    break;
   // Make sure the service is not already stopped.
   if (FALSE == ::QueryServiceStatusEx(
    schService,
    SC_STATUS_PROCESS_INFO,
    (LPBYTE)&ssp,
    sizeof(SERVICE_STATUS_PROCESS),
    &dwBytesNeeded))
    break;
   if (ssp.dwCurrentState == SERVICE_STOPPED)
    break;
   // If a stop is pending, wait for it.
   while (ssp.dwCurrentState == SERVICE_STOP_PENDING) {
    // Do not wait longer than the wait hint. A good interval is 
    // one-tenth of the wait hint but not less than 1 second  
    // and not more than 10 seconds. 
    dwWaitTime = ssp.dwWaitHint / 10;
    if (dwWaitTime < 1000)
     dwWaitTime = 1000;
    else if (dwWaitTime > 10000)
     dwWaitTime = 10000;
    ::Sleep(dwWaitTime);
    if (FALSE == ::QueryServiceStatusEx(
     schService,
     SC_STATUS_PROCESS_INFO,
     (LPBYTE)&ssp,
     sizeof(SERVICE_STATUS_PROCESS),
     &dwBytesNeeded))
     break;

    if (ssp.dwCurrentState == SERVICE_STOPPED)
     break;

    if (::GetTickCount() - dwStartTime > dwTimeout)
     break;
   }
   // If the service is running, dependencies must be stopped first.
   StopDependentServices(schSCManager, schService);
   // Send a stop code to the service.
   if (FALSE == ::ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp))
    break;
   // Wait for the service to stop.
   while (ssp.dwCurrentState != SERVICE_STOPPED) {
    ::Sleep(ssp.dwWaitHint);
    if (FALSE == ::QueryServiceStatusEx(
     schService,
     SC_STATUS_PROCESS_INFO,
     (LPBYTE)&ssp,
     sizeof(SERVICE_STATUS_PROCESS),
     &dwBytesNeeded))
     break;
    if (ssp.dwCurrentState == SERVICE_STOPPED)
     break;
    if (::GetTickCount() - dwStartTime > dwTimeout)
     break;
   }
   result = true;
  } while (0);
  SK_CLOSE_SERVICE_HANDLE(schService);
  SK_CLOSE_SERVICE_HANDLE(schSCManager);
#endif
  return result;
 }

 bool Win::Service::RestartW(const std::wstring& service_name) {
  bool result = false;
  do {
   if (service_name.empty())
    break;
   Service::StopW(service_name, INFINITE);
   if (!Service::StartW(service_name, INFINITE))
    break;
   result = true;
  } while (0);
  return result;
 }
 bool Win::Service::RestartA(const std::string& service_name) {
  return RestartW(IConv::MBytesToWString(service_name));
 }
 std::uint64_t Win::Service::Status(const SC_HANDLE& hServiceManager, const SC_HANDLE& hService) {
  std::uint64_t result = SERVICE_STOPPED;
  do {
   if (!hServiceManager || !hService)
    break;
   SERVICE_STATUS service_status = { 0 };
   if (!::QueryServiceStatus(hService, &service_status))
    break;
   result = service_status.dwCurrentState;
  } while (0);
  return result;
 }
 std::uint64_t Win::Service::StatusW(const std::wstring& service_name) {
  std::uint64_t result = SERVICE_STOPPED;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = ::OpenSCManagerW(NULL, NULL, GENERIC_EXECUTE);
  do {
   if (!hServiceManager)
    break;
   hService = ::OpenServiceW(hServiceManager, service_name.c_str(),
    SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
   if (!hService)
    break;
   SERVICE_STATUS service_status = { 0 };
   if (!::QueryServiceStatus(hService, &service_status))
    break;
   result = service_status.dwCurrentState;
  } while (0);
  SK_CLOSE_SERVICE_HANDLE(hService);
  SK_CLOSE_SERVICE_HANDLE(hServiceManager);
  return result;
 }
 std::uint64_t Win::Service::StatusA(const std::string& service_name) {
  return StatusW(IConv::MBytesToWString(service_name));
 }
 bool Win::Service::ServiceExists(const std::string& service_name) {
  bool result = false;
  SC_HANDLE hService = nullptr;
  SC_HANDLE hServiceManager = ::OpenSCManager(NULL, NULL, GENERIC_ALL);
  do {
   if (service_name.empty())
    break;
   if (!hServiceManager)
    break;
   hService = ::OpenServiceA(hServiceManager, service_name.c_str(), SERVICE_QUERY_STATUS);
   if (!hService)
    break;
#if 0
#define SERVICE_STOPPED                        0x00000001
#define SERVICE_START_PENDING                  0x00000002
#define SERVICE_STOP_PENDING                   0x00000003
#define SERVICE_RUNNING                        0x00000004
#define SERVICE_CONTINUE_PENDING               0x00000005
#define SERVICE_PAUSE_PENDING                  0x00000006
#define SERVICE_PAUSED                         0x00000007
#endif
   result = true;
  } while (0);
  if (hService) {
   if (::CloseServiceHandle(hService))
    hService = nullptr;
  }
  if (hServiceManager) {
   if (::CloseServiceHandle(hServiceManager))
    hServiceManager = nullptr;
  }
  return result;
 }
 std::wstring WmiQueryValue(IWbemClassObject* pclsObj, LPCWSTR szName)
 {
  std::wstring value;
  if (pclsObj != NULL && szName != NULL)
  {
   VARIANT vtProp;
   HRESULT hr = pclsObj->Get(szName, 0, &vtProp, 0, 0);
   if (SUCCEEDED(hr))
   {
    if (vtProp.vt == VT_BSTR && ::SysStringLen(vtProp.bstrVal) > 0)
    {
#ifdef _UNICODE
     value = vtProp.bstrVal;
#else
     int len = ::SysStringLen(vtProp.bstrVal) + 1;
     if (len > 0)
     {
      value.resize(len);
      ::WideCharToMultiByte(CP_ACP,
       0,
       vtProp.bstrVal,
       -1,
       &value[0],
       len,
       NULL,
       NULL);
     }
#endif
    }
   }
  }
  return value;
 }
 bool Win::Service::ServiceListBrief(std::set<std::string>& services) {
  bool result = false;
  services.clear();
  HRESULT hr;
  IWbemLocator* m_pLocator = nullptr;
  IWbemServices* m_pServices = nullptr;
  IEnumWbemClassObject* pEnumerator = nullptr;
  do {
   // Initialize COM.
   hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
   if (FAILED(hr))
    break;
   // Set general COM security levels
   hr = ::CoInitializeSecurity(
    NULL,
    -1,                          // COM authentication
    NULL,                        // Authentication services
    NULL,                        // Reserved
    RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
    RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
    NULL,                        // Authentication info
    EOAC_NONE,                   // Additional capabilities
    NULL                         // Reserved
   );
   if (FAILED(hr))
    break;
   HRESULT hr = ::CoCreateInstance(
    CLSID_WbemLocator,
    0,
    CLSCTX_INPROC_SERVER,
    IID_IWbemLocator, (LPVOID*)&m_pLocator);
   if (FAILED(hr))
    break;
   // Connect to WMI through the IWbemLocator::ConnectServer method
// Connect to the root\cimv2 namespace with the current user
   hr = m_pLocator->ConnectServer(
    _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
    NULL,                    // User name. NULL = current user
    NULL,                    // User password. NULL = current
    0,                       // Locale. NULL indicates current
    NULL,                    // Security flags.
    0,                       // Authority (e.g. Kerberos)
    0,                       // Context object
    &m_pServices             // pointer to IWbemServices proxy
   );
   if (FAILED(hr))
    break;
   // Set security levels on the proxy
   hr = ::CoSetProxyBlanket(
    m_pServices,                 // Indicates the proxy to set
    RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
    RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
    NULL,                        // Server principal name
    RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
    RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
    NULL,                        // client identity
    EOAC_NONE                    // proxy capabilities
   );
   if (FAILED(hr))
    break;
   pEnumerator = NULL;
   hr = m_pServices->ExecQuery(
    bstr_t("WQL"),
    bstr_t("SELECT * FROM Win32_Service"),
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    NULL,
    &pEnumerator);
   if (FAILED(hr))
    break;
   while (pEnumerator) {
    // Get the data from the query
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    do {
     hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
     if (FAILED(hr))
      break;
     // find the values of the properties we are interested in
     std::wstring name = WmiQueryValue(pclsObj, L"Name");
#if 0
     std::wstring publisher = WmiQueryValue(pclsObj, L"Vendor");
     std::wstring version = WmiQueryValue(pclsObj, L"Version");
     std::wstring location = WmiQueryValue(pclsObj, L"InstallLocation");
#endif
     if (!name.empty()) {
      std::string lower_name = IConv::WStringToMBytes(name);
      lower_name = IConv::ToLowerA(lower_name);
      services.emplace(lower_name);
     }
    } while (0);
    if (pclsObj) {
     pclsObj->Release();
     pclsObj = nullptr;
    }
    if (0 == uReturn)
     break;
   }
   result = true;
  } while (0);
  if (pEnumerator) {
   pEnumerator->Release();
   pEnumerator = nullptr;
  }
  if (m_pServices) {
   m_pServices->Release();
   m_pServices = nullptr;
  }
  if (m_pLocator) {
   m_pLocator->Release();
   m_pLocator = nullptr;
  }
  ::CoUninitialize();
  return result;
 }
}///namespace shared