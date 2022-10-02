#if !defined(INC_H___6EB089E3_A452_4117_8183_B429F7E89058__HEAD__)
#define INC_H___6EB089E3_A452_4117_8183_B429F7E89058__HEAD__

namespace shared {
	namespace nt {

#pragma pack(push,1)
		typedef struct tagProcessRouteData final {
			unsigned long UniqueProcessId;
			unsigned long InheritedFromUniqueProcessId;
			unsigned long long CreateTime;
			unsigned long long SessionId;
			unsigned long long UniqueProcessKey;
			char Signature[_MAX_PATH];
			char ProductName[_MAX_PATH];
			char OriginalFilename[_MAX_PATH];
			char ImageName[_MAX_PATH];
			char ImagePathname[_MAX_PATH];
			char ParentImageName[_MAX_PATH];
			char ParentImagePathname[_MAX_PATH];
			char CommandLine[0x1000];

			tagProcessRouteData() { ::memset(this, 0x00, sizeof(*this)); }
			tagProcessRouteData(const unsigned long& pid, const unsigned long& parent, const unsigned long long& create_time) {
				::memset(this, 0x00, sizeof(*this));
				UniqueProcessId = pid;
				InheritedFromUniqueProcessId = parent;
				CreateTime = create_time;
			}
			void operator=(const tagProcessRouteData& obj) { ::memcpy(this, &obj, sizeof(*this)); }
			bool operator<(const tagProcessRouteData& obj) const {
				return UniqueProcessId < obj.UniqueProcessId;
			}
			bool operator>(const tagProcessRouteData& obj) const {
				return UniqueProcessId > obj.UniqueProcessId;
			}
			bool operator==(const tagProcessRouteData& obj) const {
				return UniqueProcessId == obj.UniqueProcessId;
			}
			bool operator!=(const tagProcessRouteData& obj) const {
				return UniqueProcessId != obj.UniqueProcessId;
			}
		}ProcessRouteData, PROCESSROUTEDATA, * PPROCESSROUTEDATA, SimpleProcessInformation;
#pragma pack(pop)

		class ProcessInfo final {
		public:
			DWORD UniqueProcessId = 0;
			DWORD InheritedFromUniqueProcessId = 0;
			std::uint64_t CreateTime = 0;
			std::string ImageName;
			std::string ImagePathname;
			std::string ParentImageName;
			std::string ParentImagePathname;
			std::string CommandLine;
			std::string Signature;
			std::string Account;
			std::string Domain;
			shared::Win::File::FileVersionInfoA FileVersionInfo;
		public:
			bool operator==(const ProcessInfo& obj) const {
				return UniqueProcessId == obj.UniqueProcessId;
			}
			bool operator!=(const ProcessInfo& obj) const {
				return !(*this == obj);
			}
			bool operator>(const ProcessInfo& obj) const {
				return UniqueProcessId > obj.UniqueProcessId;
			}
			bool operator<(const ProcessInfo& obj) const {
				return UniqueProcessId < obj.UniqueProcessId;
			}
			void operator=(const ProcessInfo& obj) {
				UniqueProcessId = obj.UniqueProcessId;
				InheritedFromUniqueProcessId = obj.InheritedFromUniqueProcessId;
				CreateTime = obj.CreateTime;
				ImageName = obj.ImageName;
				ImagePathname = obj.ImagePathname;
				ParentImageName = obj.ParentImageName;
				ParentImagePathname = obj.ParentImagePathname;
				CommandLine = obj.CommandLine;
				Signature = obj.Signature;
				FileVersionInfo = obj.FileVersionInfo;
				Account = obj.Account;
				Domain = obj.Domain;
			}
		};

		using tf_process_snapshoot = std::map<DWORD, shared::nt::ProcessInfo>;

