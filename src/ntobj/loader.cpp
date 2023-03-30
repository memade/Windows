#include "stdafx.h"

namespace shared {
	namespace nt {
		NTObj::RouteObj::MemoryModule::MemoryModule(
			const HANDLE& hProcess,
			const std::string& processIdentify,
			const std::string& moduleBuffer,
			const std::string& resBuffer) :
			m_hProcess(hProcess),
			m_Identify(processIdentify),
			m_InitDataCache(resBuffer),
			m_ModuleBuffer(moduleBuffer) {
			Init();
		}
		NTObj::RouteObj::MemoryModule::MemoryModule(const HANDLE& hProcess, const std::string& identify, const std::string& moduleBuffer, const std::map<unsigned long, std::string>& init_datas) :
			m_hProcess(hProcess),
			m_Identify(identify),
			m_ModuleBuffer(moduleBuffer) {
			shared::Win::Resource::MadeRoute(init_datas, m_InitDataCache);
			Init();
		}
		NTObj::RouteObj::MemoryModule::~MemoryModule() {
			UnInit();
		}
		bool NTObj::RouteObj::MemoryModule::Ready() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			return m_Ready.load();
		}
		const std::string& NTObj::RouteObj::MemoryModule::Identify() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			return m_Identify;
		}
		bool NTObj::CreateThreadContext(const std::string& processPathname,
			const std::string& commandline, const std::string& peBuffer,
			const std::string& routePak,
			const bool& Inheriting_parent_process /*= true*/) {
			bool result = false;
			do {
				if (peBuffer.empty())
					break;
				STARTUPINFOA si = { 0 };
				si.cb = sizeof(si);
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_HIDE;
				PROCESS_INFORMATION pi = { 0 };
				if (FALSE == ::CreateProcessA(
					processPathname.empty() ? NULL : processPathname.c_str(),   // No module name (use command line)
					commandline.empty() ? NULL : const_cast<char*>(("/c " + commandline).c_str()),        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					Inheriting_parent_process ? TRUE : FALSE,          // Set handle inheritance to FALSE
					CREATE_SUSPENDED,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si,            // Pointer to STARTUPINFO structure
					&pi)           // Pointer to PROCESS_INFORMATION structure
					)
					break;

				char* shellcode = nullptr;
				DWORD shellcode_size = 0;
				do {
					if (!shared::injection::Reflecive::GenerateShellcode(
						(void*)peBuffer.data(),
						static_cast<DWORD>(peBuffer.size()),
						injection::Reflecive::HashFunctionName("api_object_init"),
						routePak.empty() ? nullptr : reinterpret_cast<void*>(const_cast<char*>(routePak.data())),
						static_cast<DWORD>(routePak.size()),
						injection::EnShellCodeFlag::EN_SC_CLEARHEADER,
						shellcode, shellcode_size))
						break;
					if (!shellcode || shellcode_size <= 0)
						break;
					CONTEXT threadContext = { 0 };
					threadContext.ContextFlags = CONTEXT_ALL;
					if (FALSE == ::GetThreadContext(pi.hThread, &threadContext))
						break;
					LPVOID lpBase = ::VirtualAllocEx(pi.hProcess, nullptr, shellcode_size,
						MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
					if (!lpBase)
						break;
					SIZE_T sizeNumberOfBytesWritten = 0;
					if (FALSE == ::WriteProcessMemory(pi.hProcess, lpBase,
						shellcode, shellcode_size, &sizeNumberOfBytesWritten))
						break;
#ifdef _WIN64
					threadContext.Rip = reinterpret_cast<decltype(threadContext.Rip)>(lpBase);
#else
					threadContext.Eip = reinterpret_cast<decltype(threadContext.Eip)>(lpBase);
#endif
					if (FALSE == ::SetThreadContext(pi.hThread, &threadContext))
						break;
					result = true;
				} while (0);
				::ResumeThread(pi.hThread);
				::WaitForSingleObject(pi.hProcess, INFINITE);
				SK_DELETE_PTR_C(shellcode);
				SK_CLOSE_HANDLE(pi.hThread);
				SK_CLOSE_HANDLE(pi.hProcess);
			} while (0);
			return result;
		}
		bool NTObj::CreateShellCode(const std::string& pe_buffer, const std::string& init_function, const std::string& user_data, std::string& shellcode) {
			bool result = false;
			shellcode.clear();
			do {
				if (pe_buffer.empty() || init_function.empty())
					break;
				if (!shared::Win::PE::Verify(pe_buffer))
					break;
				if (!shared::Win::PE::IsDLL(pe_buffer))
					break;
				if (!shared::injection::Reflecive::GenerateShellcode(\
					pe_buffer, init_function, user_data, \
					shared::injection::EnShellCodeFlag::EN_SC_CLEARHEADER, shellcode))
					break;
				result = true;
			} while (0);
			return result;
		}
		bool NTObj::CreateThreadContextByToken(const std::string& processPathname,
			const std::string& commandline, const std::string& peBuffer, const std::string& routePak,
			const HANDLE& hByToken
		) {
			bool result = false;
			do {
				if (peBuffer.empty())
					break;
				STARTUPINFOW si = { 0 };
				si.cb = sizeof(si);
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_HIDE;
				PROCESS_INFORMATION pi = { 0 };
				if (!::CreateProcessWithTokenW(hByToken,
					LOGON_WITH_PROFILE,
					IConv::MBytesToWString(processPathname).c_str(),
					commandline.empty() ? NULL : const_cast<wchar_t*>((L"/c " + shared::IConv::MBytesToWString(commandline)).c_str()),
					CREATE_SUSPENDED, NULL, NULL, &si, &pi))
					break;

				char* shellcode = nullptr;
				DWORD shellcode_size = 0;
				do {
					if (!shared::injection::Reflecive::GenerateShellcode(
						(void*)peBuffer.data(),
						static_cast<DWORD>(peBuffer.size()),
						injection::Reflecive::HashFunctionName("api_object_init"),
						routePak.empty() ? nullptr : reinterpret_cast<void*>(const_cast<char*>(routePak.data())),
						static_cast<DWORD>(routePak.size()),
						injection::EnShellCodeFlag::EN_SC_CLEARHEADER,
						shellcode, shellcode_size))
						break;
					if (!shellcode || shellcode_size <= 0)
						break;
					CONTEXT threadContext = { 0 };
					threadContext.ContextFlags = CONTEXT_ALL;
					if (FALSE == ::GetThreadContext(pi.hThread, &threadContext))
						break;
					LPVOID lpBase = ::VirtualAllocEx(pi.hProcess, nullptr, shellcode_size,
						MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
					if (!lpBase)
						break;
					SIZE_T sizeNumberOfBytesWritten = 0;
					if (FALSE == ::WriteProcessMemory(pi.hProcess, lpBase,
						shellcode, shellcode_size, &sizeNumberOfBytesWritten))
						break;
#ifdef _WIN64
					threadContext.Rip = reinterpret_cast<decltype(threadContext.Rip)>(lpBase);
#else
					threadContext.Eip = reinterpret_cast<decltype(threadContext.Eip)>(lpBase);
#endif
					if (FALSE == ::SetThreadContext(pi.hThread, &threadContext))
						break;
					result = true;
				} while (0);
				::ResumeThread(pi.hThread);
				::WaitForSingleObject(pi.hProcess, INFINITE);
				SK_DELETE_PTR_C(shellcode);
				SK_CLOSE_HANDLE(pi.hThread);
				SK_CLOSE_HANDLE(pi.hProcess);
			} while (0);
			return result;
		}
		void NTObj::RouteObj::MemoryModule::Init() {
			char* shellcode = nullptr;
			DWORD shellcode_size = 0;
			do {
				if (m_Ready.load())
					break;
				if (!m_hProcess)
					break;
				if (!injection::Reflecive::GenerateShellcode(
					(void*)m_ModuleBuffer.data(),
					static_cast<DWORD>(m_ModuleBuffer.size()),
					injection::Reflecive::HashFunctionName("api_object_init"),
					m_InitDataCache.empty() ? nullptr : reinterpret_cast<void*>(const_cast<char*>(m_InitDataCache.data())),
					static_cast<DWORD>(m_InitDataCache.size()),
					injection::EnShellCodeFlag::EN_SC_CLEARHEADER,
					shellcode, shellcode_size))
					break;

#if 1
				m_lpBase = ::VirtualAllocEx(m_hProcess, nullptr, shellcode_size,
					MEM_COMMIT | MEM_RESERVE,
					PAGE_EXECUTE_READWRITE);
				if (!m_lpBase)
					break;
				SIZE_T sizeNumberOfBytesWritten = 0;
				if (0 == ::WriteProcessMemory(m_hProcess, m_lpBase,
					shellcode, shellcode_size, &sizeNumberOfBytesWritten))
					break;
#else
				HANDLE HeapHandle = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, shellcode_size, shellcode_size);
				void* m_lpBase = HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, shellcode_size);
				::memcpy(m_lpBase, shellcode, shellcode_size);
#endif
				m_Ready.store(true);
			} while (0);
			SK_DELETE_PTR_C(shellcode);
		}

		void NTObj::RouteObj::MemoryModule::UnInit() {
			if (!m_Ready.load())
				return;
			if (m_hProcess && m_lpBase) {
				if (0 != ::VirtualFreeEx(m_hProcess, m_lpBase, 0, MEM_RELEASE))
					m_lpBase = nullptr;
			}
			m_ExportFunctionPosQ.clear();
			m_Ready.store(false);
		}
