#if !defined(AFX_SKSTU_DUMP_H__433CD_FF716475_C2C53070147C49A_98FD__HEAD__)
#define AFX_SKSTU_DUMP_H__433CD_FF716475_C2C53070147C49A_98FD__HEAD__

#include <Windows.h>
#include <Dbghelp.h>

#include <ctime>
#include <string>
#include <sstream>
#include <assert.h>
#include <shellapi.h>

namespace shared {

	class Dump final {
	public:
		typedef BOOL(WINAPI* MINIDUMPWRITEDUMP) (
			HANDLE hProcess,
			DWORD ProcessId,
			HANDLE hFile,
			MINIDUMP_TYPE DumpType,
			PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
			PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
			PMINIDUMP_CALLBACK_INFORMATION CallbackParam
			);
	private:
		typedef BOOL(WINAPI* getUserModeExceptionProc)(LPDWORD);
		typedef BOOL(WINAPI* setUserModeExceptionProc)(DWORD);
	public:
		explicit Dump() :
			m_oldExceptionFilter(NULL) {
			BOOL bRet = ::HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
			::OutputDebugStringW(format(L"HeapSetInformation, bRet: <%lu, %lu>.\n", bRet, ::GetLastError()).c_str());
			bRet = ::SetProcessDEPPolicy(PROCESS_DEP_ENABLE | PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION);
			::OutputDebugStringW(format(L"SetProcessDEPPolicy, bRet: <%lu, %lu>.\n", bRet, ::GetLastError()).c_str());
			m_oldExceptionFilter = ::SetUnhandledExceptionFilter(UnhandledExceptionFilter);
			HMODULE hKernel32 = GetModuleHandle(TEXT("KERNEL32"));
			if (NULL == hKernel32) {
				::OutputDebugStringW(L"GetModuleHandle faled.\n");
			}
			else {
				DWORD dwFlags = 0;
				getUserModeExceptionProc procGetProcessUserModeExceptionPolicy;
				setUserModeExceptionProc procSetProcessUserModeExceptionPolicy;
				procGetProcessUserModeExceptionPolicy = (getUserModeExceptionProc)::GetProcAddress(hKernel32, "GetProcessUserModeExceptionPolicy");
				procSetProcessUserModeExceptionPolicy = (setUserModeExceptionProc)::GetProcAddress(hKernel32, "SetProcessUserModeExceptionPolicy");
				if (procGetProcessUserModeExceptionPolicy && procSetProcessUserModeExceptionPolicy) {
					if (procGetProcessUserModeExceptionPolicy(&dwFlags)) {
						bRet = procSetProcessUserModeExceptionPolicy(dwFlags & ~1);
						::OutputDebugStringW(format(L"GetProcessUserModeExceptionPolicy, bRet: %lu.\n", bRet).c_str());
					}
					::OutputDebugStringW(format(L"SetProcessUserModeExceptionPolicy, bRet: %lu, dwFlags: %lu.\n", bRet, dwFlags).c_str());
				}
			}
		}
		//static LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
		explicit Dump(const LPTOP_LEVEL_EXCEPTION_FILTER& lptopLevelExceptionFilter) :
			m_oldExceptionFilter(nullptr) {
			BOOL bRet = ::HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
			::OutputDebugStringW(format(L"HeapSetInformation, bRet: <%lu, %lu>.\n", bRet, ::GetLastError()).c_str());
			bRet = ::SetProcessDEPPolicy(PROCESS_DEP_ENABLE | PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION);
			::OutputDebugStringW(format(L"SetProcessDEPPolicy, bRet: <%lu, %lu>.\n", bRet, ::GetLastError()).c_str());
			m_oldExceptionFilter = ::SetUnhandledExceptionFilter(lptopLevelExceptionFilter);
			HMODULE hKernel32 = GetModuleHandle(TEXT("KERNEL32"));
			if (NULL == hKernel32) {
				::OutputDebugStringW(L"GetModuleHandle faled.\n");
			}
			else {
				DWORD dwFlags = 0;
				getUserModeExceptionProc procGetProcessUserModeExceptionPolicy;
				setUserModeExceptionProc procSetProcessUserModeExceptionPolicy;
				procGetProcessUserModeExceptionPolicy = (getUserModeExceptionProc)::GetProcAddress(hKernel32, "GetProcessUserModeExceptionPolicy");
				procSetProcessUserModeExceptionPolicy = (setUserModeExceptionProc)::GetProcAddress(hKernel32, "SetProcessUserModeExceptionPolicy");
				if (procGetProcessUserModeExceptionPolicy && procSetProcessUserModeExceptionPolicy) {
					if (procGetProcessUserModeExceptionPolicy(&dwFlags)) {
						bRet = procSetProcessUserModeExceptionPolicy(dwFlags & ~1);
						::OutputDebugStringW(format(L"GetProcessUserModeExceptionPolicy, bRet: %lu.\n", bRet).c_str());
					}
					::OutputDebugStringW(format(L"SetProcessUserModeExceptionPolicy, bRet: %lu, dwFlags: %lu.\n", bRet, dwFlags).c_str());
				}
			}
		}
		~Dump() {
			if (NULL != m_oldExceptionFilter) {
				::SetUnhandledExceptionFilter(m_oldExceptionFilter);
			}
		}
	private:
		LPTOP_LEVEL_EXCEPTION_FILTER        m_oldExceptionFilter;
	private:
		static std::wstring format(const wchar_t* pszFormat, ...) {
			wchar_t buffer[MAX_PATH] = { 0 };
			va_list ap;
			va_start(ap, pszFormat);
			int nCount = ::vswprintf_s(buffer, _countof(buffer), pszFormat, ap);
			va_end(ap);
			if (nCount < 0) {
				assert(false);
				return pszFormat;
			}
			return buffer;
		}
		static LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo) {
			if (::IsDebuggerPresent()) {
				::OutputDebugStringW(L"IsDebuggerPresent return TRUE.\n");
				return EXCEPTION_CONTINUE_SEARCH;
			}
			static BOOL inExceptionHandler = FALSE;
			if (inExceptionHandler) {
				::OutputDebugStringW(L"Current function has crashed.Shit.\n");
				return EXCEPTION_CONTINUE_SEARCH;
			}
			inExceptionHandler = TRUE;
			WCHAR fullPath[MAX_PATH] = { 0 };
			DWORD pathLength = ::GetModuleFileNameW(NULL, fullPath, MAX_PATH);
			if (0 == pathLength) {
				::OutputDebugStringW(L"GetModuleFileNameW failed.\n");
				return EXCEPTION_CONTINUE_SEARCH;
			}
			LPCWSTR lastSlash = ::wcsrchr(fullPath, L'\\');
			if (NULL == lastSlash) {
				::OutputDebugStringW(L"wcsrchr return wrong.\n");
				return EXCEPTION_CONTINUE_SEARCH;
			}
			std::wstring exeDirPath(fullPath, lastSlash - fullPath + 1);
			WCHAR filePath[MAX_PATH] = { 0 };
			for (int i = 0; ; ++i) {
				SYSTEMTIME sys_time = { 0 };
				::GetLocalTime(&sys_time);
				::swprintf_s(filePath, _countof(filePath) - 1, L"%s%04u_%02u_%02u_%02u_%02u_%02u_%d.dmp"
					, exeDirPath.c_str()
					, sys_time.wYear, sys_time.wMonth, sys_time.wDay
					, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, i);
				if (::GetFileAttributesW(filePath) == INVALID_FILE_ATTRIBUTES) {
					break;
				}
			}
			HANDLE hFile = ::CreateFileW(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile) {
				::OutputDebugStringW(L"CreateFileW failed.\n");
				return EXCEPTION_CONTINUE_SEARCH;
			}
			HMODULE hDbgHelp = LoadLibraryW(L"DBGHELP");
			if (!hDbgHelp) {
				::OutputDebugStringW(L"LoadLibraryW DBGHELP failed.\n");
				return EXCEPTION_CONTINUE_SEARCH;
			}
			MINIDUMPWRITEDUMP fnMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
			if (!fnMiniDumpWriteDump)
			{
				::OutputDebugStringW(L"GetProcAddress MiniDumpWriteDump failed.\n");

				::FreeLibrary(hDbgHelp);

				return EXCEPTION_CONTINUE_SEARCH;
			}
			MINIDUMP_TYPE dumpFlags = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithUnloadedModules | MiniDumpWithProcessThreadData);
			MINIDUMP_EXCEPTION_INFORMATION miniInfo = { 0 };
			miniInfo.ClientPointers = TRUE;
			miniInfo.ExceptionPointers = pExceptionInfo;
			miniInfo.ThreadId = ::GetCurrentThreadId();
			if (fnMiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, dumpFlags, &miniInfo, NULL, NULL)) {
				WCHAR buffer[MAX_PATH] = { 0 };
				::swprintf_s(buffer, _countof(buffer) - 1, L"Process has crashed.\nMinidump was saved to: \n\\%s\n", filePath);
				::OutputDebugStringW(buffer);
				//::MessageBoxW(NULL, buffer, NULL, MB_ICONERROR | MB_OK);
			}
			else {
				::OutputDebugStringW(format(L"Minidump was saved failed: %hu.\n", ::GetLastError()).c_str());

				::MessageBoxW(NULL, format(L"Minidump was saved failed: %hu.\n", ::GetLastError()).c_str(), NULL, MB_ICONERROR | MB_OK);
			}
			::FreeLibrary(hDbgHelp);
			::CloseHandle(hFile);
			return EXCEPTION_CONTINUE_SEARCH;
		}
	};


}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年07月18日 10:2:50|928**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_DUMP_H__433CD_FF716475_C2C53070147C49A_98FD__TAIL__