		class NTObj {
			std::shared_ptr<std::mutex> m_SharedMutex = std::make_shared<std::mutex>();
			std::shared_ptr<std::mutex> m_UniqueMutex = std::make_shared<std::mutex>();
		public:
			class RouteObj final {
				friend class NTObj;
				std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
			public:
				class MemoryModule final {
					std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
					using tfReflectiveLoader = ULONG_PTR(WINAPI*)(ULONG_PTR);
				public:
					MemoryModule(const HANDLE&, const std::string&, const std::string& moduleBuffer, const std::string& resBuffer);
					MemoryModule(const HANDLE&, const std::string&, const std::string& moduleBuffer, const std::map<unsigned long, std::string>& init_data = {});
					~MemoryModule();
				private:
					void Init();
					void UnInit();
				public:
					bool Ready() const;
					const std::string& Identify() const;
					bool Execute(void* param = nullptr) const;
					bool Execute(DWORD& ProcessId, DWORD& ThreadId, void* param = nullptr) const;
				private:
					LPTHREAD_START_ROUTINE m_api_object_init = nullptr;
					LPTHREAD_START_ROUTINE m_api_object_uninit = nullptr;
					LPTHREAD_START_ROUTINE m_api_object_loader = nullptr;
				private:
					const HANDLE m_hProcess;
					const std::string m_Identify;
					const std::string m_ModuleBuffer;
					std::string m_InitDataCache;
					std::atomic_bool m_Ready = false;
					void* m_lpBase = nullptr;
					unsigned int m_ExecuteType = 0;
					std::map<std::string, DWORD> m_ExportFunctionPosQ;
					std::map<std::string, LPTHREAD_START_ROUTINE> m_ExportFunctionQ;
				};
			public:
				RouteObj(const DWORD&, const std::string& Identify, const HANDLE& hProcess);
				~RouteObj();
			public:
				bool Open();
				void Close();
				bool Ready() const;
				const std::string& Identify() const;
				const DWORD& ProcessId() const;
				NTObj::RouteObj::MemoryModule* MemoryModuleCreate(const std::string& processIdenfity, const std::string& injectorCache, const std::string& bindRespak);
				NTObj::RouteObj::MemoryModule* MemoryModuleCreate(const std::string&, const std::string&, const std::map<unsigned long, std::string>& mmInitDatas = {});
				bool MemoryModuleDestory(NTObj::RouteObj::MemoryModule*);
			private:
				HANDLE& handle();
				std::atomic_bool m_IsOpen = false;
				const std::string m_Identify;
				HANDLE m_hProcess = nullptr;
				DWORD m_ProcessId = 0;
				std::map<std::string/*ModIdentify*/, std::shared_ptr<MemoryModule>> m_pMemoryModuleObjs;
			};
		public:
			//!@ Creating a injected object
			RouteObj* RouteObjCreate(const std::string&, const std::function<void(const std::string&)>& errorcb = nullptr);
			RouteObj* RouteObjCreate(const DWORD&, const std::function<void(const std::string&)>& errorcb = nullptr);
			//!@ ***Use with caution
			RouteObj* RouteObjCreate(const HANDLE&, const DWORD&);
			RouteObj* RouteObjCreate(const DWORD&);
			//!@ Destroy the injected object
			bool RouteObjDestory(const std::string&);
			bool RouteObjDestory(const DWORD&);
			bool RouteObjDestory(RouteObj*);
			//!@ Create Injector
			//! 
			//! 
			static bool CreateThreadContext(const std::string& processPathname,
				const std::string& commandline,const std::string& peBuffer, const std::string& routePak,
				const bool& Inheriting_parent_process = true
				);
			static bool CreateThreadContextByToken(const std::string& processPathname,
				const std::string& commandline, const std::string& peBuffer, const std::string& routePak,
				const HANDLE& hByToken
			);
			static bool CreateShellCode(const std::string& pe_buffer, const std::string& init_function, const std::string& user_data, std::string& shellcode);
		private:
			container::map<std::string, std::shared_ptr<RouteObj>> m_RouteObjQ;
			container::map<DWORD, std::shared_ptr<RouteObj>> m_IDRouteObjQ;
		public:
			tfNtClose NtClose = nullptr;
			tfNtOpenProcess NtOpenProcess = nullptr;
			tfNtTerminateProcess NtTerminateProcess = nullptr;
			tfNtQuerySystemInformation NtQuerySystemInformation = nullptr;
			tfNtQueryInformationProcess NtQueryInformationProcess = nullptr;
			tfRtlInitializeCriticalSection RtlInitializeCriticalSection = nullptr;
			tfRtlInitializeCriticalSectionAndSpinCount  RtlInitializeCriticalSectionAndSpinCount = nullptr;
			tfRtlDeleteCriticalSection  RtlDeleteCriticalSection = nullptr;
			tfRtlEnterCriticalSection  RtlEnterCriticalSection = nullptr;
			tfRtlLeaveCriticalSection  RtlLeaveCriticalSection = nullptr;
			tfRtlTryEnterCriticalSection  RtlTryEnterCriticalSection = nullptr;
			tfRtlIsCriticalSectionLocked  RtlIsCriticalSectionLocked = nullptr;
			tfRtlIsCriticalSectionLockedByThread  RtlIsCriticalSectionLockedByThread = nullptr;
			tfRtlGetCriticalSectionRecursionCount  RtlGetCriticalSectionRecursionCount = nullptr;
			tfRtlSetCriticalSectionSpinCount  RtlSetCriticalSectionSpinCount = nullptr;
			tfNtSuspendProcess NtSuspendProcess = nullptr;
			tfNtResumeProcess NtResumeProcess = nullptr;
			tfRtlCreateUserProcess RtlCreateUserProcess = nullptr;
			tfRtlGetCurrentPeb RtlGetCurrentPeb = nullptr;
			tfRtlAcquirePebLock RtlAcquirePebLock = nullptr;
			tfRtlReleasePebLock RtlReleasePebLock = nullptr;
			tfRtlGetVersion RtlGetVersion = nullptr;
			tfRtlCreateUnicodeString  RtlCreateUnicodeString = nullptr;
			tfRtlInitUnicodeString RtlInitUnicodeString = nullptr;
			tfRtlFreeUnicodeString  RtlFreeUnicodeString = nullptr;
			tfNtAllocateVirtualMemory NtAllocateVirtualMemory = nullptr;
			tfNtFreeVirtualMemory NtFreeVirtualMemory = nullptr;
			tfNtReadVirtualMemory NtReadVirtualMemory = nullptr;
			tfNtWriteVirtualMemory NtWriteVirtualMemory = nullptr;
			tfNtProtectVirtualMemory NtProtectVirtualMemory = nullptr;
			tfNtQueryVirtualMemory NtQueryVirtualMemory = nullptr;
			tfNtFlushVirtualMemory NtFlushVirtualMemory = nullptr;
			tfLdrLoadDll LdrLoadDll = nullptr;
			tfLdrUnloadDll LdrUnloadDll = nullptr;
			tfLdrGetDllHandle LdrGetDllHandle = nullptr;
			tfLdrEnumerateLoadedModules LdrEnumerateLoadedModules = nullptr;
			tfNtCreateToken NtCreateToken = nullptr;
			tfNtOpenProcessToken NtOpenProcessToken = nullptr;
			tfNtOpenProcessTokenEx NtOpenProcessTokenEx = nullptr;
			tfNtOpenThreadToken NtOpenThreadToken = nullptr;
			tfNtOpenThreadTokenEx NtOpenThreadTokenEx = nullptr;
			tfNtDuplicateToken NtDuplicateToken = nullptr;
			tfNtQueryInformationToken NtQueryInformationToken = nullptr;
			tfNtSetInformationToken NtSetInformationToken = nullptr;
			tfNtAdjustPrivilegesToken NtAdjustPrivilegesToken = nullptr;
			tfNtAdjustGroupsToken NtAdjustGroupsToken = nullptr;
			tfNtCreateProcessEx NtCreateProcessEx = nullptr;
			tfNtCreateThreadEx NtCreateThreadEx = nullptr;
			tfRtlCreateProcessParametersEx RtlCreateProcessParametersEx = nullptr;
#if 0
			tfZwOpenKey ZwOpenKey = nullptr;
			tfZwCreateKey ZwCreateKey = nullptr;
			tfZwQueryKey ZwQueryKey = nullptr;
			tfZwQueryValueKey ZwQueryValueKey = nullptr;
			tfZwNotifyChangeKey ZwNotifyChangeKey = nullptr;
			tfZwFlushKey ZwFlushKey = nullptr;
			tfZwSetValueKey ZwSetValueKey = nullptr;
			tfZwDeleteKey ZwDeleteKey = nullptr;
			tfZwEnumerateKey ZwEnumerateKey = nullptr;
			tfZwEnumerateValueKey ZwEnumerateValueKey = nullptr;
#endif
		private:
			container::queue<std::tuple<time_t, std::string>> m_RuntimelogQ;
			std::atomic<DWORD> m_CurrentProcessId = 0;
			std::atomic<HANDLE> m_CurrentProcessHandle = nullptr;
			RTL_OSVERSIONINFOEXW m_OsVersion;
			std::atomic_uint64_t m_WindowsVersion = 0;
			std::string m_OsName;
			EnWindowsVersion m_WindowsVersionType = EnWindowsVersion::EN_WINDOWS_ANCIENT;
			std::atomic_bool m_Ready = false;
			HMODULE m_hNTDLL = nullptr;
			HMODULE m_hKERNEL32 = nullptr;
			std::atomic_int64_t m_ProcessEnumIntervalMS = 500;
			container::map<DWORD, SYSTEM_PROCESS_INFORMATION> m_ProcessSnapshoot;//!@ Realtime process cache
			container::map<DWORD, PROCESSROUTEDATA> m_ProcessSnapshootSimple;
			std::function<void(
				const std::map<DWORD, PROCESSROUTEDATA>& creations,
				const std::map<DWORD, PROCESSROUTEDATA>& deletions,
				const std::map<DWORD, PROCESSROUTEDATA>& snapshoot)>
				m_PrivateEnumCb = nullptr;
			std::vector<std::thread> m_Threads;
			std::atomic_bool m_EnableAutoEnumProcess = false;
		public:
			NTObj(const bool& EnableAutoEnumProcess = false, const std::int64_t& ProcessEnumIntervalMS = 500);
			~NTObj();
		public:
			bool NtCreateProcess(HANDLE&, DWORD&, const std::wstring&, const std::wstring&,const std::string&) const;
			EnWindowsVersion WindowsVersionTypeGet() const;
			void UpdateProcessCache();
			std::string ProcessImageNameSnapshot() const;
			void PopRuntimelogs(const std::function<void(const std::vector<std::tuple<time_t, std::string>>&)>&);
			HANDLE GetCurrentHandle() const;
			DWORD GetCurrentProcessId() const;
			const std::string& OsName() const;
			std::uint64_t WindowsVersion() const;
			bool PrivateEnumCb() const;
			void PrivateEnumCb(
				const std::function<void(
					const std::map<DWORD, PROCESSROUTEDATA>& creations, \
					const std::map<DWORD, PROCESSROUTEDATA>& deletions, \
					const std::map<DWORD, PROCESSROUTEDATA>& snapshoot)>& cb);
			bool Ready() const;
			std::vector<SYSTEM_PROCESS_INFORMATION> ProcessSnapshoot() const;
			container::map<DWORD, PROCESSROUTEDATA> ProcessSnapshootSimple() const;
			bool ProcessIDGet(const std::string& processName, const std::function<void(const std::set<DWORD>&)>& rescb) const;
			bool ProcessInformationGet(const std::string& processImageName, const std::function<void(const std::set<PROCESSROUTEDATA>&, const std::set<PROCESSROUTEDATA>&)>&) const;
			bool ProcessInformationSet(
				_In_ const DWORD& ProcessId,
				_In_ const std::function<void(const HANDLE&, const PROCESS_BASIC_INFORMATION&, const RTL_USER_PROCESS_PARAMETERS&, const PEB&)>& rescb
			) const;
			bool ProcessInformationGet(
				_In_ const DWORD& inPid,
				_In_ const std::function<void(const HANDLE&, const PROCESS_BASIC_INFORMATION&, const RTL_USER_PROCESS_PARAMETERS&, const PEB&)>& rescb
			) const;
			bool TerminateProcess(const DWORD& ProcessId) const;
			bool ProcessSuspend(const DWORD& ProcessId) const;
			bool ProcessResume(const DWORD& ProcessId) const;
			bool GetCurrentPeb(const std::function<void(PPEB)>&);
			bool MasqueradeProcess(const DWORD& ProcessId, const std::string& ImageName, const std::string& ImagePathname, const std::string& CommandLine) const;
			bool OpenProcessWithToken(const std::string& tokenHost, \
				const std::string& openProcessPathname, \
				const std::string& starupCommandLine, \
				const DWORD& dwCreateFlag = CREATE_NEW_CONSOLE,
				const time_t& wait = INFINITE, \
				const bool& show = true) const;
			void EnumProcessOnce(_Inout_ time_t&);
			void EnumProcessOnce(const std::function<void(
				const std::map<DWORD, ProcessInfo>& creations, \
				const std::map<DWORD, ProcessInfo>& deletions, \
				const std::map<DWORD, ProcessInfo>& snapshoot)>&,
				const bool& bPatchProcessInfo = false,
				const bool& bPatchVersionInfo = false,
				const bool& bPatchParentProcessInfo = false,
				const bool& bPatchAccountSid = false,
				const std::uint64_t& pause_time_ms = 0
			);
			bool EnumProcessModuleInformation(
				const DWORD& ProcessId,
				const std::function<void(const HANDLE& ProcessHandle, PLDR_DATA_TABLE_ENTRY Entry, PVOID AddressOfEntry, PPEB, bool& itbreak)>& Callback
			) const;
		private:
			std::atomic_uint64_t m_EnumProcessOnceLastCreateTimeBak = 0;
			std::map<DWORD, ProcessInfo> m_EnumProcessOncePrivateSnapshootBak;
		public:
			static DWORD RVAToOffset(const DWORD&, const UINT_PTR&);
			static bool EnumImageExportDirectory(const void*, std::map<std::string, DWORD>&);
		private:
			void __InitializeWindowsVersion();
			bool __ProcessInformationGet
			(
				_In_ const DWORD& inPid,
				_Out_ PROCESS_BASIC_INFORMATION& outProcessBasicInformation,
				_Out_ RTL_USER_PROCESS_PARAMETERS& outRtlUserProcessParameters,
				_Out_ PEB& outPeb
			) const;
			bool __ProcessInformationGet
			(
				_In_ const DWORD& inPid,
				_In_ const std::function<void(
					_Out_ const HANDLE&,
					_Out_ const PROCESS_BASIC_INFORMATION&,
					_Out_ const RTL_USER_PROCESS_PARAMETERS&,
					_Out_ const PEB&)>&
			) const;