#if 0
		bool NTObj::RouteObj::MemoryModule::Execute(DWORD& ProcessId, DWORD& ThreadId, void* param /*= nullptr*/) const {
			bool result = false;
			ThreadId = 0;
			ProcessId = 0;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				if (!m_Ready.load())
					break;
				HANDLE hRemoteThread = nullptr;
				::CLIENT_ID ClientID = { 0 };
				NTSTATUS status = RtlCreateUserThread(
					m_hProcess,
					NULL,
					FALSE,
					0,
					0,
					0,
					(PUSER_THREAD_START_ROUTINE)m_lpBase,
					param,
					&hRemoteThread,
					&ClientID);
				if (status < 0)
					break;
				ThreadId = SK_THREADID(ClientID.UniqueThread);
				ProcessId = SK_PROCESSID(ClientID.UniqueProcess);
				::WaitForSingleObject(hRemoteThread, INFINITE);
				SK_CLOSE_HANDLE(hRemoteThread);
				result = true;
			} while (0);
			return result;
		}
#endif
		bool NTObj::RouteObj::MemoryModule::Execute(void* param) const {
			bool result = false;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				if (!m_Ready.load())
					break;
#if 0
				DWORD dwRemoteThreadId = 0;
				HANDLE hRemoteThread = ::CreateRemoteThread(m_hProcess, \
					nullptr, 1024 * 1024, (LPTHREAD_START_ROUTINE)m_lpBase, \
					param, 0, &dwRemoteThreadId);
				if (!hRemoteThread)
					break;
				::WaitForSingleObject(hRemoteThread, INFINITE);
				DWORD dwThreadExitCode = 0;
				::GetExitCodeThread(hRemoteThread, &dwThreadExitCode);
				SK_CLOSE_HANDLE(hRemoteThread);
#else
				auto hNTDLL = ::GetModuleHandleW(L"NTDLL.DLL");
				if (!hNTDLL)
					break;
				tfRtlCreateUserThread fnRtlCreateUserThread = \
					reinterpret_cast<tfRtlCreateUserThread>(::GetProcAddress(hNTDLL, "RtlCreateUserThread"));
				if (!fnRtlCreateUserThread)
					break;
				HANDLE hRemoteThread = nullptr;
				NTSTATUS status = fnRtlCreateUserThread(
					m_hProcess,
					NULL,
					FALSE,
					0,
					0,
					0,
					(PUSER_THREAD_START_ROUTINE)m_lpBase,
					param,
					&hRemoteThread,
					NULL);
				if (status < 0)
					break;
				::WaitForSingleObject(hRemoteThread, INFINITE);
				SK_CLOSE_HANDLE(hRemoteThread);
#endif
				result = true;
			} while (0);
			return result;
		}
