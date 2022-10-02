#if !defined(INC_H___BD63BF90_7E1E_4A54_88C9_FFF675CC3020__HEAD__)
#define INC_H___BD63BF90_7E1E_4A54_88C9_FFF675CC3020__HEAD__

#include "win.h"

namespace shared {
 namespace hook {
  // This class needs to be instantiated and used
  // @System
  class System {
  public:
   using tfExitWindowsEx = 
    BOOL
    (WINAPI*)(
     _In_ UINT uFlags,
     _In_ DWORD dwReason);
  public:
   const tfExitWindowsEx ExitWindowsExLocal = ::ExitWindowsEx;
   tfExitWindowsEx ExitWindowsExRemote = nullptr;
  };

  // This class needs to be instantiated and used
  // @ws2_32.dll
  //
  class WinSock2 {
  public:
   using tfconnect =
    int
    (WSAAPI*)
    (
     _In_ SOCKET s,
     _In_reads_bytes_(namelen) const struct sockaddr FAR* name,
     _In_ int namelen
     );
   using tfWSASend =
    int
    (WSAAPI*)
    (
     _In_ SOCKET s,
     _In_reads_(dwBufferCount) LPWSABUF lpBuffers,
     _In_ DWORD dwBufferCount,
     _Out_opt_ LPDWORD lpNumberOfBytesSent,
     _In_ DWORD dwFlags,
     _Inout_opt_ LPWSAOVERLAPPED lpOverlapped,
     _In_opt_ LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
     );
  public:
   const tfconnect connectLocal = ::connect;
   tfconnect connectRemote = nullptr;
   const tfWSASend WSASendLocal = ::WSASend;
   tfWSASend WSASendRemote = nullptr;
  };


  // This class needs to be instantiated and used
  // @User32.dll
  //
  class User32 {
  public:
   using tfGetWindowTextW =
    int
    (WINAPI*)
    (
     _In_ HWND hWnd,
     _Out_writes_(nMaxCount) LPWSTR lpString,
     _In_ int nMaxCount
     );
   using tfSetWindowTextW =
    BOOL
    (WINAPI*)
    (
     _In_ HWND hWnd,
     _In_opt_ LPCWSTR lpString
     );