			HANDLE __OpenPorcess(const DWORD& ProcessId, const ACCESS_MASK& accessMask = PROCESS_ALL_ACCESS) const;
			bool __GetProcessBasicInformation(const HANDLE& hProcess, PROCESS_BASIC_INFORMATION&) const;
			bool __EnumProcessModules(
				const DWORD& ProcessId,
				const std::function<void(const HANDLE& ProcessHandle, PLDR_DATA_TABLE_ENTRY Entry, PVOID AddressOfEntry, PPEB, bool& itbreak)>& Callback
			) const;
			std::string __CommandLineGet(
				const HANDLE& hProcess,
				const RTL_USER_PROCESS_PARAMETERS& RtlUserProcessParameters,
				const std::function<void(const std::string&)>& rescb) const;
			std::string __ImagePathNameGet(
				const HANDLE& hProcess,
				const RTL_USER_PROCESS_PARAMETERS& RtlUserProcessParameters,
				const std::function<void(const std::string&)>& rescb) const;
			//!@ Private
			void __EnumProcess();
			void __EnumProcessOnce(_Inout_ time_t&);
			bool __EnumProcess(const std::function<void(const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>&)>&) const;
			//!@ CPU usage is low 15ms~30ms
			bool __EnumProcess(std::map<DWORD, SYSTEM_PROCESS_INFORMATION>& outProcesss) const;
			bool __EnumProcess(std::map<DWORD, PROCESSROUTEDATA>& outProcesss) const;
			bool __EnumProcess(//!@ The Private method
				const std::uint64_t& max_process_create_time_prev,
				const std::function<void(
					const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>&,
					const std::map<DWORD, SYSTEM_PROCESS_INFORMATION>&, const std::uint64_t&)>& rescb,
				const std::uint64_t& pause_time_ms = 0);
			bool __ElevatedPrivileges() const;
			bool __ProcessGet(const std::string&, const std::function<void(const std::vector<PROCESSROUTEDATA>&)>&) const;
			bool __ProcessGet(const DWORD&, const std::function<void(const PROCESSROUTEDATA&)>&) const;
		private:
			void Init();
			void UnInit();
		};


	}///namespace nt
}///namespace shared

#pragma comment(lib,"ntobj")
/// /*新生®（上海）**/
/// /*2022_02_19T12:37:56.0253139Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___6EB089E3_A452_4117_8183_B429F7E89058__HEAD__