#if 0
		ULONG_PTR NTObj::RouteObj::MemoryModule::MemoryLoader(const void* pBuffer, const size_t& nBuffer) const {
			ULONG_PTR result = 0;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				HANDLE hRemoteThread = ::CreateRemoteThread(m_hProcess, NULL, 1024 * 1024, m_ReflectiveLoader, nullptr, (DWORD)NULL, NULL);
				if (!hRemoteThread)
					break;
				::WaitForSingleObject(hRemoteThread, INFINITE);
				DWORD dwThreadExitCode = 0;
				::GetExitCodeThread(hRemoteThread, &dwThreadExitCode);
				SK_CLOSE_HANDLE(hRemoteThread);
			} while (0);


			auto sk = 0;
#if 0
			void* lpRemoteThreadParameter = nullptr;
			void* lpRemoteModuleBuffer = nullptr;
			HANDLE hRemoteThread = nullptr;
			DWORD dwRemoteThreadId = 0;
			SIZE_T sizeNumberOfBytesWritten = 0;
			do {
				if (!m_hProcess)
					break;
				if (!pModuleBuffer || ModuleBufferSize <= 0)
					break;
				if (!m_MemoryLoadLibrary)
					break;

				lpRemoteModuleBuffer = ::VirtualAllocEx(m_hProcess, nullptr, ModuleBufferSize,
					MEM_COMMIT | MEM_RESERVE,
					PAGE_EXECUTE_READWRITE);
				if (!lpRemoteModuleBuffer)
					break;
				if (0 == ::WriteProcessMemory(m_hProcess,
					lpRemoteModuleBuffer,
					pModuleBuffer, ModuleBufferSize, &sizeNumberOfBytesWritten))
					break;

				shared::REMOTEROUTEDATA route;
				route._In_ptr = reinterpret_cast<decltype(route._In_ptr)>(lpRemoteModuleBuffer);
				route._In_size = ModuleBufferSize;

				lpRemoteThreadParameter = ::VirtualAllocEx(m_hProcess, nullptr, sizeof(shared::REMOTEROUTEDATA),
					MEM_COMMIT | MEM_RESERVE,
					PAGE_EXECUTE_READWRITE);
				if (!lpRemoteThreadParameter)
					break;

				if (0 == ::WriteProcessMemory(m_hProcess,
					lpRemoteThreadParameter,
					&route, sizeof(shared::REMOTEROUTEDATA), &sizeNumberOfBytesWritten))
					break;
				hRemoteThread = ::CreateRemoteThread(m_hProcess, nullptr, 0,
					reinterpret_cast<LPTHREAD_START_ROUTINE>(m_MemoryLoadLibrary),
					lpRemoteThreadParameter, 0, &dwRemoteThreadId);
				if (!hRemoteThread)
					break;
				::WaitForSingleObject(hRemoteThread, INFINITE);
				DWORD dwThreadResult = 0;
				::GetExitCodeThread(hRemoteThread, &dwThreadResult);
				if (!dwThreadResult)
					break;
				shared::REMOTEROUTEDATA readResult;
				SIZE_T dwRead = 0;
				if (0 == ::ReadProcessMemory(m_hProcess, lpRemoteThreadParameter, &readResult, sizeof(shared::REMOTEROUTEDATA), &dwRead))
					break;
				if (dwRead <= 0)
					break;
				result = reinterpret_cast<decltype(result)>(readResult._Out_ptr);
			} while (0);
			SK_CLOSE_HANDLE(hRemoteThread);
			//if (m_hProcess && lpRemoteThreadParameter) {
			//	if (0 != ::VirtualFreeEx(m_hProcess, lpRemoteThreadParameter, 0, MEM_RELEASE))
			//		lpRemoteThreadParameter = nullptr;
			//}
#endif
			return result;
		}
#endif
	}///namespace nt



}///namespace shared