   using tfShowWindow =
    BOOL
    (WINAPI*)
    (
     _In_ HWND hWnd,
     _In_ int nCmdShow
     );
  public:
   const tfGetWindowTextW GetWindowTextWLocal = ::GetWindowTextW;
   tfGetWindowTextW GetWindowTextWRemote = nullptr;
   const tfSetWindowTextW SetWindowTextWLocal = ::SetWindowTextW;
   tfSetWindowTextW SetWindowTextWRemote = nullptr;
   const tfShowWindow ShowWindowLocal = ::ShowWindow;
   tfShowWindow ShowWindowRemote = nullptr;
  };
  // This class needs to be instantiated and used
  // @Shell32.dll
  //
  class Shell32 {
  public:
   using tfCommandLineToArgvW =
    LPWSTR *
    (WINAPI*)
    (
     _In_ LPCWSTR lpCmdLine, _Out_ int* pNumArgs
     );
   using tfShellExecuteExW =
    BOOL
    (WINAPI*)
    (
     _Inout_ SHELLEXECUTEINFOW* pExecInfo
     );
   using tfShellExecuteW =
    HINSTANCE
    (WINAPI*)
    (
     _In_opt_ HWND hwnd,
     _In_opt_ LPCWSTR lpOperation,
     _In_ LPCWSTR lpFile,
     _In_opt_ LPCWSTR lpParameters,
     _In_opt_ LPCWSTR lpDirectory,
     _In_ INT nShowCmd
     );
  public:
   const tfCommandLineToArgvW tfCommandLineToArgvWLocal = ::CommandLineToArgvW;
   tfCommandLineToArgvW CommandLineToArgvWRemote = nullptr;
   const tfShellExecuteExW ShellExecuteExWLocal = ::ShellExecuteExW;
   tfShellExecuteExW ShellExecuteExWRemote = nullptr;
   const tfShellExecuteW ShellExecuteWLocal = ::ShellExecuteW;
   tfShellExecuteW ShellExecuteWRemote = nullptr;
  };
  // This class needs to be instantiated and used
  // @Kernel32.dll
  //
  class Kernel32 {
  public:
   Kernel32() {
    m_hModue = ::GetModuleHandleW(L"Kernel32.dll");
    if (!m_hModue)
     m_hModue = ::LoadLibraryW(L"Kernel32.dll");
    if (m_hModue)
    {
     CreateProcessInternalWLocal = reinterpret_cast<decltype(CreateProcessInternalWLocal)>(::GetProcAddress(m_hModue, "CreateProcessInternalW"));
    }
   }
   ~Kernel32() {
   }
  public:
   using tfCreateProcessW =
    BOOL
    (WINAPI*)
    (
     _In_opt_ LPCWSTR lpApplicationName,
     _Inout_opt_ LPWSTR lpCommandLine,
     _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
     _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
     _In_ BOOL bInheritHandles,
     _In_ DWORD dwCreationFlags,
     _In_opt_ LPVOID lpEnvironment,
     _In_opt_ LPCWSTR lpCurrentDirectory,
     _In_ LPSTARTUPINFOW lpStartupInfo,
     _Out_ LPPROCESS_INFORMATION lpProcessInformation
     );
   using tfCreateProcessAsUserW =
    BOOL
    (WINAPI*)
    (
     _In_opt_ HANDLE hToken,
     _In_opt_ LPCWSTR lpApplicationName,
     _Inout_opt_ LPWSTR lpCommandLine,
     _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
     _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
     _In_ BOOL bInheritHandles,
     _In_ DWORD dwCreationFlags,
     _In_opt_ LPVOID lpEnvironment,
     _In_opt_ LPCWSTR lpCurrentDirectory,
     _In_ LPSTARTUPINFOW lpStartupInfo,
     _Out_ LPPROCESS_INFORMATION lpProcessInformation
     );

