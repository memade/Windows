#include "stdafx.h"

namespace shared {
	namespace nt {

		void NTObj::Init() {
			do {
				if (m_CurrentProcessId.load() <= 0)
					m_CurrentProcessId.store(::GetCurrentProcessId());
				if (m_CurrentProcessHandle.load() == nullptr)
					m_CurrentProcessHandle.store(::GetCurrentProcess());
				__ElevatedPrivileges();
				if (m_Ready.load())
					return;
				m_hNTDLL = ::GetModuleHandleW(L"ntdll.dll");
				m_hKERNEL32 = ::GetModuleHandleW(L"kernel32.dll");
				if (!m_hNTDLL || !m_hKERNEL32)
					break;

				NtCreateToken = \
					reinterpret_cast<tfNtCreateToken>(\
						::GetProcAddress(m_hNTDLL, "NtCreateToken"));
				NtOpenProcessToken = \
					reinterpret_cast<tfNtOpenProcessToken>(\
						::GetProcAddress(m_hNTDLL, "NtOpenProcessToken"));
				NtOpenProcessTokenEx = \
					reinterpret_cast<tfNtOpenProcessTokenEx>(\
						::GetProcAddress(m_hNTDLL, "NtOpenProcessTokenEx"));
				NtOpenThreadToken = \
					reinterpret_cast<tfNtOpenThreadToken>(\
						::GetProcAddress(m_hNTDLL, "NtOpenThreadToken"));
				NtOpenThreadTokenEx = \
					reinterpret_cast<tfNtOpenThreadTokenEx>(\
						::GetProcAddress(m_hNTDLL, "NtOpenThreadTokenEx"));
				NtDuplicateToken = \
					reinterpret_cast<tfNtDuplicateToken>(\
						::GetProcAddress(m_hNTDLL, "NtDuplicateToken"));
				NtQueryInformationToken = \
					reinterpret_cast<tfNtQueryInformationToken>(\
						::GetProcAddress(m_hNTDLL, "NtQueryInformationToken"));
				NtSetInformationToken = \
					reinterpret_cast<tfNtSetInformationToken>(\
						::GetProcAddress(m_hNTDLL, "NtSetInformationToken"));
				NtAdjustPrivilegesToken = \
					reinterpret_cast<tfNtAdjustPrivilegesToken>(\
						::GetProcAddress(m_hNTDLL, "NtAdjustPrivilegesToken"));
				NtAdjustGroupsToken = \
					reinterpret_cast<tfNtAdjustGroupsToken>(\
						::GetProcAddress(m_hNTDLL, "NtAdjustGroupsToken"));
				if (!NtCreateToken || !NtOpenProcessToken || !NtOpenProcessTokenEx || \
					!NtOpenThreadToken || !NtOpenThreadTokenEx || !NtDuplicateToken || !NtQueryInformationToken || \
					!NtSetInformationToken || !NtAdjustPrivilegesToken || !NtAdjustGroupsToken)
					break;

				NtClose = \
					reinterpret_cast<tfNtClose>(\
						::GetProcAddress(m_hNTDLL, "NtClose"));
				NtOpenProcess = \
					reinterpret_cast<tfNtOpenProcess>(\
						::GetProcAddress(m_hNTDLL, "NtOpenProcess"));
				NtTerminateProcess = \
					reinterpret_cast<tfNtTerminateProcess>(\
						::GetProcAddress(m_hNTDLL, "NtTerminateProcess"));
				NtQuerySystemInformation = \
					reinterpret_cast<tfNtQuerySystemInformation>(\
						::GetProcAddress(m_hNTDLL, "NtQuerySystemInformation"));
				NtQueryInformationProcess = \
					reinterpret_cast<tfNtQueryInformationProcess>(\
						::GetProcAddress(m_hNTDLL, "NtQueryInformationProcess"));
				if (!NtQuerySystemInformation || !NtQueryInformationProcess || !NtTerminateProcess || !NtOpenProcess || !NtClose)
					break;

				RtlInitializeCriticalSection = \
					reinterpret_cast<tfRtlInitializeCriticalSection>(\
						::GetProcAddress(m_hNTDLL, "RtlInitializeCriticalSection"));
				RtlInitializeCriticalSectionAndSpinCount = \
					reinterpret_cast<tfRtlInitializeCriticalSectionAndSpinCount>(\
						::GetProcAddress(m_hNTDLL, "RtlInitializeCriticalSectionAndSpinCount"));
				RtlDeleteCriticalSection = \
					reinterpret_cast<tfRtlDeleteCriticalSection>(\
						::GetProcAddress(m_hNTDLL, "RtlDeleteCriticalSection"));
				RtlEnterCriticalSection = \
					reinterpret_cast<tfRtlEnterCriticalSection>(\
						::GetProcAddress(m_hNTDLL, "RtlEnterCriticalSection"));
				RtlLeaveCriticalSection = \
					reinterpret_cast<tfRtlLeaveCriticalSection>(\
						::GetProcAddress(m_hNTDLL, "RtlLeaveCriticalSection"));
				RtlTryEnterCriticalSection = \
					reinterpret_cast<tfRtlTryEnterCriticalSection>(\
						::GetProcAddress(m_hNTDLL, "RtlTryEnterCriticalSection"));
				RtlIsCriticalSectionLocked = \
					reinterpret_cast<tfRtlIsCriticalSectionLocked>(\
						::GetProcAddress(m_hNTDLL, "RtlIsCriticalSectionLocked"));
				RtlIsCriticalSectionLockedByThread = \
					reinterpret_cast<tfRtlIsCriticalSectionLockedByThread>(\
						::GetProcAddress(m_hNTDLL, "RtlIsCriticalSectionLockedByThread"));
				RtlGetCriticalSectionRecursionCount = \
					reinterpret_cast<tfRtlGetCriticalSectionRecursionCount>(\
						::GetProcAddress(m_hNTDLL, "RtlGetCriticalSectionRecursionCount"));
				RtlSetCriticalSectionSpinCount = \
					reinterpret_cast<tfRtlSetCriticalSectionSpinCount>(\
						::GetProcAddress(m_hNTDLL, "RtlSetCriticalSectionSpinCount"));

				if (!RtlInitializeCriticalSection || !RtlSetCriticalSectionSpinCount || !RtlGetCriticalSectionRecursionCount || !RtlIsCriticalSectionLockedByThread \
					|| !RtlIsCriticalSectionLocked || !RtlTryEnterCriticalSection || !RtlLeaveCriticalSection || !RtlEnterCriticalSection || !RtlDeleteCriticalSection || \
					!RtlInitializeCriticalSectionAndSpinCount)
					break;

				NtSuspendProcess = \
					reinterpret_cast<tfNtSuspendProcess>(\
						::GetProcAddress(m_hNTDLL, "NtSuspendProcess"));
				NtResumeProcess = \
					reinterpret_cast<tfNtResumeProcess>(\
						::GetProcAddress(m_hNTDLL, "NtResumeProcess"));

				RtlCreateUserProcess = \
					reinterpret_cast<tfRtlCreateUserProcess>(\
						::GetProcAddress(m_hNTDLL, "RtlCreateUserProcess"));

				if (!NtSuspendProcess || !NtResumeProcess || !RtlCreateUserProcess)
					break;

				RtlGetCurrentPeb = \
					reinterpret_cast<tfRtlGetCurrentPeb>(\
						::GetProcAddress(m_hNTDLL, "RtlGetCurrentPeb"));
				RtlAcquirePebLock = \
					reinterpret_cast<tfRtlAcquirePebLock>(\
						::GetProcAddress(m_hNTDLL, "RtlAcquirePebLock"));
				RtlReleasePebLock = \
					reinterpret_cast<tfRtlReleasePebLock>(\
						::GetProcAddress(m_hNTDLL, "RtlReleasePebLock"));

				if (!RtlGetCurrentPeb || !RtlAcquirePebLock || !RtlReleasePebLock)
					break;

				RtlCreateUnicodeString = \
					reinterpret_cast<tfRtlCreateUnicodeString>(\
						::GetProcAddress(m_hNTDLL, "RtlCreateUnicodeString"));
				RtlInitUnicodeString = \
					reinterpret_cast<tfRtlInitUnicodeString>(\
						::GetProcAddress(m_hNTDLL, "RtlInitUnicodeString"));
				RtlFreeUnicodeString = \
					reinterpret_cast<tfRtlFreeUnicodeString>(\
						::GetProcAddress(m_hNTDLL, "RtlFreeUnicodeString"));
				if (!RtlCreateUnicodeString || !RtlInitUnicodeString || !RtlFreeUnicodeString)
					break;

				NtAllocateVirtualMemory = \
					reinterpret_cast<tfNtAllocateVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtAllocateVirtualMemory"));
				NtFreeVirtualMemory = \
					reinterpret_cast<tfNtFreeVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtFreeVirtualMemory"));
				NtReadVirtualMemory = \
					reinterpret_cast<tfNtReadVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtReadVirtualMemory"));
				NtWriteVirtualMemory = \
					reinterpret_cast<tfNtWriteVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtWriteVirtualMemory"));
				NtProtectVirtualMemory = \
					reinterpret_cast<tfNtProtectVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtProtectVirtualMemory"));
				NtQueryVirtualMemory = \
					reinterpret_cast<tfNtQueryVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtQueryVirtualMemory"));
				NtFlushVirtualMemory = \
					reinterpret_cast<tfNtFlushVirtualMemory>(\
						::GetProcAddress(m_hNTDLL, "NtFlushVirtualMemory"));
				if (!NtAllocateVirtualMemory || !NtFreeVirtualMemory || !NtReadVirtualMemory || !NtWriteVirtualMemory || !NtProtectVirtualMemory || !NtQueryVirtualMemory || !NtFlushVirtualMemory)
					break;

				LdrLoadDll = \
					reinterpret_cast<tfLdrLoadDll>(\
						::GetProcAddress(m_hNTDLL, "LdrLoadDll"));
				LdrUnloadDll = \
					reinterpret_cast<tfLdrUnloadDll>(\
						::GetProcAddress(m_hNTDLL, "LdrUnloadDll"));
				LdrGetDllHandle = \
					reinterpret_cast<tfLdrGetDllHandle>(\
						::GetProcAddress(m_hNTDLL, "LdrGetDllHandle"));

				if (!LdrLoadDll || !LdrUnloadDll || !LdrGetDllHandle)
					break;

				LdrEnumerateLoadedModules = \
					reinterpret_cast<tfLdrEnumerateLoadedModules>(\
						::GetProcAddress(m_hNTDLL, "LdrEnumerateLoadedModules"));
				if (!LdrEnumerateLoadedModules)
					break;

				NtCreateProcessEx = \
					reinterpret_cast<tfNtCreateProcessEx>(\
						::GetProcAddress(m_hNTDLL, "NtCreateProcessEx"));
				if (!NtCreateProcessEx)
					break;

				NtCreateThreadEx = \
					reinterpret_cast<tfNtCreateThreadEx>(\
						::GetProcAddress(m_hNTDLL, "NtCreateThreadEx"));
				if (!NtCreateThreadEx)
					break;

				RtlCreateProcessParametersEx = \
					reinterpret_cast<tfRtlCreateProcessParametersEx>(\
						::GetProcAddress(m_hNTDLL, "RtlCreateProcessParametersEx"));
				if (!RtlCreateProcessParametersEx)
					break;


#if 0//!@ 需要Ring 3 权限
				//!@ Register
				ZwOpenKey = \
					reinterpret_cast<tfZwOpenKey>(\
						::GetProcAddress(m_hNTDLL, "ZwOpenKey"));
				ZwCreateKey = \
					reinterpret_cast<tfZwCreateKey>(\
						::GetProcAddress(m_hNTDLL, "ZwCreateKey"));
				ZwQueryKey = \
					reinterpret_cast<tfZwQueryKey>(\
						::GetProcAddress(m_hNTDLL, "ZwQueryKey"));
				ZwQueryValueKey = \
					reinterpret_cast<tfZwQueryValueKey>(\
						::GetProcAddress(m_hNTDLL, "ZwQueryValueKey"));
				ZwNotifyChangeKey = \
					reinterpret_cast<tfZwNotifyChangeKey>(\
						::GetProcAddress(m_hNTDLL, "ZwNotifyChangeKey"));
				ZwFlushKey = \
					reinterpret_cast<tfZwFlushKey>(\
						::GetProcAddress(m_hNTDLL, "ZwFlushKey"));
				ZwSetValueKey = \
					reinterpret_cast<tfZwSetValueKey>(\
						::GetProcAddress(m_hNTDLL, "ZwSetValueKey"));
				ZwDeleteKey = \
					reinterpret_cast<tfZwDeleteKey>(\
						::GetProcAddress(m_hNTDLL, "ZwDeleteKey"));
				ZwEnumerateKey = \
					reinterpret_cast<tfZwEnumerateKey>(\
						::GetProcAddress(m_hNTDLL, "ZwEnumerateKey"));
				ZwEnumerateValueKey = \
					reinterpret_cast<tfZwEnumerateValueKey>(\
						::GetProcAddress(m_hNTDLL, "ZwEnumerateValueKey"));

				if (!ZwOpenKey || !ZwCreateKey || !ZwQueryKey || !ZwQueryValueKey || !ZwNotifyChangeKey || !ZwFlushKey || !ZwSetValueKey || !ZwDeleteKey || !ZwEnumerateKey || !ZwEnumerateValueKey)
					break;
#endif
#ifdef _DEBUG
				do {

					auto __debug = 0;
				} while (0);

#endif

				RtlGetVersion = \
					reinterpret_cast<tfRtlGetVersion>(\
						::GetProcAddress(m_hNTDLL, "RtlGetVersion"));
				if (!RtlGetVersion)
					break;
				__InitializeWindowsVersion();
				m_Ready.store(true);
				if (m_EnableAutoEnumProcess.load()) {
					m_Threads.emplace_back([this]() {__EnumProcess(); });
				}
			} while (0);
		}
		void NTObj::UnInit() {
			if (!m_Ready.load())
				return;
			m_Ready.store(false);
			if (m_EnableAutoEnumProcess.load()) {
				std::for_each(m_Threads.begin(), m_Threads.end(),
					[](auto& it) {it.join(); });
				m_Threads.clear();
			}
		}

	}///namespace nt
	
}///namespace shared