   using tfCreateProcessInternalW = BOOL(WINAPI*)(
    HANDLE hToken,
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation,
    PHANDLE hNewToken);
  public:
   const tfCreateProcessW CreateProcessWLocal = ::CreateProcessW;
   tfCreateProcessW CreateProcessWRemote = nullptr;
   const tfCreateProcessAsUserW CreateProcessAsUserWLocal = ::CreateProcessAsUserW;
   tfCreateProcessAsUserW CreateProcessAsUserWRemote = nullptr;
   tfCreateProcessInternalW CreateProcessInternalWLocal = nullptr;
   tfCreateProcessInternalW CreateProcessInternalWRemote = nullptr;
  private:
   HMODULE m_hModue = nullptr;
  };
  // This class needs to be instantiated and used
  // @WinHttp.dll
  //
  class WinHttp {
  public:
   WinHttp() {}
   ~WinHttp() {}
  public:
   // WinHttpCreateUrl
   using tfWinHttpCreateUrl =
    BOOL
    (WINAPI*)
    (
     _In_ LPURL_COMPONENTS lpUrlComponents,
     _In_ DWORD dwFlags,
     _Out_writes_to_opt_(*pdwUrlLength, *pdwUrlLength) LPWSTR pwszUrl,
     _Inout_ LPDWORD pdwUrlLength
     );
   // WinHttpAddRequestHeaders
   using tfWinHttpAddRequestHeaders =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     _When_(dwHeadersLength == (DWORD)-1, _In_z_)
     _When_(dwHeadersLength != (DWORD)-1, _In_reads_(dwHeadersLength))
     LPCWSTR lpszHeaders,
     IN DWORD dwHeadersLength,
     IN DWORD dwModifiers
     );
   // WinHttpAddRequestHeadersEx
   using tfWinHttpAddRequestHeadersEx =
    DWORD
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     IN DWORD dwModifiers,
     IN ULONGLONG ullFlags,
     IN ULONGLONG ullExtra,
     IN DWORD cHeaders,
     _In_reads_(cHeaders) WINHTTP_EXTENDED_HEADER* pHeaders
     );
   // WinHttpCloseHandle
   using tfWinHttpCloseHandle =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hInternet
     );
   // WinHttpConnect
   using tfWinHttpConnect =
    HINTERNET
    (WINAPI*)
    (
     IN HINTERNET hSession,
     IN LPCWSTR pswzServerName,
     IN INTERNET_PORT nServerPort,
     IN DWORD dwReserved
     );
   // WinHttpCrackUrl
   using tfWinHttpCrackUrl =
    BOOL
    (WINAPI*)
    (
     _In_reads_(dwUrlLength) LPCWSTR pwszUrl,
     _In_ DWORD dwUrlLength,
     _In_ DWORD dwFlags,
     _Inout_ LPURL_COMPONENTS lpUrlComponents
     );
   // WinHttpOpen
   using tfWinHttpOpen =
    HINTERNET
    (WINAPI*)
    (
     _In_opt_z_ LPCWSTR pszAgentW,
     _In_ DWORD dwAccessType,
     _In_opt_z_ LPCWSTR pszProxyW,
     _In_opt_z_ LPCWSTR pszProxyBypassW,
     _In_ DWORD dwFlags
     );
   // WinHttpOpenRequest
   using tfWinHttpOpenRequest =
    HINTERNET
    (WINAPI*)
    (
     IN HINTERNET hConnect,
     IN LPCWSTR pwszVerb,
     IN LPCWSTR pwszObjectName,
     IN LPCWSTR pwszVersion,
     IN LPCWSTR pwszReferrer OPTIONAL,
     IN LPCWSTR FAR* ppwszAcceptTypes OPTIONAL,
     IN DWORD dwFlags
     );
   // WinHttpQueryHeaders
   using tfWinHttpQueryHeaders =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     IN DWORD dwInfoLevel,
     IN LPCWSTR pwszName OPTIONAL,
     _Out_writes_bytes_to_opt_(*lpdwBufferLength, *lpdwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer,
     IN OUT LPDWORD lpdwBufferLength,
     IN OUT LPDWORD lpdwIndex OPTIONAL
     );
   // WinHttpReadData
   using tfWinHttpReadData =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     _Out_writes_bytes_to_(dwNumberOfBytesToRead, *lpdwNumberOfBytesRead) __out_data_source(NETWORK) LPVOID lpBuffer,
     IN DWORD dwNumberOfBytesToRead,
     OUT LPDWORD lpdwNumberOfBytesRead
     );
   // WinHttpReceiveResponse
   using tfWinHttpReceiveResponse =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     IN LPVOID lpReserved
     );
   // WinHttpSendRequest
   using tfWinHttpSendRequest =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     _In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
     IN DWORD dwHeadersLength,
     _In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional,
     IN DWORD dwOptionalLength,
     IN DWORD dwTotalLength,
     IN DWORD_PTR dwContext
     );

   using tfWinHttpSetOption =
    BOOL
    (WINAPI*)
    (
     _In_opt_ HINTERNET hInternet,
     _In_ DWORD dwOption,
     _When_((dwOption == WINHTTP_OPTION_USERNAME ||
      dwOption == WINHTTP_OPTION_PASSWORD ||
      dwOption == WINHTTP_OPTION_PROXY_USERNAME ||
      dwOption == WINHTTP_OPTION_PROXY_PASSWORD ||
      dwOption == WINHTTP_OPTION_USER_AGENT),
      _At_((LPCWSTR)lpBuffer, _In_reads_(dwBufferLength)))
     _When_((dwOption == WINHTTP_OPTION_CLIENT_CERT_CONTEXT),
      _In_reads_bytes_opt_(dwBufferLength))
     _When_((dwOption != WINHTTP_OPTION_USERNAME &&
      dwOption != WINHTTP_OPTION_PASSWORD &&
      dwOption != WINHTTP_OPTION_PROXY_USERNAME &&
      dwOption != WINHTTP_OPTION_PROXY_PASSWORD &&
      dwOption != WINHTTP_OPTION_CLIENT_CERT_CONTEXT &&
      dwOption != WINHTTP_OPTION_USER_AGENT),
      _In_reads_bytes_(dwBufferLength))
     LPVOID lpBuffer,
     _In_ DWORD dwBufferLength
     );
   // WinHttpSetTimeouts
   using tfWinHttpSetTimeouts =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hInternet,           // Session/Request handle.
     IN int nResolveTimeout,
     IN int nConnectTimeout,
     IN int nSendTimeout,
     IN int nReceiveTimeout
     );
   // WinHttpWriteData
   using tfWinHttpWriteData =
    BOOL
    (WINAPI*)
    (
     IN HINTERNET hRequest,
     _In_reads_bytes_opt_(dwNumberOfBytesToWrite) LPCVOID lpBuffer,
     IN DWORD dwNumberOfBytesToWrite,
     OUT LPDWORD lpdwNumberOfBytesWritten
     );
  public:
   const tfWinHttpWriteData WinHttpWriteDataLocal = ::WinHttpWriteData;
   tfWinHttpWriteData WinHttpWriteDataRemote = nullptr;
   const tfWinHttpAddRequestHeaders WinHttpAddRequestHeadersLocal = ::WinHttpAddRequestHeaders;
   tfWinHttpAddRequestHeaders WinHttpAddRequestHeadersRemote = nullptr;
   const tfWinHttpAddRequestHeadersEx WinHttpAddRequestHeadersExLocal = ::WinHttpAddRequestHeadersEx;
   tfWinHttpAddRequestHeadersEx WinHttpAddRequestHeadersExRemote = nullptr;
   const tfWinHttpSendRequest WinHttpSendRequestLocal = ::WinHttpSendRequest;
   tfWinHttpSendRequest WinHttpSendRequestRemote = nullptr;
   const tfWinHttpOpenRequest WinHttpOpenRequestLocal = ::WinHttpOpenRequest;
   tfWinHttpOpenRequest WinHttpOpenRequestRemote = nullptr;
   const tfWinHttpConnect WinHttpConnectLocal = ::WinHttpConnect;
   tfWinHttpConnect WinHttpConnectRemote = nullptr;
   const tfWinHttpCrackUrl WinHttpCrackUrlLocal = ::WinHttpCrackUrl;
   tfWinHttpCrackUrl WinHttpCrackUrlRemote = nullptr;
   const tfWinHttpQueryHeaders WinHttpQueryHeadersLocal = ::WinHttpQueryHeaders;
   tfWinHttpQueryHeaders WinHttpQueryHeadersRemote = nullptr;
   const tfWinHttpSetOption WinHttpSetOptionLocal = ::WinHttpSetOption;
   tfWinHttpSetOption WinHttpSetOptionRemote = nullptr;
   const tfWinHttpCreateUrl WinHttpCreateUrlLocal = ::WinHttpCreateUrl;
   tfWinHttpCreateUrl WinHttpCreateUrlRemote = nullptr;
   const tfWinHttpReceiveResponse WinHttpReceiveResponseLocal = ::WinHttpReceiveResponse;
   tfWinHttpReceiveResponse WinHttpReceiveResponseRemote = nullptr;
  };

 }///namespace hook
}///namespace shared


/// /*新生®（上海）**/
/// /*2022_02_23T00:09:06.6067378Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___BD63BF90_7E1E_4A54_88C9_FFF675CC3020__HEAD__