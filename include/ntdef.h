#if !defined(INC_H___8490570A_162F_4486_875D_C86829FB75D8__HEAD__)
#define INC_H___8490570A_162F_4486_875D_C86829FB75D8__HEAD__

namespace shared {
	namespace nt {

		using NTSTATUS = LONG;
		//!@----------------------------------------------------------------------------------------------------------------------
		typedef LONG KPRIORITY;
#define GDI_HANDLE_BUFFER_SIZE32 34
#define GDI_HANDLE_BUFFER_SIZE64 60

#ifndef _WIN64
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE64
#endif

		typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];

		typedef ULONG GDI_HANDLE_BUFFER32[GDI_HANDLE_BUFFER_SIZE32];
		typedef ULONG GDI_HANDLE_BUFFER64[GDI_HANDLE_BUFFER_SIZE64];
		typedef ULONG LOGICAL;
		typedef ULONG* PLOGICAL;

		const TCHAR SePrivileges[][0x100] =
		{
		TEXT("SeCreateTokenPrivilege"),
		TEXT("SeAssignPrimaryTokenPrivilege"),
		TEXT("SeLockMemoryPrivilege"),
		TEXT("SeIncreaseQuotaPrivilege"),
		TEXT("SeUnsolicitedInputPrivilege"),
		TEXT("SeMachineAccountPrivilege"),
		TEXT("SeTcbPrivilege"),
		TEXT("SeSecurityPrivilege"),
		TEXT("SeTakeOwnershipPrivilege"),
		TEXT("SeLoadDriverPrivilege"),
		TEXT("SeSystemProfilePrivilege"),
		TEXT("SeSystemtimePrivilege"),
		TEXT("SeProfileSingleProcessPrivilege"),
		TEXT("SeIncreaseBasePriorityPrivilege"),
		TEXT("SeCreatePagefilePrivilege"),
		TEXT("SeCreatePermanentPrivilege"),
		TEXT("SeBackupPrivilege"),
		TEXT("SeRestorePrivilege"),
		TEXT("SeShutdownPrivilege"),
		TEXT("SeDebugPrivilege"),
		TEXT("SeAuditPrivilege"),
		TEXT("SeSystemEnvironmentPrivilege"),
		TEXT("SeChangeNotifyPrivilege"),
		TEXT("SeRemoteShutdownPrivilege"),
		TEXT("SeUndockPrivilege"),
		TEXT("SeSyncAgentPrivilege"),
		TEXT("SeEnableDelegationPrivilege"),
		TEXT("SeManageVolumePrivilege"),
		TEXT("SeImpersonatePrivilege"),
		TEXT("SeCreateGlobalPrivilege"),
		TEXT("SeTrustedCredManAccessPrivilege"),
		TEXT("SeRelabelPrivilege"),
		TEXT("SeIncreaseWorkingSetPrivilege"),
		TEXT("SeTimeZonePrivilege"),
		TEXT("SeCreateSymbolicLinkPrivilege"),
		TEXT("SeDelegateSessionUserImpersonatePrivilege"),
		};
		//!@----------------------------------------------------------------------------------------------------------------------
		typedef enum _KEY_VALUE_INFORMATION_CLASS {
			KeyValueBasicInformation,
			KeyValueFullInformation,
			KeyValuePartialInformation,
			KeyValueFullInformationAlign64,
			KeyValuePartialInformationAlign64,
			KeyValueLayerInformation,
			MaxKeyValueInfoClass
		} KEY_VALUE_INFORMATION_CLASS;

		typedef enum _KEY_INFORMATION_CLASS {
			KeyBasicInformation,
			KeyNodeInformation,
			KeyFullInformation,
			KeyNameInformation,
			KeyCachedInformation,
			KeyFlagsInformation,
			KeyVirtualizationInformation,
			KeyHandleTagsInformation,
			KeyTrustInformation,
			KeyLayerInformation,
			MaxKeyInfoClass
		} KEY_INFORMATION_CLASS;

		typedef enum _KTHREAD_STATE
		{
			Initialized,
			Ready,
			Running,
			Standby,
			Terminated,
			Waiting,
			Transition,
			DeferredReady,
			GateWaitObsolete,
			WaitingForProcessInSwap,
			MaximumThreadState
		} KTHREAD_STATE, * PKTHREAD_STATE;

		typedef enum _KWAIT_REASON
		{
			Executive,
			FreePage,
			PageIn,
			PoolAllocation,
			DelayExecution,
			Suspended,
			UserRequest,
			WrExecutive,
			WrFreePage,
			WrPageIn,
			WrPoolAllocation,
			WrDelayExecution,
			WrSuspended,
			WrUserRequest,
			WrEventPair,
			WrQueue,
			WrLpcReceive,
			WrLpcReply,
			WrVirtualMemory,
			WrPageOut,
			WrRendezvous,
			WrKeyedEvent,
			WrTerminated,
			WrProcessInSwap,
			WrCpuRateControl,
			WrCalloutStack,
			WrKernel,
			WrResource,
			WrPushLock,
			WrMutex,
			WrQuantumEnd,
			WrDispatchInt,
			WrPreempted,
			WrYieldExecution,
			WrFastMutex,
			WrGuardedMutex,
			WrRundown,
			WrAlertByThreadId,
			WrDeferredPreempt,
			WrPhysicalFault,
			MaximumWaitReason
		} KWAIT_REASON, * PKWAIT_REASON;
		// rev
	// private
		typedef enum _SYSTEM_INFORMATION_CLASS
		{
			SystemBasicInformation, // q: SYSTEM_BASIC_INFORMATION
			SystemProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
			SystemPerformanceInformation, // q: SYSTEM_PERFORMANCE_INFORMATION
			SystemTimeOfDayInformation, // q: SYSTEM_TIMEOFDAY_INFORMATION
			SystemPathInformation, // not implemented
			SystemProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
			SystemCallCountInformation, // q: SYSTEM_CALL_COUNT_INFORMATION
			SystemDeviceInformation, // q: SYSTEM_DEVICE_INFORMATION
			SystemProcessorPerformanceInformation, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
			SystemFlagsInformation, // q: SYSTEM_FLAGS_INFORMATION
			SystemCallTimeInformation, // not implemented // SYSTEM_CALL_TIME_INFORMATION // 10
			SystemModuleInformation, // q: RTL_PROCESS_MODULES
			SystemLocksInformation, // q: RTL_PROCESS_LOCKS
			SystemStackTraceInformation, // q: RTL_PROCESS_BACKTRACES
			SystemPagedPoolInformation, // not implemented
			SystemNonPagedPoolInformation, // not implemented
			SystemHandleInformation, // q: SYSTEM_HANDLE_INFORMATION
			SystemObjectInformation, // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
			SystemPageFileInformation, // q: SYSTEM_PAGEFILE_INFORMATION
			SystemVdmInstemulInformation, // q: SYSTEM_VDM_INSTEMUL_INFO
			SystemVdmBopInformation, // not implemented // 20
			SystemFileCacheInformation, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
			SystemPoolTagInformation, // q: SYSTEM_POOLTAG_INFORMATION
			SystemInterruptInformation, // q: SYSTEM_INTERRUPT_INFORMATION
			SystemDpcBehaviorInformation, // q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
			SystemFullMemoryInformation, // not implemented // SYSTEM_MEMORY_USAGE_INFORMATION
			SystemLoadGdiDriverInformation, // s (kernel-mode only)
			SystemUnloadGdiDriverInformation, // s (kernel-mode only)
			SystemTimeAdjustmentInformation, // q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
			SystemSummaryMemoryInformation, // not implemented // SYSTEM_MEMORY_USAGE_INFORMATION
			SystemMirrorMemoryInformation, // s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege) // 30
			SystemPerformanceTraceInformation, // q; s: (type depends on EVENT_TRACE_INFORMATION_CLASS)
			SystemObsolete0, // not implemented
			SystemExceptionInformation, // q: SYSTEM_EXCEPTION_INFORMATION
			SystemCrashDumpStateInformation, // s: SYSTEM_CRASH_DUMP_STATE_INFORMATION (requires SeDebugPrivilege)
			SystemKernelDebuggerInformation, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
			SystemContextSwitchInformation, // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
			SystemRegistryQuotaInformation, // q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
			SystemExtendServiceTableInformation, // s (requires SeLoadDriverPrivilege) // loads win32k only
			SystemPrioritySeperation, // s (requires SeTcbPrivilege)
			SystemVerifierAddDriverInformation, // s (requires SeDebugPrivilege) // 40
			SystemVerifierRemoveDriverInformation, // s (requires SeDebugPrivilege)
			SystemProcessorIdleInformation, // q: SYSTEM_PROCESSOR_IDLE_INFORMATION
			SystemLegacyDriverInformation, // q: SYSTEM_LEGACY_DRIVER_INFORMATION
			SystemCurrentTimeZoneInformation, // q; s: RTL_TIME_ZONE_INFORMATION
			SystemLookasideInformation, // q: SYSTEM_LOOKASIDE_INFORMATION
			SystemTimeSlipNotification, // s: HANDLE (NtCreateEvent) (requires SeSystemtimePrivilege)
			SystemSessionCreate, // not implemented
			SystemSessionDetach, // not implemented
			SystemSessionInformation, // not implemented (SYSTEM_SESSION_INFORMATION)
			SystemRangeStartInformation, // q: SYSTEM_RANGE_START_INFORMATION // 50
			SystemVerifierInformation, // q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
			SystemVerifierThunkExtend, // s (kernel-mode only)
			SystemSessionProcessInformation, // q: SYSTEM_SESSION_PROCESS_INFORMATION
			SystemLoadGdiDriverInSystemSpace, // s: SYSTEM_GDI_DRIVER_INFORMATION (kernel-mode only) (same as SystemLoadGdiDriverInformation)
			SystemNumaProcessorMap, // q: SYSTEM_NUMA_INFORMATION
			SystemPrefetcherInformation, // q; s: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
			SystemExtendedProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
			SystemRecommendedSharedDataAlignment, // q: ULONG // KeGetRecommendedSharedDataAlignment
			SystemComPlusPackage, // q; s: ULONG
			SystemNumaAvailableMemory, // q: SYSTEM_NUMA_INFORMATION // 60
			SystemProcessorPowerInformation, // q: SYSTEM_PROCESSOR_POWER_INFORMATION
			SystemEmulationBasicInformation, // q: SYSTEM_BASIC_INFORMATION
			SystemEmulationProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
			SystemExtendedHandleInformation, // q: SYSTEM_HANDLE_INFORMATION_EX
			SystemLostDelayedWriteInformation, // q: ULONG
			SystemBigPoolInformation, // q: SYSTEM_BIGPOOL_INFORMATION
			SystemSessionPoolTagInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION
			SystemSessionMappedViewInformation, // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
			SystemHotpatchInformation, // q; s: SYSTEM_HOTPATCH_CODE_INFORMATION
			SystemObjectSecurityMode, // q: ULONG // 70
			SystemWatchdogTimerHandler, // s: SYSTEM_WATCHDOG_HANDLER_INFORMATION // (kernel-mode only)
			SystemWatchdogTimerInformation, // q: SYSTEM_WATCHDOG_TIMER_INFORMATION // (kernel-mode only)
			SystemLogicalProcessorInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION (EX in: USHORT ProcessorGroup)
			SystemWow64SharedInformationObsolete, // not implemented
			SystemRegisterFirmwareTableInformationHandler, // s: SYSTEM_FIRMWARE_TABLE_HANDLER // (kernel-mode only)
			SystemFirmwareTableInformation, // SYSTEM_FIRMWARE_TABLE_INFORMATION
			SystemModuleInformationEx, // q: RTL_PROCESS_MODULE_INFORMATION_EX
			SystemVerifierTriageInformation, // not implemented
			SystemSuperfetchInformation, // q; s: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
			SystemMemoryListInformation, // q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege) // 80
			SystemFileCacheInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
			SystemThreadPriorityClientIdInformation, // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)
			SystemProcessorIdleCycleTimeInformation, // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[] (EX in: USHORT ProcessorGroup)
			SystemVerifierCancellationInformation, // SYSTEM_VERIFIER_CANCELLATION_INFORMATION // name:wow64:whNT32QuerySystemVerifierCancellationInformation
			SystemProcessorPowerInformationEx, // not implemented
			SystemRefTraceInformation, // q; s: SYSTEM_REF_TRACE_INFORMATION // ObQueryRefTraceInformation
			SystemSpecialPoolInformation, // q; s: SYSTEM_SPECIAL_POOL_INFORMATION (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
			SystemProcessIdInformation, // q: SYSTEM_PROCESS_ID_INFORMATION
			SystemErrorPortInformation, // s (requires SeTcbPrivilege)
			SystemBootEnvironmentInformation, // q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION // 90
			SystemHypervisorInformation, // q: SYSTEM_HYPERVISOR_QUERY_INFORMATION
			SystemVerifierInformationEx, // q; s: SYSTEM_VERIFIER_INFORMATION_EX
			SystemTimeZoneInformation, // q; s: RTL_TIME_ZONE_INFORMATION (requires SeTimeZonePrivilege)
			SystemImageFileExecutionOptionsInformation, // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
			SystemCoverageInformation, // q: COVERAGE_MODULES s: COVERAGE_MODULE_REQUEST // ExpCovQueryInformation (requires SeDebugPrivilege)
			SystemPrefetchPatchInformation, // SYSTEM_PREFETCH_PATCH_INFORMATION
			SystemVerifierFaultsInformation, // s: SYSTEM_VERIFIER_FAULTS_INFORMATION (requires SeDebugPrivilege)
			SystemSystemPartitionInformation, // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
			SystemSystemDiskInformation, // q: SYSTEM_SYSTEM_DISK_INFORMATION
			SystemProcessorPerformanceDistribution, // q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION // 100
			SystemNumaProximityNodeInformation, // q; s: SYSTEM_NUMA_PROXIMITY_MAP
			SystemDynamicTimeZoneInformation, // q; s: RTL_DYNAMIC_TIME_ZONE_INFORMATION (requires SeTimeZonePrivilege)
			SystemCodeIntegrityInformation, // q: SYSTEM_CODEINTEGRITY_INFORMATION // SeCodeIntegrityQueryInformation
			SystemProcessorMicrocodeUpdateInformation, // s: SYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION
			SystemProcessorBrandString, // q: CHAR[] // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
			SystemVirtualAddressInformation, // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
			SystemLogicalProcessorAndGroupInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // since WIN7 // KeQueryLogicalProcessorRelationship
			SystemProcessorCycleTimeInformation, // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[] (EX in: USHORT ProcessorGroup)
			SystemStoreInformation, // q; s: SYSTEM_STORE_INFORMATION (requires SeProfileSingleProcessPrivilege) // SmQueryStoreInformation
			SystemRegistryAppendString, // s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS // 110
			SystemAitSamplingValue, // s: ULONG (requires SeProfileSingleProcessPrivilege)
			SystemVhdBootInformation, // q: SYSTEM_VHD_BOOT_INFORMATION
			SystemCpuQuotaInformation, // q; s: PS_CPU_QUOTA_QUERY_INFORMATION
			SystemNativeBasicInformation, // q: SYSTEM_BASIC_INFORMATION
			SystemErrorPortTimeouts, // SYSTEM_ERROR_PORT_TIMEOUTS
			SystemLowPriorityIoInformation, // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
			SystemTpmBootEntropyInformation, // q: TPM_BOOT_ENTROPY_NT_RESULT // ExQueryTpmBootEntropyInformation
			SystemVerifierCountersInformation, // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
			SystemPagedPoolInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
			SystemSystemPtesInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes) // 120
			SystemNodeDistanceInformation,
			SystemAcpiAuditInformation, // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
			SystemBasicPerformanceInformation, // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
			SystemQueryPerformanceCounterInformation, // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
			SystemSessionBigPoolInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION // since WIN8
			SystemBootGraphicsInformation, // q; s: SYSTEM_BOOT_GRAPHICS_INFORMATION (kernel-mode only)
			SystemScrubPhysicalMemoryInformation, // q; s: MEMORY_SCRUB_INFORMATION
			SystemBadPageInformation,
			SystemProcessorProfileControlArea, // q; s: SYSTEM_PROCESSOR_PROFILE_CONTROL_AREA
			SystemCombinePhysicalMemoryInformation, // s: MEMORY_COMBINE_INFORMATION, MEMORY_COMBINE_INFORMATION_EX, MEMORY_COMBINE_INFORMATION_EX2 // 130
			SystemEntropyInterruptTimingInformation, // q; s: SYSTEM_ENTROPY_TIMING_INFORMATION
			SystemConsoleInformation, // q: SYSTEM_CONSOLE_INFORMATION
			SystemPlatformBinaryInformation, // q: SYSTEM_PLATFORM_BINARY_INFORMATION (requires SeTcbPrivilege)
			SystemPolicyInformation, // q: SYSTEM_POLICY_INFORMATION
			SystemHypervisorProcessorCountInformation, // q: SYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION
			SystemDeviceDataInformation, // q: SYSTEM_DEVICE_DATA_INFORMATION
			SystemDeviceDataEnumerationInformation, // q: SYSTEM_DEVICE_DATA_INFORMATION
			SystemMemoryTopologyInformation, // q: SYSTEM_MEMORY_TOPOLOGY_INFORMATION
			SystemMemoryChannelInformation, // q: SYSTEM_MEMORY_CHANNEL_INFORMATION
			SystemBootLogoInformation, // q: SYSTEM_BOOT_LOGO_INFORMATION // 140
			SystemProcessorPerformanceInformationEx, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX // since WINBLUE
			SystemCriticalProcessErrorLogInformation,
			SystemSecureBootPolicyInformation, // q: SYSTEM_SECUREBOOT_POLICY_INFORMATION
			SystemPageFileInformationEx, // q: SYSTEM_PAGEFILE_INFORMATION_EX
			SystemSecureBootInformation, // q: SYSTEM_SECUREBOOT_INFORMATION
			SystemEntropyInterruptTimingRawInformation,
			SystemPortableWorkspaceEfiLauncherInformation, // q: SYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION
			SystemFullProcessInformation, // q: SYSTEM_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
			SystemKernelDebuggerInformationEx, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
			SystemBootMetadataInformation, // 150
			SystemSoftRebootInformation, // q: ULONG
			SystemElamCertificateInformation, // s: SYSTEM_ELAM_CERTIFICATE_INFORMATION
			SystemOfflineDumpConfigInformation, // q: OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V2
			SystemProcessorFeaturesInformation, // q: SYSTEM_PROCESSOR_FEATURES_INFORMATION
			SystemRegistryReconciliationInformation, // s: NULL (requires admin) (flushes registry hives)
			SystemEdidInformation, // q: SYSTEM_EDID_INFORMATION
			SystemManufacturingInformation, // q: SYSTEM_MANUFACTURING_INFORMATION // since THRESHOLD
			SystemEnergyEstimationConfigInformation, // q: SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
			SystemHypervisorDetailInformation, // q: SYSTEM_HYPERVISOR_DETAIL_INFORMATION
			SystemProcessorCycleStatsInformation, // q: SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION // 160
			SystemVmGenerationCountInformation,
			SystemTrustedPlatformModuleInformation, // q: SYSTEM_TPM_INFORMATION
			SystemKernelDebuggerFlags, // SYSTEM_KERNEL_DEBUGGER_FLAGS
			SystemCodeIntegrityPolicyInformation, // q: SYSTEM_CODEINTEGRITYPOLICY_INFORMATION
			SystemIsolatedUserModeInformation, // q: SYSTEM_ISOLATED_USER_MODE_INFORMATION
			SystemHardwareSecurityTestInterfaceResultsInformation,
			SystemSingleModuleInformation, // q: SYSTEM_SINGLE_MODULE_INFORMATION
			SystemAllowedCpuSetsInformation,
			SystemVsmProtectionInformation, // q: SYSTEM_VSM_PROTECTION_INFORMATION (previously SystemDmaProtectionInformation)
			SystemInterruptCpuSetsInformation, // q: SYSTEM_INTERRUPT_CPU_SET_INFORMATION // 170
			SystemSecureBootPolicyFullInformation, // q: SYSTEM_SECUREBOOT_POLICY_FULL_INFORMATION
			SystemCodeIntegrityPolicyFullInformation,
			SystemAffinitizedInterruptProcessorInformation, // (requires SeIncreaseBasePriorityPrivilege)
			SystemRootSiloInformation, // q: SYSTEM_ROOT_SILO_INFORMATION
			SystemCpuSetInformation, // q: SYSTEM_CPU_SET_INFORMATION // since THRESHOLD2
			SystemCpuSetTagInformation, // q: SYSTEM_CPU_SET_TAG_INFORMATION
			SystemWin32WerStartCallout,
			SystemSecureKernelProfileInformation, // q: SYSTEM_SECURE_KERNEL_HYPERGUARD_PROFILE_INFORMATION
			SystemCodeIntegrityPlatformManifestInformation, // q: SYSTEM_SECUREBOOT_PLATFORM_MANIFEST_INFORMATION // since REDSTONE
			SystemInterruptSteeringInformation, // SYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT // 180
			SystemSupportedProcessorArchitectures, // p: in opt: HANDLE, out: SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION[] // NtQuerySystemInformationEx
			SystemMemoryUsageInformation, // q: SYSTEM_MEMORY_USAGE_INFORMATION
			SystemCodeIntegrityCertificateInformation, // q: SYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION
			SystemPhysicalMemoryInformation, // q: SYSTEM_PHYSICAL_MEMORY_INFORMATION // since REDSTONE2
			SystemControlFlowTransition,
			SystemKernelDebuggingAllowed, // s: ULONG
			SystemActivityModerationExeState, // SYSTEM_ACTIVITY_MODERATION_EXE_STATE
			SystemActivityModerationUserSettings, // SYSTEM_ACTIVITY_MODERATION_USER_SETTINGS
			SystemCodeIntegrityPoliciesFullInformation,
			SystemCodeIntegrityUnlockInformation, // SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION // 190
			SystemIntegrityQuotaInformation,
			SystemFlushInformation, // q: SYSTEM_FLUSH_INFORMATION
			SystemProcessorIdleMaskInformation, // q: ULONG_PTR // since REDSTONE3
			SystemSecureDumpEncryptionInformation,
			SystemWriteConstraintInformation, // SYSTEM_WRITE_CONSTRAINT_INFORMATION
			SystemKernelVaShadowInformation, // SYSTEM_KERNEL_VA_SHADOW_INFORMATION
			SystemHypervisorSharedPageInformation, // SYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION // since REDSTONE4
			SystemFirmwareBootPerformanceInformation,
			SystemCodeIntegrityVerificationInformation, // SYSTEM_CODEINTEGRITYVERIFICATION_INFORMATION
			SystemFirmwarePartitionInformation, // SYSTEM_FIRMWARE_PARTITION_INFORMATION // 200
			SystemSpeculationControlInformation, // SYSTEM_SPECULATION_CONTROL_INFORMATION // (CVE-2017-5715) REDSTONE3 and above.
			SystemDmaGuardPolicyInformation, // SYSTEM_DMA_GUARD_POLICY_INFORMATION
			SystemEnclaveLaunchControlInformation, // SYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION
			SystemWorkloadAllowedCpuSetsInformation, // SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION // since REDSTONE5
			SystemCodeIntegrityUnlockModeInformation,
			SystemLeapSecondInformation, // SYSTEM_LEAP_SECOND_INFORMATION
			SystemFlags2Information, // q: SYSTEM_FLAGS_INFORMATION
			SystemSecurityModelInformation, // SYSTEM_SECURITY_MODEL_INFORMATION // since 19H1
			SystemCodeIntegritySyntheticCacheInformation,
			SystemFeatureConfigurationInformation, // SYSTEM_FEATURE_CONFIGURATION_INFORMATION // since 20H1 // 210
			SystemFeatureConfigurationSectionInformation, // SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION
			SystemFeatureUsageSubscriptionInformation, // SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS
			SystemSecureSpeculationControlInformation, // SECURE_SPECULATION_CONTROL_INFORMATION
			SystemSpacesBootInformation, // since 20H2
			SystemFwRamdiskInformation, // SYSTEM_FIRMWARE_RAMDISK_INFORMATION
			SystemWheaIpmiHardwareInformation,
			SystemDifSetRuleClassInformation,
			SystemDifClearRuleClassInformation,
			SystemDifApplyPluginVerificationOnDriver,
			SystemDifRemovePluginVerificationOnDriver, // 220
			SystemShadowStackInformation, // SYSTEM_SHADOW_STACK_INFORMATION
			SystemBuildVersionInformation, // SYSTEM_BUILD_VERSION_INFORMATION
			SystemPoolLimitInformation, // SYSTEM_POOL_LIMIT_INFORMATION
			SystemCodeIntegrityAddDynamicStore,
			SystemCodeIntegrityClearDynamicStores,
			SystemDifPoolTrackingInformation,
			SystemPoolZeroingInformation, // SYSTEM_POOL_ZEROING_INFORMATION
			SystemDpcWatchdogInformation,
			SystemDpcWatchdogInformation2,
			SystemSupportedProcessorArchitectures2,// q: in opt: HANDLE, out: SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION[] // NtQuerySystemInformationEx  // 230
			SystemSingleProcessorRelationshipInformation,
			SystemXfgCheckFailureInformation,
			MaxSystemInfoClass
		} SYSTEM_INFORMATION_CLASS;
		typedef enum _PROCESSINFOCLASS
		{
			ProcessBasicInformation, // q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
			ProcessQuotaLimits, // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
			ProcessIoCounters, // q: IO_COUNTERS
			ProcessVmCounters, // q: VM_COUNTERS, VM_COUNTERS_EX, VM_COUNTERS_EX2
			ProcessTimes, // q: KERNEL_USER_TIMES
			ProcessBasePriority, // s: KPRIORITY
			ProcessRaisePriority, // s: ULONG
			ProcessDebugPort, // q: HANDLE
			ProcessExceptionPort, // s: PROCESS_EXCEPTION_PORT
			ProcessAccessToken, // s: PROCESS_ACCESS_TOKEN
			ProcessLdtInformation, // qs: PROCESS_LDT_INFORMATION // 10
			ProcessLdtSize, // s: PROCESS_LDT_SIZE
			ProcessDefaultHardErrorMode, // qs: ULONG
			ProcessIoPortHandlers, // (kernel-mode only) // PROCESS_IO_PORT_HANDLER_INFORMATION
			ProcessPooledUsageAndLimits, // q: POOLED_USAGE_AND_LIMITS
			ProcessWorkingSetWatch, // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
			ProcessUserModeIOPL, // qs: ULONG (requires SeTcbPrivilege)
			ProcessEnableAlignmentFaultFixup, // s: BOOLEAN
			ProcessPriorityClass, // qs: PROCESS_PRIORITY_CLASS
			ProcessWx86Information, // qs: ULONG (requires SeTcbPrivilege) (VdmAllowed)
			ProcessHandleCount, // q: ULONG, PROCESS_HANDLE_INFORMATION // 20
			ProcessAffinityMask, // s: KAFFINITY
			ProcessPriorityBoost, // qs: ULONG
			ProcessDeviceMap, // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
			ProcessSessionInformation, // q: PROCESS_SESSION_INFORMATION
			ProcessForegroundInformation, // s: PROCESS_FOREGROUND_BACKGROUND
			ProcessWow64Information, // q: ULONG_PTR
			ProcessImageFileName, // q: UNICODE_STRING
			ProcessLUIDDeviceMapsEnabled, // q: ULONG
			ProcessBreakOnTermination, // qs: ULONG
			ProcessDebugObjectHandle, // q: HANDLE // 30
			ProcessDebugFlags, // qs: ULONG
			ProcessHandleTracing, // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
			ProcessIoPriority, // qs: IO_PRIORITY_HINT
			ProcessExecuteFlags, // qs: ULONG
			ProcessTlsInformation, // PROCESS_TLS_INFORMATION // ProcessResourceManagement 
			ProcessCookie, // q: ULONG
			ProcessImageInformation, // q: SECTION_IMAGE_INFORMATION
			ProcessCycleTime, // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
			ProcessPagePriority, // q: PAGE_PRIORITY_INFORMATION
			ProcessInstrumentationCallback, // s: PVOID or PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION // 40
			ProcessThreadStackAllocation, // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
			ProcessWorkingSetWatchEx, // q: PROCESS_WS_WATCH_INFORMATION_EX[]
			ProcessImageFileNameWin32, // q: UNICODE_STRING
			ProcessImageFileMapping, // q: HANDLE (input)
			ProcessAffinityUpdateMode, // qs: PROCESS_AFFINITY_UPDATE_MODE
			ProcessMemoryAllocationMode, // qs: PROCESS_MEMORY_ALLOCATION_MODE
			ProcessGroupInformation, // q: USHORT[]
			ProcessTokenVirtualizationEnabled, // s: ULONG
			ProcessConsoleHostProcess, // q: ULONG_PTR // ProcessOwnerInformation
			ProcessWindowInformation, // q: PROCESS_WINDOW_INFORMATION // 50
			ProcessHandleInformation, // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
			ProcessMitigationPolicy, // s: PROCESS_MITIGATION_POLICY_INFORMATION
			ProcessDynamicFunctionTableInformation,
			ProcessHandleCheckingMode, // qs: ULONG; s: 0 disables, otherwise enables
			ProcessKeepAliveCount, // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
			ProcessRevokeFileHandles, // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
			ProcessWorkingSetControl, // s: PROCESS_WORKING_SET_CONTROL
			ProcessHandleTable, // q: ULONG[] // since WINBLUE
			ProcessCheckStackExtentsMode, // qs: ULONG // KPROCESS->CheckStackExtents (CFG)
			ProcessCommandLineInformation, // q: UNICODE_STRING // 60
			ProcessProtectionInformation, // q: PS_PROTECTION
			ProcessMemoryExhaustion, // PROCESS_MEMORY_EXHAUSTION_INFO // since THRESHOLD
			ProcessFaultInformation, // PROCESS_FAULT_INFORMATION
			ProcessTelemetryIdInformation, // q: PROCESS_TELEMETRY_ID_INFORMATION
			ProcessCommitReleaseInformation, // PROCESS_COMMIT_RELEASE_INFORMATION
			ProcessDefaultCpuSetsInformation,
			ProcessAllowedCpuSetsInformation,
			ProcessSubsystemProcess,
			ProcessJobMemoryInformation, // q: PROCESS_JOB_MEMORY_INFO
			ProcessInPrivate, // s: void // ETW // since THRESHOLD2 // 70
			ProcessRaiseUMExceptionOnInvalidHandleClose, // qs: ULONG; s: 0 disables, otherwise enables
			ProcessIumChallengeResponse,
			ProcessChildProcessInformation, // q: PROCESS_CHILD_PROCESS_INFORMATION
			ProcessHighGraphicsPriorityInformation, // qs: BOOLEAN (requires SeTcbPrivilege)
			ProcessSubsystemInformation, // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
			ProcessEnergyValues, // q: PROCESS_ENERGY_VALUES, PROCESS_EXTENDED_ENERGY_VALUES
			ProcessPowerThrottlingState, // qs: POWER_THROTTLING_PROCESS_STATE
			ProcessReserved3Information, // ProcessActivityThrottlePolicy // PROCESS_ACTIVITY_THROTTLE_POLICY
			ProcessWin32kSyscallFilterInformation, // q: WIN32K_SYSCALL_FILTER
			ProcessDisableSystemAllowedCpuSets, // 80
			ProcessWakeInformation, // PROCESS_WAKE_INFORMATION
			ProcessEnergyTrackingState, // PROCESS_ENERGY_TRACKING_STATE
			ProcessManageWritesToExecutableMemory, // MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
			ProcessCaptureTrustletLiveDump,
			ProcessTelemetryCoverage,
			ProcessEnclaveInformation,
			ProcessEnableReadWriteVmLogging, // PROCESS_READWRITEVM_LOGGING_INFORMATION
			ProcessUptimeInformation, // q: PROCESS_UPTIME_INFORMATION
			ProcessImageSection, // q: HANDLE
			ProcessDebugAuthInformation, // since REDSTONE4 // 90
			ProcessSystemResourceManagement, // PROCESS_SYSTEM_RESOURCE_MANAGEMENT
			ProcessSequenceNumber, // q: ULONGLONG
			ProcessLoaderDetour, // since REDSTONE5
			ProcessSecurityDomainInformation, // PROCESS_SECURITY_DOMAIN_INFORMATION
			ProcessCombineSecurityDomainsInformation, // PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION
			ProcessEnableLogging, // PROCESS_LOGGING_INFORMATION
			ProcessLeapSecondInformation, // PROCESS_LEAP_SECOND_INFORMATION
			ProcessFiberShadowStackAllocation, // PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION // since 19H1
			ProcessFreeFiberShadowStackAllocation, // PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
			ProcessAltSystemCallInformation, // qs: BOOLEAN (kernel-mode only) // INT2E // since 20H1 // 100
			ProcessDynamicEHContinuationTargets, // PROCESS_DYNAMIC_EH_CONTINUATION_TARGETS_INFORMATION
			ProcessDynamicEnforcedCetCompatibleRanges, // PROCESS_DYNAMIC_ENFORCED_ADDRESS_RANGE_INFORMATION // since 20H2
			ProcessCreateStateChange, // since WIN11
			ProcessApplyStateChange,
			ProcessEnableOptionalXStateFeatures,
			MaxProcessInfoClass
		} PROCESSINFOCLASS;
		typedef enum _THREADINFOCLASS
		{
			ThreadBasicInformation, // q: THREAD_BASIC_INFORMATION
			ThreadTimes, // q: KERNEL_USER_TIMES
			ThreadPriority, // s: KPRIORITY (requires SeIncreaseBasePriorityPrivilege)
			ThreadBasePriority, // s: LONG
			ThreadAffinityMask, // s: KAFFINITY
			ThreadImpersonationToken, // s: HANDLE
			ThreadDescriptorTableEntry, // q: DESCRIPTOR_TABLE_ENTRY (or WOW64_DESCRIPTOR_TABLE_ENTRY)
			ThreadEnableAlignmentFaultFixup, // s: BOOLEAN
			ThreadEventPair,
			ThreadQuerySetWin32StartAddress, // q: ULONG_PTR
			ThreadZeroTlsCell, // s: ULONG // TlsIndex // 10
			ThreadPerformanceCount, // q: LARGE_INTEGER
			ThreadAmILastThread, // q: ULONG
			ThreadIdealProcessor, // s: ULONG
			ThreadPriorityBoost, // qs: ULONG
			ThreadSetTlsArrayAddress, // s: ULONG_PTR 
			ThreadIsIoPending, // q: ULONG
			ThreadHideFromDebugger, // q: BOOLEAN; s: void
			ThreadBreakOnTermination, // qs: ULONG
			ThreadSwitchLegacyState, // s: void // NtCurrentThread // NPX/FPU
			ThreadIsTerminated, // q: ULONG // 20
			ThreadLastSystemCall, // q: THREAD_LAST_SYSCALL_INFORMATION
			ThreadIoPriority, // qs: IO_PRIORITY_HINT (requires SeIncreaseBasePriorityPrivilege)
			ThreadCycleTime, // q: THREAD_CYCLE_TIME_INFORMATION
			ThreadPagePriority, // q: ULONG
			ThreadActualBasePriority, // s: LONG (requires SeIncreaseBasePriorityPrivilege)
			ThreadTebInformation, // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
			ThreadCSwitchMon,
			ThreadCSwitchPmu,
			ThreadWow64Context, // qs: WOW64_CONTEXT
			ThreadGroupInformation, // q: GROUP_AFFINITY // 30
			ThreadUmsInformation, // q: THREAD_UMS_INFORMATION
			ThreadCounterProfiling, // q: BOOLEAN; s: THREAD_PROFILING_INFORMATION?
			ThreadIdealProcessorEx, // q: PROCESSOR_NUMBER
			ThreadCpuAccountingInformation, // q: BOOLEAN; s: HANDLE (NtOpenSession) // NtCurrentThread // since WIN8
			ThreadSuspendCount, // q: ULONG // since WINBLUE
			ThreadHeterogeneousCpuPolicy, // q: KHETERO_CPU_POLICY // since THRESHOLD
			ThreadContainerId, // q: GUID
			ThreadNameInformation, // qs: THREAD_NAME_INFORMATION
			ThreadSelectedCpuSets,
			ThreadSystemThreadInformation, // q: SYSTEM_THREAD_INFORMATION // 40
			ThreadActualGroupAffinity, // q: GROUP_AFFINITY // since THRESHOLD2
			ThreadDynamicCodePolicyInfo, // q: ULONG; s: ULONG (NtCurrentThread)
			ThreadExplicitCaseSensitivity, // qs: ULONG; s: 0 disables, otherwise enables
			ThreadWorkOnBehalfTicket, // RTL_WORK_ON_BEHALF_TICKET_EX
			ThreadSubsystemInformation, // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
			ThreadDbgkWerReportActive, // s: ULONG; s: 0 disables, otherwise enables
			ThreadAttachContainer, // s: HANDLE (job object) // NtCurrentThread
			ThreadManageWritesToExecutableMemory, // MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
			ThreadPowerThrottlingState, // POWER_THROTTLING_THREAD_STATE
			ThreadWorkloadClass, // THREAD_WORKLOAD_CLASS // since REDSTONE5 // 50
			ThreadCreateStateChange, // since WIN11
			ThreadApplyStateChange,
			MaxThreadInfoClass
		} THREADINFOCLASS;

		typedef enum _MEMORY_INFORMATION_CLASS
		{
			MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
			MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
			MemoryMappedFilenameInformation, // UNICODE_STRING
			MemoryRegionInformation, // MEMORY_REGION_INFORMATION
			MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
			MemorySharedCommitInformation, // MEMORY_SHARED_COMMIT_INFORMATION
			MemoryImageInformation, // MEMORY_IMAGE_INFORMATION
			MemoryRegionInformationEx, // MEMORY_REGION_INFORMATION
			MemoryPrivilegedBasicInformation,
			MemoryEnclaveImageInformation, // MEMORY_ENCLAVE_IMAGE_INFORMATION // since REDSTONE3
			MemoryBasicInformationCapped, // 10
			MemoryPhysicalContiguityInformation, // MEMORY_PHYSICAL_CONTIGUITY_INFORMATION // since 20H1
			MemoryBadInformation, // since WIN11
			MaxMemoryInfoClass
		} MEMORY_INFORMATION_CLASS;
		//!@----------------------------------------------------------------------------------------------------------------------
		typedef struct _CLIENT_ID
		{
			HANDLE UniqueProcess;
			HANDLE UniqueThread;

			_CLIENT_ID() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _CLIENT_ID& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} CLIENT_ID, * PCLIENT_ID;

		typedef struct _CLIENT_ID32
		{
			ULONG UniqueProcess;
			ULONG UniqueThread;

			_CLIENT_ID32() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _CLIENT_ID32& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} CLIENT_ID32, * PCLIENT_ID32;

		typedef struct _CLIENT_ID64
		{
			ULONGLONG UniqueProcess;
			ULONGLONG UniqueThread;

			_CLIENT_ID64() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _CLIENT_ID64& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} CLIENT_ID64, * PCLIENT_ID64;

		typedef struct _PEB_FREE_BLOCK
		{
			struct _PEB_FREE_BLOCK* Next;
			ULONG Size;

			_PEB_FREE_BLOCK() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _PEB_FREE_BLOCK& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} PEB_FREE_BLOCK, * PPEB_FREE_BLOCK;

		typedef struct _PEB_LDR_DATA
		{
			ULONG Length;
			BOOLEAN Initialized;
			HANDLE SsHandle;
			LIST_ENTRY InLoadOrderModuleList;
			LIST_ENTRY InMemoryOrderModuleList;
			LIST_ENTRY InInitializationOrderModuleList;
			PVOID EntryInProgress;
			BOOLEAN ShutdownInProgress;
			HANDLE ShutdownThreadId;

			_PEB_LDR_DATA() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _PEB_LDR_DATA& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} PEB_LDR_DATA, * PPEB_LDR_DATA;
		typedef struct _API_SET_NAMESPACE
		{
			ULONG Version;
			ULONG Size;
			ULONG Flags;
			ULONG Count;
			ULONG EntryOffset;
			ULONG HashOffset;
			ULONG HashFactor;
			_API_SET_NAMESPACE() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _API_SET_NAMESPACE& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} API_SET_NAMESPACE, * PAPI_SET_NAMESPACE;


#define DOS_MAX_COMPONENT_LENGTH 255
#define DOS_MAX_PATH_LENGTH (DOS_MAX_COMPONENT_LENGTH + 5)

		typedef struct _STRING {
			USHORT Length;
			USHORT MaximumLength;
			PCHAR  Buffer;
		} STRING, PSTRING;

		typedef struct _UNICODE_STRING
		{
			USHORT Length;
			USHORT MaximumLength;
			_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;

			_UNICODE_STRING() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _UNICODE_STRING& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} UNICODE_STRING, * PUNICODE_STRING;

		typedef const UNICODE_STRING* PCUNICODE_STRING;

		typedef struct _CURDIR
		{
			UNICODE_STRING DosPath;
			HANDLE Handle;

			_CURDIR() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _CURDIR& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} CURDIR, * PCURDIR;

#define RTL_USER_PROC_CURDIR_CLOSE 0x00000002
#define RTL_USER_PROC_CURDIR_INHERIT 0x00000003

		typedef struct _RTL_DRIVE_LETTER_CURDIR
		{
			USHORT Flags;
			USHORT Length;
			ULONG TimeStamp;
			STRING DosPath;

			_RTL_DRIVE_LETTER_CURDIR() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _RTL_DRIVE_LETTER_CURDIR& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

		typedef struct _RTL_USER_PROCESS_PARAMETERS
		{
			ULONG MaximumLength;
			ULONG Length;

			ULONG Flags;
			ULONG DebugFlags;

			HANDLE ConsoleHandle;
			ULONG ConsoleFlags;
			HANDLE StandardInput;
			HANDLE StandardOutput;
			HANDLE StandardError;

			CURDIR CurrentDirectory;
			UNICODE_STRING DllPath;
			UNICODE_STRING ImagePathName;
			UNICODE_STRING CommandLine;
			PVOID Environment;

			ULONG StartingX;
			ULONG StartingY;
			ULONG CountX;
			ULONG CountY;
			ULONG CountCharsX;
			ULONG CountCharsY;
			ULONG FillAttribute;

			ULONG WindowFlags;
			ULONG ShowWindowFlags;
			UNICODE_STRING WindowTitle;
			UNICODE_STRING DesktopInfo;
			UNICODE_STRING ShellInfo;
			UNICODE_STRING RuntimeData;
			RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

			ULONG_PTR EnvironmentSize;
			ULONG_PTR EnvironmentVersion;

			PVOID PackageDependencyData;
			ULONG ProcessGroupId;
			ULONG LoaderThreads;

			UNICODE_STRING RedirectionDllName; // REDSTONE4
			UNICODE_STRING HeapPartitionName; // 19H1
			ULONG_PTR DefaultThreadpoolCpuSetMasks;
			ULONG DefaultThreadpoolCpuSetMaskCount;

			_RTL_USER_PROCESS_PARAMETERS() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _RTL_USER_PROCESS_PARAMETERS& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

		typedef struct _RTL_USER_PROCESS_PARAMETERS* PRTL_USER_PROCESS_PARAMETERS;
		typedef struct _RTL_CRITICAL_SECTION* PRTL_CRITICAL_SECTION;

		typedef struct _PEB
		{
			BOOLEAN InheritedAddressSpace;
			BOOLEAN ReadImageFileExecOptions;
			BOOLEAN BeingDebugged;
			union
			{
				BOOLEAN BitField;
				struct
				{
					BOOLEAN ImageUsesLargePages : 1;
					BOOLEAN IsProtectedProcess : 1;
					BOOLEAN IsImageDynamicallyRelocated : 1;
					BOOLEAN SkipPatchingUser32Forwarders : 1;
					BOOLEAN IsPackagedProcess : 1;
					BOOLEAN IsAppContainer : 1;
					BOOLEAN IsProtectedProcessLight : 1;
					BOOLEAN IsLongPathAwareProcess : 1;
				};
			};

			HANDLE Mutant;

			PVOID ImageBaseAddress;
			PPEB_LDR_DATA Ldr;
			PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
			PVOID SubSystemData;
			PVOID ProcessHeap;
			PRTL_CRITICAL_SECTION FastPebLock;
			PSLIST_HEADER AtlThunkSListPtr;
			PVOID IFEOKey;

			union
			{
				ULONG CrossProcessFlags;
				struct
				{
					ULONG ProcessInJob : 1;
					ULONG ProcessInitializing : 1;
					ULONG ProcessUsingVEH : 1;
					ULONG ProcessUsingVCH : 1;
					ULONG ProcessUsingFTH : 1;
					ULONG ProcessPreviouslyThrottled : 1;
					ULONG ProcessCurrentlyThrottled : 1;
					ULONG ProcessImagesHotPatched : 1; // REDSTONE5
					ULONG ReservedBits0 : 24;
				};
			};
			union
			{
				PVOID KernelCallbackTable;
				PVOID UserSharedInfoPtr;
			};
			ULONG SystemReserved;
			ULONG AtlThunkSListPtr32;
			PAPI_SET_NAMESPACE ApiSetMap;
			ULONG TlsExpansionCounter;
			PVOID TlsBitmap;
			ULONG TlsBitmapBits[2];

			PVOID ReadOnlySharedMemoryBase;
			PVOID SharedData; // HotpatchInformation
			PVOID* ReadOnlyStaticServerData;

			PVOID AnsiCodePageData; // PCPTABLEINFO
			PVOID OemCodePageData; // PCPTABLEINFO
			PVOID UnicodeCaseTableData; // PNLSTABLEINFO

			ULONG NumberOfProcessors;
			ULONG NtGlobalFlag;

			ULARGE_INTEGER CriticalSectionTimeout;
			SIZE_T HeapSegmentReserve;
			SIZE_T HeapSegmentCommit;
			SIZE_T HeapDeCommitTotalFreeThreshold;
			SIZE_T HeapDeCommitFreeBlockThreshold;

			ULONG NumberOfHeaps;
			ULONG MaximumNumberOfHeaps;
			PVOID* ProcessHeaps; // PHEAP

			PVOID GdiSharedHandleTable;
			PVOID ProcessStarterHelper;
			ULONG GdiDCAttributeList;

			PRTL_CRITICAL_SECTION LoaderLock;

			ULONG OSMajorVersion;
			ULONG OSMinorVersion;
			USHORT OSBuildNumber;
			USHORT OSCSDVersion;
			ULONG OSPlatformId;
			ULONG ImageSubsystem;
			ULONG ImageSubsystemMajorVersion;
			ULONG ImageSubsystemMinorVersion;
			ULONG_PTR ActiveProcessAffinityMask;
			GDI_HANDLE_BUFFER GdiHandleBuffer;
			PVOID PostProcessInitRoutine;

			PVOID TlsExpansionBitmap;
			ULONG TlsExpansionBitmapBits[32];

			ULONG SessionId;

			ULARGE_INTEGER AppCompatFlags;
			ULARGE_INTEGER AppCompatFlagsUser;
			PVOID pShimData;
			PVOID AppCompatInfo; // APPCOMPAT_EXE_DATA

			UNICODE_STRING CSDVersion;

			PVOID ActivationContextData; // ACTIVATION_CONTEXT_DATA
			PVOID ProcessAssemblyStorageMap; // ASSEMBLY_STORAGE_MAP
			PVOID SystemDefaultActivationContextData; // ACTIVATION_CONTEXT_DATA
			PVOID SystemAssemblyStorageMap; // ASSEMBLY_STORAGE_MAP

			SIZE_T MinimumStackCommit;

			PVOID SparePointers[4]; // 19H1 (previously FlsCallback to FlsHighIndex)
			ULONG SpareUlongs[5]; // 19H1
			//PVOID* FlsCallback;
			//LIST_ENTRY FlsListHead;
			//PVOID FlsBitmap;
			//ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
			//ULONG FlsHighIndex;

			PVOID WerRegistrationData;
			PVOID WerShipAssertPtr;
			PVOID pUnused; // pContextData
			PVOID pImageHeaderHash;
			union
			{
				ULONG TracingFlags;
				struct
				{
					ULONG HeapTracingEnabled : 1;
					ULONG CritSecTracingEnabled : 1;
					ULONG LibLoaderTracingEnabled : 1;
					ULONG SpareTracingBits : 29;
				};
			};
			ULONGLONG CsrServerReadOnlySharedMemoryBase;
			PRTL_CRITICAL_SECTION TppWorkerpListLock;
			LIST_ENTRY TppWorkerpList;
			PVOID WaitOnAddressHashTable[128];
			PVOID TelemetryCoverageHeader; // REDSTONE3
			ULONG CloudFileFlags;
			ULONG CloudFileDiagFlags; // REDSTONE4
			CHAR PlaceholderCompatibilityMode;
			CHAR PlaceholderCompatibilityModeReserved[7];
			struct _LEAP_SECOND_DATA* LeapSecondData; // REDSTONE5
			union
			{
				ULONG LeapSecondFlags;
				struct
				{
					ULONG SixtySecondEnabled : 1;
					ULONG Reserved : 31;
				};
			};
			ULONG NtGlobalFlag2;

			_PEB() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _PEB& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} PEB, * PPEB;

		typedef struct _PROCESS_BASIC_INFORMATION
		{
			LONG ExitStatus;
			PPEB PebBaseAddress;
			ULONG_PTR AffinityMask;
			KPRIORITY BasePriority;
			HANDLE UniqueProcessId;
			HANDLE InheritedFromUniqueProcessId;

			DWORD ProcessId() const { return static_cast<DWORD>(reinterpret_cast<size_t>(UniqueProcessId)); }
			DWORD ParentProcessId() const { return static_cast<DWORD>(reinterpret_cast<size_t>(InheritedFromUniqueProcessId)); }
			_PROCESS_BASIC_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			_PROCESS_BASIC_INFORMATION(const HANDLE& pid, const HANDLE& parent, const LONG& kpriority) {
				::memset(this, 0x00, sizeof(*this));
				UniqueProcessId = pid;
				InheritedFromUniqueProcessId = parent;
				BasePriority = kpriority;
			}
			void operator=(const _PROCESS_BASIC_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
			bool operator<(const _PROCESS_BASIC_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return UniqueProcessId < obj.UniqueProcessId;
				if (BasePriority != obj.BasePriority)
					return BasePriority < obj.BasePriority;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return InheritedFromUniqueProcessId < obj.InheritedFromUniqueProcessId;
				return false;
			}
			bool operator>(const _PROCESS_BASIC_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return UniqueProcessId > obj.UniqueProcessId;
				if (BasePriority != obj.BasePriority)
					return BasePriority > obj.BasePriority;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return InheritedFromUniqueProcessId > obj.InheritedFromUniqueProcessId;
				return false;
			}
			bool operator==(const _PROCESS_BASIC_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return false;
				if (BasePriority != obj.BasePriority)
					return false;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return false;
				return true;
			}
			bool operator!=(const _PROCESS_BASIC_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return true;
				if (BasePriority != obj.BasePriority)
					return true;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return true;
				return false;
			}
		} PROCESS_BASIC_INFORMATION, * PPROCESS_BASIC_INFORMATION;

		typedef struct _SYSTEM_THREAD_INFORMATION
		{
			LARGE_INTEGER KernelTime;
			LARGE_INTEGER UserTime;
			LARGE_INTEGER CreateTime;
			ULONG WaitTime;
			PVOID StartAddress;
			CLIENT_ID ClientId;
			KPRIORITY Priority;
			LONG BasePriority;
			ULONG ContextSwitches;
			KTHREAD_STATE ThreadState;
			KWAIT_REASON WaitReason;

			_SYSTEM_THREAD_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _SYSTEM_THREAD_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

		typedef struct _SYSTEM_PROCESS_INFORMATION
		{
			ULONG NextEntryOffset;
			ULONG NumberOfThreads;
			LARGE_INTEGER WorkingSetPrivateSize; // since VISTA
			ULONG HardFaultCount; // since WIN7
			ULONG NumberOfThreadsHighWatermark; // since WIN7
			ULONGLONG CycleTime; // since WIN7
			LARGE_INTEGER CreateTime;
			LARGE_INTEGER UserTime;
			LARGE_INTEGER KernelTime;
			UNICODE_STRING ImageName;
			KPRIORITY BasePriority;
			HANDLE UniqueProcessId;
			HANDLE InheritedFromUniqueProcessId;
			ULONG HandleCount;
			ULONG SessionId;
			ULONG_PTR UniqueProcessKey; // since VISTA (requires SystemExtendedProcessInformation)
			SIZE_T PeakVirtualSize;
			SIZE_T VirtualSize;
			ULONG PageFaultCount;
			SIZE_T PeakWorkingSetSize;
			SIZE_T WorkingSetSize;
			SIZE_T QuotaPeakPagedPoolUsage;
			SIZE_T QuotaPagedPoolUsage;
			SIZE_T QuotaPeakNonPagedPoolUsage;
			SIZE_T QuotaNonPagedPoolUsage;
			SIZE_T PagefileUsage;
			SIZE_T PeakPagefileUsage;
			SIZE_T PrivatePageCount;
			LARGE_INTEGER ReadOperationCount;
			LARGE_INTEGER WriteOperationCount;
			LARGE_INTEGER OtherOperationCount;
			LARGE_INTEGER ReadTransferCount;
			LARGE_INTEGER WriteTransferCount;
			LARGE_INTEGER OtherTransferCount;
			SYSTEM_THREAD_INFORMATION Threads[1]; // SystemProcessInformation
#if 0
			SYSTEM_EXTENDED_THREAD_INFORMATION Threads[1]; // SystemExtendedProcessinformation
			SYSTEM_EXTENDED_THREAD_INFORMATION + SYSTEM_PROCESS_INFORMATION_EXTENSION // SystemFullProcessInformation
#endif
				_SYSTEM_PROCESS_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			_SYSTEM_PROCESS_INFORMATION(const HANDLE& pid, const HANDLE& parent, const LONGLONG& createtime) {
				::memset(this, 0x00, sizeof(*this));
				UniqueProcessId = pid;
				InheritedFromUniqueProcessId = parent;
				CreateTime.QuadPart = createtime;
			}
			void operator=(const _SYSTEM_PROCESS_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
			bool operator<(const _SYSTEM_PROCESS_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return UniqueProcessId < obj.UniqueProcessId;
				if (CreateTime.QuadPart != obj.CreateTime.QuadPart)
					return CreateTime.QuadPart < obj.CreateTime.QuadPart;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return InheritedFromUniqueProcessId < obj.InheritedFromUniqueProcessId;
				return false;
			}
			bool operator==(const _SYSTEM_PROCESS_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return false;
				if (CreateTime.QuadPart != obj.CreateTime.QuadPart)
					return false;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return false;
				return true;
			}
			bool operator!=(const _SYSTEM_PROCESS_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return true;
				if (CreateTime.QuadPart != obj.CreateTime.QuadPart)
					return true;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return true;
				return false;
			}
			bool operator>(const _SYSTEM_PROCESS_INFORMATION& obj) const {
				if (UniqueProcessId != obj.UniqueProcessId)
					return UniqueProcessId > obj.UniqueProcessId;
				if (CreateTime.QuadPart != obj.CreateTime.QuadPart)
					return CreateTime.QuadPart > obj.CreateTime.QuadPart;
				if (InheritedFromUniqueProcessId != obj.InheritedFromUniqueProcessId)
					return InheritedFromUniqueProcessId > obj.InheritedFromUniqueProcessId;
				return false;
			}
			DWORD ProcessId() const { return static_cast<DWORD>(reinterpret_cast<size_t>(UniqueProcessId)); }
			DWORD ParentProcessId() const { return static_cast<DWORD>(reinterpret_cast<size_t>(InheritedFromUniqueProcessId)); }
		} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

		// private
		typedef struct _SYSTEM_PROCESS_ID_INFORMATION
		{
			HANDLE ProcessId;
			UNICODE_STRING ImageName;

			_SYSTEM_PROCESS_ID_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _SYSTEM_PROCESS_ID_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} SYSTEM_PROCESS_ID_INFORMATION, * PSYSTEM_PROCESS_ID_INFORMATION;


		typedef struct _KEY_VALUE_BASIC_INFORMATION {
			ULONG TitleIndex;
			ULONG Type;
			ULONG NameLength;
			WCHAR Name[1];

			_KEY_VALUE_BASIC_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _KEY_VALUE_BASIC_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} KEY_VALUE_BASIC_INFORMATION, * PKEY_VALUE_BASIC_INFORMATION;

		typedef struct _KEY_VALUE_FULL_INFORMATION {
			ULONG TitleIndex;
			ULONG Type;
			ULONG DataOffset;
			ULONG DataLength;
			ULONG NameLength;
			WCHAR Name[1];

			_KEY_VALUE_FULL_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _KEY_VALUE_FULL_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} KEY_VALUE_FULL_INFORMATION, * PKEY_VALUE_FULL_INFORMATION;

		typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
			ULONG TitleIndex;
			ULONG Type;
			ULONG DataLength;
			UCHAR Data[1];

			_KEY_VALUE_PARTIAL_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _KEY_VALUE_PARTIAL_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} KEY_VALUE_PARTIAL_INFORMATION, * PKEY_VALUE_PARTIAL_INFORMATION;

		typedef struct _IO_STATUS_BLOCK {
			union {
				LONG Status;
				PVOID    Pointer;
			};
			ULONG_PTR Information;

			_IO_STATUS_BLOCK() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _IO_STATUS_BLOCK& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

#if 0
#define OBJ_INHERIT                         0x00000002L
#define OBJ_PERMANENT                       0x00000010L
#define OBJ_EXCLUSIVE                       0x00000020L
#define OBJ_CASE_INSENSITIVE                0x00000040L
#define OBJ_OPENIF                          0x00000080L
#define OBJ_OPENLINK                        0x00000100L
#define OBJ_KERNEL_HANDLE                   0x00000200L
#define OBJ_FORCE_ACCESS_CHECK              0x00000400L
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP   0x00000800L
#define OBJ_DONT_REPARSE                    0x00001000L
#define OBJ_VALID_ATTRIBUTES                0x00001FF2L
#endif


#define OBJ_PROTECT_CLOSE 0x00000001

#ifdef OBJ_INHERIT
#undef OBJ_INHERIT
#define OBJ_INHERIT                         0x00000002L
#else
#define OBJ_INHERIT                         0x00000002L
#endif

#ifdef OBJ_PERMANENT
#undef OBJ_PERMANENT
#define OBJ_PERMANENT                       0x00000010L
#else
#define OBJ_PERMANENT                       0x00000010L
#endif

#ifdef OBJ_EXCLUSIVE
#undef OBJ_EXCLUSIVE
#define OBJ_EXCLUSIVE                       0x00000020L
#else
#define OBJ_EXCLUSIVE                       0x00000020L
#endif

#ifdef OBJ_CASE_INSENSITIVE
#undef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE                0x00000040L
#else
#define OBJ_CASE_INSENSITIVE                0x00000040L
#endif

#ifdef OBJ_OPENIF
#undef OBJ_OPENIF
#define OBJ_OPENIF                          0x00000080L
#else
#define OBJ_OPENIF                          0x00000080L
#endif

#ifdef OBJ_OPENLINK
#undef OBJ_OPENLINK
#define OBJ_OPENLINK                        0x00000100L
#else
#define OBJ_OPENLINK                        0x00000100L
#endif

#ifdef OBJ_KERNEL_HANDLE
#undef OBJ_KERNEL_HANDLE
#define OBJ_KERNEL_HANDLE                   0x00000200L
#else
#define OBJ_KERNEL_HANDLE                   0x00000200L
#endif

#ifdef OBJ_FORCE_ACCESS_CHECK
#undef OBJ_FORCE_ACCESS_CHECK
#define OBJ_FORCE_ACCESS_CHECK              0x00000400L
#else
#define OBJ_FORCE_ACCESS_CHECK              0x00000400L
#endif

#ifdef OBJ_IGNORE_IMPERSONATED_DEVICEMAP
#undef OBJ_IGNORE_IMPERSONATED_DEVICEMAP
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP   0x00000800L
#else
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP   0x00000800L
#endif

#ifdef OBJ_DONT_REPARSE
#undef OBJ_DONT_REPARSE
#define OBJ_DONT_REPARSE                    0x00001000L
#else
#define OBJ_DONT_REPARSE                    0x00001000L
#endif

#ifdef OBJ_VALID_ATTRIBUTES
#undef OBJ_VALID_ATTRIBUTES
#define OBJ_VALID_ATTRIBUTES                0x00001FF2L
#else
#define OBJ_VALID_ATTRIBUTES                0x00001FF2L
#endif

#if 0
#ifndef OBJ_INHERIT
#define OBJ_INHERIT 0x00000002
#endif
#define OBJ_AUDIT_OBJECT_CLOSE 0x00000004
#ifndef OBJ_PERMANENT
#define OBJ_PERMANENT 0x00000010
#endif
#ifndef OBJ_EXCLUSIVE
#define OBJ_EXCLUSIVE 0x00000020
#endif
#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE 0x00000040
#endif
#ifndef OBJ_OPENIF
#define OBJ_OPENIF 0x00000080
#endif
#ifndef OBJ_OPENLINK
#define OBJ_OPENLINK 0x00000100
#endif
#ifndef OBJ_KERNEL_HANDLE
#define OBJ_KERNEL_HANDLE 0x00000200
#endif
#ifndef OBJ_FORCE_ACCESS_CHECK
#define OBJ_FORCE_ACCESS_CHECK 0x00000400
#endif
#ifndef OBJ_IGNORE_IMPERSONATED_DEVICEMAP
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP 0x00000800
#endif
#ifndef OBJ_DONT_REPARSE
#define OBJ_DONT_REPARSE 0x00001000
#endif
#ifndef OBJ_VALID_ATTRIBUTES
#define OBJ_VALID_ATTRIBUTES 0x00001ff2
#endif
#endif
		typedef struct _OBJECT_ATTRIBUTES
		{
			ULONG Length;
			HANDLE RootDirectory;
			PUNICODE_STRING ObjectName;
			ULONG Attributes;
			PVOID SecurityDescriptor; // PSECURITY_DESCRIPTOR;
			PVOID SecurityQualityOfService; // PSECURITY_QUALITY_OF_SERVICE

			_OBJECT_ATTRIBUTES() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _OBJECT_ATTRIBUTES& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

		typedef const OBJECT_ATTRIBUTES* PCOBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
    }

#define RTL_CONSTANT_OBJECT_ATTRIBUTES(n, a) { sizeof(OBJECT_ATTRIBUTES), NULL, n, a, NULL, NULL }
#define RTL_INIT_OBJECT_ATTRIBUTES(n, a) RTL_CONSTANT_OBJECT_ATTRIBUTES(n, a)

#define OBJ_NAME_PATH_SEPARATOR ((WCHAR)L'\\')

		// Portability

		typedef struct _OBJECT_ATTRIBUTES64
		{
			ULONG Length;
			ULONG64 RootDirectory;
			ULONG64 ObjectName;
			ULONG Attributes;
			ULONG64 SecurityDescriptor;
			ULONG64 SecurityQualityOfService;

			_OBJECT_ATTRIBUTES64() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _OBJECT_ATTRIBUTES64& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} OBJECT_ATTRIBUTES64, * POBJECT_ATTRIBUTES64;

		typedef const OBJECT_ATTRIBUTES64* PCOBJECT_ATTRIBUTES64;

		typedef struct _OBJECT_ATTRIBUTES32
		{
			ULONG Length;
			ULONG RootDirectory;
			ULONG ObjectName;
			ULONG Attributes;
			ULONG SecurityDescriptor;
			ULONG SecurityQualityOfService;

			_OBJECT_ATTRIBUTES32() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _OBJECT_ATTRIBUTES32& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} OBJECT_ATTRIBUTES32, * POBJECT_ATTRIBUTES32;

		typedef const OBJECT_ATTRIBUTES32* PCOBJECT_ATTRIBUTES32;

		typedef struct _SECTION_IMAGE_INFORMATION
		{
			PVOID TransferAddress;
			ULONG ZeroBits;
			SIZE_T MaximumStackSize;
			SIZE_T CommittedStackSize;
			ULONG SubSystemType;
			union
			{
				struct
				{
					USHORT SubSystemMinorVersion;
					USHORT SubSystemMajorVersion;
				};
				ULONG SubSystemVersion;
			};
			union
			{
				struct
				{
					USHORT MajorOperatingSystemVersion;
					USHORT MinorOperatingSystemVersion;
				};
				ULONG OperatingSystemVersion;
			};
			USHORT ImageCharacteristics;
			USHORT DllCharacteristics;
			USHORT Machine;
			BOOLEAN ImageContainsCode;
			union
			{
				UCHAR ImageFlags;
				struct
				{
					UCHAR ComPlusNativeReady : 1;
					UCHAR ComPlusILOnly : 1;
					UCHAR ImageDynamicallyRelocated : 1;
					UCHAR ImageMappedFlat : 1;
					UCHAR BaseBelow4gb : 1;
					UCHAR ComPlusPrefer32bit : 1;
					UCHAR Reserved : 2;
				};
			};
			ULONG LoaderFlags;
			ULONG ImageFileSize;
			ULONG CheckSum;

			_SECTION_IMAGE_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _SECTION_IMAGE_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} SECTION_IMAGE_INFORMATION, * PSECTION_IMAGE_INFORMATION;

		typedef struct _RTL_USER_PROCESS_INFORMATION
		{
			ULONG Length;
			HANDLE ProcessHandle;
			HANDLE ThreadHandle;
			CLIENT_ID ClientId;
			SECTION_IMAGE_INFORMATION ImageInformation;

			_RTL_USER_PROCESS_INFORMATION() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _RTL_USER_PROCESS_INFORMATION& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} RTL_USER_PROCESS_INFORMATION, * PRTL_USER_PROCESS_INFORMATION;

#define GDI_BATCH_BUFFER_SIZE 310

		typedef struct _GDI_TEB_BATCH
		{
			ULONG Offset;
			ULONG_PTR HDC;
			ULONG Buffer[GDI_BATCH_BUFFER_SIZE];

			_GDI_TEB_BATCH() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _GDI_TEB_BATCH& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} GDI_TEB_BATCH, * PGDI_TEB_BATCH;

		typedef struct _TEB_ACTIVE_FRAME_CONTEXT
		{
			ULONG Flags;
			PSTR FrameName;

			_TEB_ACTIVE_FRAME_CONTEXT() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _TEB_ACTIVE_FRAME_CONTEXT& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} TEB_ACTIVE_FRAME_CONTEXT, * PTEB_ACTIVE_FRAME_CONTEXT;

		typedef struct _TEB_ACTIVE_FRAME
		{
			ULONG Flags;
			struct _TEB_ACTIVE_FRAME* Previous;
			PTEB_ACTIVE_FRAME_CONTEXT Context;

			_TEB_ACTIVE_FRAME() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _TEB_ACTIVE_FRAME& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} TEB_ACTIVE_FRAME, * PTEB_ACTIVE_FRAME;

		typedef struct _ACTIVATION_CONTEXT_STACK
		{
			struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME* ActiveFrame;
			LIST_ENTRY FrameListCache;
			ULONG Flags;
			ULONG NextCookieSequenceNumber;
			ULONG StackId;

			_ACTIVATION_CONTEXT_STACK() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _ACTIVATION_CONTEXT_STACK& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} ACTIVATION_CONTEXT_STACK, * PACTIVATION_CONTEXT_STACK;

		typedef struct _TEB
		{
			NT_TIB NtTib;

			PVOID EnvironmentPointer;
			CLIENT_ID ClientId;
			PVOID ActiveRpcHandle;
			PVOID ThreadLocalStoragePointer;
			PPEB ProcessEnvironmentBlock;

			ULONG LastErrorValue;
			ULONG CountOfOwnedCriticalSections;
			PVOID CsrClientThread;
			PVOID Win32ThreadInfo;
			ULONG User32Reserved[26];
			ULONG UserReserved[5];
			PVOID WOW32Reserved;
			LCID CurrentLocale;
			ULONG FpSoftwareStatusRegister;
			PVOID ReservedForDebuggerInstrumentation[16];
#ifdef _WIN64
			PVOID SystemReserved1[30];
#else
			PVOID SystemReserved1[26];
#endif

			CHAR PlaceholderCompatibilityMode;
			BOOLEAN PlaceholderHydrationAlwaysExplicit;
			CHAR PlaceholderReserved[10];

			ULONG ProxiedProcessId;
			ACTIVATION_CONTEXT_STACK ActivationStack;

			UCHAR WorkingOnBehalfTicket[8];
			LONG ExceptionCode;

			PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;
			ULONG_PTR InstrumentationCallbackSp;
			ULONG_PTR InstrumentationCallbackPreviousPc;
			ULONG_PTR InstrumentationCallbackPreviousSp;
#ifdef _WIN64
			ULONG TxFsContext;
#endif

			BOOLEAN InstrumentationCallbackDisabled;
#ifdef _WIN64
			BOOLEAN UnalignedLoadStoreExceptions;
#endif
#ifndef _WIN64
			UCHAR SpareBytes[23];
			ULONG TxFsContext;
#endif
			GDI_TEB_BATCH GdiTebBatch;
			CLIENT_ID RealClientId;
			HANDLE GdiCachedProcessHandle;
			ULONG GdiClientPID;
			ULONG GdiClientTID;
			PVOID GdiThreadLocalInfo;
			ULONG_PTR Win32ClientInfo[62];
			PVOID glDispatchTable[233];
			ULONG_PTR glReserved1[29];
			PVOID glReserved2;
			PVOID glSectionInfo;
			PVOID glSection;
			PVOID glTable;
			PVOID glCurrentRC;
			PVOID glContext;

			LONG LastStatusValue;
			UNICODE_STRING StaticUnicodeString;
			WCHAR StaticUnicodeBuffer[261];

			PVOID DeallocationStack;
			PVOID TlsSlots[64];
			LIST_ENTRY TlsLinks;

			PVOID Vdm;
			PVOID ReservedForNtRpc;
			PVOID DbgSsReserved[2];

			ULONG HardErrorMode;
#ifdef _WIN64
			PVOID Instrumentation[11];
#else
			PVOID Instrumentation[9];
#endif
			GUID ActivityId;

			PVOID SubProcessTag;
			PVOID PerflibData;
			PVOID EtwTraceData;
			PVOID WinSockData;
			ULONG GdiBatchCount;

			union
			{
				PROCESSOR_NUMBER CurrentIdealProcessor;
				ULONG IdealProcessorValue;
				struct
				{
					UCHAR ReservedPad0;
					UCHAR ReservedPad1;
					UCHAR ReservedPad2;
					UCHAR IdealProcessor;
				};
			};

			ULONG GuaranteedStackBytes;
			PVOID ReservedForPerf;
			PVOID ReservedForOle;
			ULONG WaitingOnLoaderLock;
			PVOID SavedPriorityState;
			ULONG_PTR ReservedForCodeCoverage;
			PVOID ThreadPoolData;
			PVOID* TlsExpansionSlots;
#ifdef _WIN64
			PVOID DeallocationBStore;
			PVOID BStoreLimit;
#endif
			ULONG MuiGeneration;
			ULONG IsImpersonating;
			PVOID NlsCache;
			PVOID pShimData;
			ULONG HeapData;
			HANDLE CurrentTransactionHandle;
			PTEB_ACTIVE_FRAME ActiveFrame;
			PVOID FlsData;

			PVOID PreferredLanguages;
			PVOID UserPrefLanguages;
			PVOID MergedPrefLanguages;
			ULONG MuiImpersonation;

			union
			{
				USHORT CrossTebFlags;
				USHORT SpareCrossTebBits : 16;
			};
			union
			{
				USHORT SameTebFlags;
				struct
				{
					USHORT SafeThunkCall : 1;
					USHORT InDebugPrint : 1;
					USHORT HasFiberData : 1;
					USHORT SkipThreadAttach : 1;
					USHORT WerInShipAssertCode : 1;
					USHORT RanProcessInit : 1;
					USHORT ClonedThread : 1;
					USHORT SuppressDebugMsg : 1;
					USHORT DisableUserStackWalk : 1;
					USHORT RtlExceptionAttached : 1;
					USHORT InitialThread : 1;
					USHORT SessionAware : 1;
					USHORT LoadOwner : 1;
					USHORT LoaderWorker : 1;
					USHORT SkipLoaderInit : 1;
					USHORT SpareSameTebBits : 1;
				};
			};

			PVOID TxnScopeEnterCallback;
			PVOID TxnScopeExitCallback;
			PVOID TxnScopeContext;
			ULONG LockCount;
			LONG WowTebOffset;
			PVOID ResourceRetValue;
			PVOID ReservedForWdf;
			ULONGLONG ReservedForCrt;
			GUID EffectiveContainerId;
			ULONGLONG LastSleepCounter; // Win11
			ULONG SpinCallCount;
			ULONGLONG ExtendedFeatureDisableMask;

			_TEB() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _TEB& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} TEB, * PTEB;


		typedef BOOLEAN(__stdcall* PLDR_INIT_ROUTINE)(
			_In_ PVOID DllHandle,
			_In_ ULONG Reason,
			_In_opt_ PVOID Context
			);

		// symbols
		typedef struct _LDR_SERVICE_TAG_RECORD
		{
			struct _LDR_SERVICE_TAG_RECORD* Next;
			ULONG ServiceTag;

			_LDR_SERVICE_TAG_RECORD() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _LDR_SERVICE_TAG_RECORD& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} LDR_SERVICE_TAG_RECORD, * PLDR_SERVICE_TAG_RECORD;

		// symbols
		typedef struct _LDRP_CSLIST
		{
			PSINGLE_LIST_ENTRY Tail;

			_LDRP_CSLIST() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _LDRP_CSLIST& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} LDRP_CSLIST, * PLDRP_CSLIST;

		// symbols
		typedef enum _LDR_DDAG_STATE
		{
			LdrModulesMerged = -5,
			LdrModulesInitError = -4,
			LdrModulesSnapError = -3,
			LdrModulesUnloaded = -2,
			LdrModulesUnloading = -1,
			LdrModulesPlaceHolder = 0,
			LdrModulesMapping = 1,
			LdrModulesMapped = 2,
			LdrModulesWaitingForDependencies = 3,
			LdrModulesSnapping = 4,
			LdrModulesSnapped = 5,
			LdrModulesCondensed = 6,
			LdrModulesReadyToInit = 7,
			LdrModulesInitializing = 8,
			LdrModulesReadyToRun = 9
		} LDR_DDAG_STATE;

		// symbols
		typedef struct _LDR_DDAG_NODE
		{
			LIST_ENTRY Modules;
			PLDR_SERVICE_TAG_RECORD ServiceTagList;
			ULONG LoadCount;
			ULONG LoadWhileUnloadingCount;
			ULONG LowestLink;
			union
			{
				LDRP_CSLIST Dependencies;
				SINGLE_LIST_ENTRY RemovalLink;
			};
			LDRP_CSLIST IncomingDependencies;
			LDR_DDAG_STATE State;
			SINGLE_LIST_ENTRY CondenseLink;
			ULONG PreorderNumber;

			_LDR_DDAG_NODE() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _LDR_DDAG_NODE& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} LDR_DDAG_NODE, * PLDR_DDAG_NODE;

		// rev
		typedef struct _LDR_DEPENDENCY_RECORD
		{
			SINGLE_LIST_ENTRY DependencyLink;
			PLDR_DDAG_NODE DependencyNode;
			SINGLE_LIST_ENTRY IncomingDependencyLink;
			PLDR_DDAG_NODE IncomingDependencyNode;

			_LDR_DEPENDENCY_RECORD() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _LDR_DEPENDENCY_RECORD& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} LDR_DEPENDENCY_RECORD, * PLDR_DEPENDENCY_RECORD;

		// symbols
		typedef enum _LDR_DLL_LOAD_REASON
		{
			LoadReasonStaticDependency,
			LoadReasonStaticForwarderDependency,
			LoadReasonDynamicForwarderDependency,
			LoadReasonDelayloadDependency,
			LoadReasonDynamicLoad,
			LoadReasonAsImageLoad,
			LoadReasonAsDataLoad,
			LoadReasonEnclavePrimary, // REDSTONE3
			LoadReasonEnclaveDependency,
			LoadReasonUnknown = -1
		} LDR_DLL_LOAD_REASON, * PLDR_DLL_LOAD_REASON;

#define LDRP_PACKAGED_BINARY 0x00000001
#define LDRP_STATIC_LINK 0x00000002
#define LDRP_IMAGE_DLL 0x00000004
#define LDRP_LOAD_IN_PROGRESS 0x00001000
#define LDRP_UNLOAD_IN_PROGRESS 0x00002000
#define LDRP_ENTRY_PROCESSED 0x00004000
#define LDRP_ENTRY_INSERTED 0x00008000
#define LDRP_CURRENT_LOAD 0x00010000
#define LDRP_FAILED_BUILTIN_LOAD 0x00020000
#define LDRP_DONT_CALL_FOR_THREADS 0x00040000
#define LDRP_PROCESS_ATTACH_CALLED 0x00080000
#define LDRP_DEBUG_SYMBOLS_LOADED 0x00100000
#define LDRP_IMAGE_NOT_AT_BASE 0x00200000 // Vista and below
#define LDRP_COR_IMAGE 0x00400000
#define LDRP_DONT_RELOCATE 0x00800000 // LDR_COR_OWNS_UNMAP
#define LDRP_SYSTEM_MAPPED 0x01000000
#define LDRP_IMAGE_VERIFYING 0x02000000
#define LDRP_DRIVER_DEPENDENT_DLL 0x04000000
#define LDRP_ENTRY_NATIVE 0x08000000
#define LDRP_REDIRECTED 0x10000000
#define LDRP_NON_PAGED_DEBUG_INFO 0x20000000
#define LDRP_MM_LOADED 0x40000000
#define LDRP_COMPAT_DATABASE_PROCESSED 0x80000000

#define LDR_DATA_TABLE_ENTRY_SIZE_WINXP FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, DdagNode)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN7 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, BaseNameHashValue)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN8 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, ImplicitPathOptions)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN10 sizeof(LDR_DATA_TABLE_ENTRY)

		typedef struct _RTL_BALANCED_NODE
		{
			union
			{
				struct _RTL_BALANCED_NODE* Children[2];
				struct
				{
					struct _RTL_BALANCED_NODE* Left;
					struct _RTL_BALANCED_NODE* Right;
				};
			};
			union
			{
				UCHAR Red : 1;
				UCHAR Balance : 2;
				ULONG_PTR ParentValue;
			};

			_RTL_BALANCED_NODE() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _RTL_BALANCED_NODE& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} RTL_BALANCED_NODE, * PRTL_BALANCED_NODE;

#if 1//!@ 该结构在注入PE加载器中存在问题
		// symbols
		typedef struct _LDR_DATA_TABLE_ENTRY
		{
			LIST_ENTRY InLoadOrderLinks;
			LIST_ENTRY InMemoryOrderLinks;
			union
			{
				LIST_ENTRY InInitializationOrderLinks;
				LIST_ENTRY InProgressLinks;
			};
			PVOID DllBase;
			PLDR_INIT_ROUTINE EntryPoint;
			ULONG SizeOfImage;
			UNICODE_STRING FullDllName;
			UNICODE_STRING BaseDllName;
			union
			{
				UCHAR FlagGroup[4];
				ULONG Flags;
				struct
				{
					ULONG PackagedBinary : 1;
					ULONG MarkedForRemoval : 1;
					ULONG ImageDll : 1;
					ULONG LoadNotificationsSent : 1;
					ULONG TelemetryEntryProcessed : 1;
					ULONG ProcessStaticImport : 1;
					ULONG InLegacyLists : 1;
					ULONG InIndexes : 1;
					ULONG ShimDll : 1;
					ULONG InExceptionTable : 1;
					ULONG ReservedFlags1 : 2;
					ULONG LoadInProgress : 1;
					ULONG LoadConfigProcessed : 1;
					ULONG EntryProcessed : 1;
					ULONG ProtectDelayLoad : 1;
					ULONG ReservedFlags3 : 2;
					ULONG DontCallForThreads : 1;
					ULONG ProcessAttachCalled : 1;
					ULONG ProcessAttachFailed : 1;
					ULONG CorDeferredValidate : 1;
					ULONG CorImage : 1;
					ULONG DontRelocate : 1;
					ULONG CorILOnly : 1;
					ULONG ChpeImage : 1;
					ULONG ReservedFlags5 : 2;
					ULONG Redirected : 1;
					ULONG ReservedFlags6 : 2;
					ULONG CompatDatabaseProcessed : 1;
				};
			};
			USHORT ObsoleteLoadCount;
			USHORT TlsIndex;
			LIST_ENTRY HashLinks;
			ULONG TimeDateStamp;
			struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
			PVOID Lock; // RtlAcquireSRWLockExclusive
			PLDR_DDAG_NODE DdagNode;
			LIST_ENTRY NodeModuleLink;
			struct _LDRP_LOAD_CONTEXT* LoadContext;
			PVOID ParentDllBase;
			PVOID SwitchBackContext;
			RTL_BALANCED_NODE BaseAddressIndexNode;
			RTL_BALANCED_NODE MappingInfoIndexNode;
			ULONG_PTR OriginalBase;
			LARGE_INTEGER LoadTime;
			ULONG BaseNameHashValue;
			LDR_DLL_LOAD_REASON LoadReason;
			ULONG ImplicitPathOptions;
			ULONG ReferenceCount;
			ULONG DependentLoadFlags;
			UCHAR SigningLevel; // since REDSTONE2

			_LDR_DATA_TABLE_ENTRY() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _LDR_DATA_TABLE_ENTRY& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;
#else

#endif

#if 1//!@ PELoader use
		typedef struct
		{
			WORD	offset : 12;
			WORD	type : 4;
		} IMAGE_RELOC, * PIMAGE_RELOC;
		// WinDbg> dt -v ntdll!_LDR_DATA_TABLE_ENTRY
	//__declspec( align(8) ) 
		typedef struct _LDR_DATA_TABLE_ENTRY_FOR_PELOADER
		{
			//LIST_ENTRY InLoadOrderLinks; // As we search from PPEB_LDR_DATA->InMemoryOrderModuleList we dont use the first entry.
			LIST_ENTRY InMemoryOrderModuleList;
			LIST_ENTRY InInitializationOrderModuleList;
			PVOID DllBase;
			PVOID EntryPoint;
			ULONG SizeOfImage;
			UNICODE_STRING FullDllName;
			UNICODE_STRING BaseDllName;
			ULONG Flags;
			SHORT LoadCount;
			SHORT TlsIndex;
			LIST_ENTRY HashTableEntry;
			ULONG TimeDateStamp;

			_LDR_DATA_TABLE_ENTRY_FOR_PELOADER() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const _LDR_DATA_TABLE_ENTRY_FOR_PELOADER& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		} LDR_DATA_TABLE_ENTRY_FOR_PELOADER, * PLDR_DATA_TABLE_ENTRY_FOR_PELOADER;
#endif


#define SK_ENUM_PROCESS_MODULES_LIMIT 0x800
#define LDR_IS_DATAFILE(DllHandle) (((ULONG_PTR)(DllHandle)) & (ULONG_PTR)1)
#define LDR_IS_IMAGEMAPPING(DllHandle) (((ULONG_PTR)(DllHandle)) & (ULONG_PTR)2)
#define LDR_MAPPEDVIEW_TO_DATAFILE(BaseAddress) ((PVOID)(((ULONG_PTR)(BaseAddress)) | (ULONG_PTR)1))
#define LDR_MAPPEDVIEW_TO_IMAGEMAPPING(BaseAddress) ((PVOID)(((ULONG_PTR)(BaseAddress)) | (ULONG_PTR)2))
#define LDR_DATAFILE_TO_MAPPEDVIEW(DllHandle) ((PVOID)(((ULONG_PTR)(DllHandle)) & ~(ULONG_PTR)1))
#define LDR_IMAGEMAPPING_TO_MAPPEDVIEW(DllHandle) ((PVOID)(((ULONG_PTR)(DllHandle)) & ~(ULONG_PTR)2))
#define LDR_IS_RESOURCE(DllHandle) (LDR_IS_IMAGEMAPPING(DllHandle) || LDR_IS_DATAFILE(DllHandle))









		//#pragma pack(pop)
#ifndef STATUS_COMMON_ERROR 
#define STATUS_COMMON_ERROR ((LONG)-1)
#endif
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS                   ((LONG)0x00000000L)    // ntsubauth
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL              ((LONG)0xC0000001L)
#endif
#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH      ((LONG)0xC0000004L)
#endif
#ifndef STATUS_ACCESS_VIOLATION
#define STATUS_ACCESS_VIOLATION          ((LONG)0xC0000005L)    // winnt
#endif
#ifndef STATUS_ACCESS_DENIED
#define STATUS_ACCESS_DENIED ((LONG)0xC0000022L)
#endif
#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL          ((LONG)0xC0000023L)
#endif
#ifndef STATUS_OBJECT_TYPE_MISMATCH
#define STATUS_OBJECT_TYPE_MISMATCH      ((LONG)0xC0000024L)
#endif
#ifndef STATUS_NONCONTINUABLE_EXCEPTION
#define STATUS_NONCONTINUABLE_EXCEPTION  ((LONG)0xC0000025L)    // winnt
#endif
#ifndef STATUS_INVALID_DISPOSITION
#define STATUS_INVALID_DISPOSITION       ((LONG)0xC0000026L)    // winnt
#endif
#ifndef STATUS_BUFFER_OVERFLOW
#define STATUS_BUFFER_OVERFLOW           ((LONG)0x80000005L)
#endif
#ifndef STATUS_FILE_LOCK_CONFLICT
#define STATUS_FILE_LOCK_CONFLICT ((LONG)0xC0000054)
#endif
#ifndef STATUS_INVALID_FILE_FOR_SECTION
#define STATUS_INVALID_FILE_FOR_SECTION ((LONG)0xC0000020)
#endif
#ifndef STATUS_INVALID_PAGE_PROTECTION
#define STATUS_INVALID_PAGE_PROTECTION ((LONG)0xC0000045)
#endif
#ifndef STATUS_MAPPED_FILE_SIZE_ZERO
#define STATUS_MAPPED_FILE_SIZE_ZERO ((LONG)0xC000011E)
#endif
#ifndef STATUS_SECTION_TOO_BIG
#define STATUS_SECTION_TOO_BIG ((LONG)0xC0000040)
#endif


#define NT_SUCCESS(Status) (((LONG)(Status)) >= 0)
#define NT_INFORMATION(Status) ((((ULONG)(Status)) >> 30) == 1)
#define NT_WARNING(Status) ((((ULONG)(Status)) >> 30) == 2)
#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)
#define PTR_ADD_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) + (ULONG_PTR)(Offset)))
#define PTR_SUB_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) - (ULONG_PTR)(Offset)))
#define ALIGN_UP_BY(Address, Align) (((ULONG_PTR)(Address) + (Align) - 1) & ~((Align) - 1))
#define ALIGN_UP_POINTER_BY(Pointer, Align) ((PVOID)ALIGN_UP_BY(Pointer, Align))
#define ALIGN_UP(Address, Type) ALIGN_UP_BY(Address, sizeof(Type))
#define ALIGN_UP_POINTER(Pointer, Type) ((PVOID)ALIGN_UP(Pointer, Type))
#define ALIGN_DOWN_BY(Address, Align) ((ULONG_PTR)(Address) & ~((ULONG_PTR)(Align) - 1))
#define ALIGN_DOWN_POINTER_BY(Pointer, Align) ((PVOID)ALIGN_DOWN_BY(Pointer, Align))
#define ALIGN_DOWN(Address, Type) ALIGN_DOWN_BY(Address, sizeof(Type))
#define ALIGN_DOWN_POINTER(Pointer, Type) ((PVOID)ALIGN_DOWN(Pointer, Type))
	/**
		* Gets a pointer to the first process information structure in a buffer returned by
		* PhEnumProcesses().
		*
		* \param Processes A pointer to a buffer returned by PhEnumProcesses().
		*/
#define SK_FIRST_PROCESS(Processes) ((shared::nt::PSYSTEM_PROCESS_INFORMATION)(Processes))

		/**
			* Gets a pointer to the process information structure after a given structure.
			*
			* \param Process A pointer to a process information structure.
			*
			* \return A pointer to the next process information structure, or NULL if there are no more.
			*/
#define SK_NEXT_PROCESS(Process) ( \
    ((shared::nt::PSYSTEM_PROCESS_INFORMATION)(Process))->NextEntryOffset ? \
    (shared::nt::PSYSTEM_PROCESS_INFORMATION)PTR_ADD_OFFSET((Process), \
    ((shared::nt::PSYSTEM_PROCESS_INFORMATION)(Process))->NextEntryOffset) : \
    NULL \
    )

#define SK_PROCESS_EXTENSION(Process) \
    ((shared::nt::PSYSTEM_PROCESS_INFORMATION_EXTENSION)PTR_ADD_OFFSET((Process), \
    UFIELD_OFFSET(shared::nt::SYSTEM_PROCESS_INFORMATION, Threads) + \
    sizeof(shared::nt::SYSTEM_THREAD_INFORMATION) * \
    ((shared::nt::PSYSTEM_PROCESS_INFORMATION)(Process))->NumberOfThreads))

#define SK_EXTENDED_PROCESS_EXTENSION(Process) \
    ((shared::nt::PSYSTEM_PROCESS_INFORMATION_EXTENSION)PTR_ADD_OFFSET((Process), \
    UFIELD_OFFSET(shared::nt::SYSTEM_PROCESS_INFORMATION, Threads) + \
    sizeof(shared::nt::SYSTEM_EXTENDED_THREAD_INFORMATION) * \
    ((shared::nt::PSYSTEM_PROCESS_INFORMATION)(Process))->NumberOfThreads))

#define DOS_MAX_COMPONENT_LENGTH 255
#define DOS_MAX_PATH_LENGTH (DOS_MAX_COMPONENT_LENGTH + 5)

#define SK_PROCESSID(handle) static_cast<DWORD>(reinterpret_cast<size_t>(handle))
#define SK_THREADID(handle) static_cast<DWORD>(reinterpret_cast<size_t>(handle))

#define WINDOWS_ANCIENT 0
#define WINDOWS_XP 51 // August, 2001
#define WINDOWS_SERVER_2003 52 // April, 2003
#define WINDOWS_VISTA 60 // November, 2006
#define WINDOWS_7 61 // July, 2009
#define WINDOWS_8 62 // August, 2012
#define WINDOWS_8_1 63 // August, 2013
#define WINDOWS_10 100 // TH1 // July, 2015
#define WINDOWS_10_TH2 101 // November, 2015
#define WINDOWS_10_RS1 102 // August, 2016
#define WINDOWS_10_RS2 103 // April, 2017
#define WINDOWS_10_RS3 104 // October, 2017
#define WINDOWS_10_RS4 105 // April, 2018
#define WINDOWS_10_RS5 106 // November, 2018
#define WINDOWS_10_19H1 107 // May, 2019
#define WINDOWS_10_19H2 108 // November, 2019
#define WINDOWS_10_20H1 109 // May, 2020
#define WINDOWS_10_20H2 110 // October, 2020
#define WINDOWS_10_21H1 111 // May, 2021
#define WINDOWS_11 112 // October, 2021
#define WINDOWS_NEW ULONG_MAX

		enum struct EnWindowsVersion : unsigned long {
			EN_WINDOWS_ANCIENT = 0,
			EN_WINDOWS_XP = 51,//=August,=2001
			EN_WINDOWS_SERVER_2003 = 52,//=April,=2003
			EN_WINDOWS_VISTA = 60,//=November,=2006
			EN_WINDOWS_7 = 61,//=July,=2009
			EN_WINDOWS_8 = 62,//=August,=2012
			EN_WINDOWS_8_1 = 63,//=August,=2013
			EN_WINDOWS_10 = 100,//=TH1,//=July,=2015
			EN_WINDOWS_10_TH2 = 101,//=November,=2015
			EN_WINDOWS_10_RS1 = 102,//=August,=2016
			EN_WINDOWS_10_RS2 = 103,//=April,=2017
			EN_WINDOWS_10_RS3 = 104,//=October,=2017
			EN_WINDOWS_10_RS4 = 105,//=April,=2018
			EN_WINDOWS_10_RS5 = 106,//=November,=2018
			EN_WINDOWS_10_19H1 = 107,//=May,=2019
			EN_WINDOWS_10_19H2 = 108,//=November,=2019
			EN_WINDOWS_10_20H1 = 109,//=May,=2020
			EN_WINDOWS_10_20H2 = 110,//=October,=2020
			EN_WINDOWS_10_21H1 = 111,//=May,=2021
			EN_WINDOWS_11 = 112,//=October,=2021
			EN_WINDOWS_NEW = ULONG_MAX,
		};


#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
#define ZwCurrentProcess() NtCurrentProcess()
#define NtCurrentThread() ((HANDLE)(LONG_PTR)-2)
#define ZwCurrentThread() NtCurrentThread()
#define NtCurrentSession() ((HANDLE)(LONG_PTR)-3)
#define ZwCurrentSession() NtCurrentSession()

		// Windows 8 and above
#define NtCurrentProcessToken() ((HANDLE)(LONG_PTR)-4) // NtOpenProcessToken(NtCurrentProcess())
#define NtCurrentThreadToken() ((HANDLE)(LONG_PTR)-5) // NtOpenThreadToken(NtCurrentThread())
#define NtCurrentThreadEffectiveToken() ((HANDLE)(LONG_PTR)-6) // NtOpenThreadToken(NtCurrentThread()) + NtOpenProcessToken(NtCurrentProcess())

#define NtCurrentSilo() ((HANDLE)(LONG_PTR)-1)
#if 0
#define NtCurrentPeb() (::NtCurrentTeb()->ProcessEnvironmentBlock)
// Not NT, but useful.
#define NtCurrentProcessId() (::NtCurrentTeb()->ClientId.UniqueProcess)
#define NtCurrentThreadId() (::NtCurrentTeb()->ClientId.UniqueThread)
#endif
//!@----------------------------------------------------------------------------------------------------------------------
		using tfNtAllocateVirtualMemory =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle,
				_Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID* BaseAddress,
				_In_ ULONG_PTR ZeroBits,
				_Inout_ PSIZE_T RegionSize,
				_In_ ULONG AllocationType,
				_In_ ULONG Protect
				);

		using tfNtFreeVirtualMemory =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle,
				_Inout_ PVOID* BaseAddress,
				_Inout_ PSIZE_T RegionSize,
				_In_ ULONG FreeType
				);

		using tfNtReadVirtualMemory =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle,
				_In_opt_ PVOID BaseAddress,
				_Out_writes_bytes_(BufferSize) PVOID Buffer,
				_In_ SIZE_T BufferSize,
				_Out_opt_ PSIZE_T NumberOfBytesRead
				);

		using tfNtWriteVirtualMemory =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle,
				_In_opt_ PVOID BaseAddress,
				_In_reads_bytes_(BufferSize) PVOID Buffer,
				_In_ SIZE_T BufferSize,
				_Out_opt_ PSIZE_T NumberOfBytesWritten
				);

		using tfNtProtectVirtualMemory =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle,
				_Inout_ PVOID* BaseAddress,
				_Inout_ PSIZE_T RegionSize,
				_In_ ULONG NewProtect,
				_Out_ PULONG OldProtect
				);

		using tfNtQueryVirtualMemory =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle,
				_In_opt_ PVOID BaseAddress,
				_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
				_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
				_In_ SIZE_T MemoryInformationLength,
				_Out_opt_ PSIZE_T ReturnLength
				);

		using tfNtFlushVirtualMemory =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE ProcessHandle,
				_Inout_ PVOID* BaseAddress,
				_Inout_ PSIZE_T RegionSize,
				_Out_ struct _IO_STATUS_BLOCK* IoStatus
				);

		using SK_ENUM_PROCESS_MODULES_CALLBACK = BOOLEAN(__stdcall*)(
			_In_ HANDLE ProcessHandle,
			_In_ PLDR_DATA_TABLE_ENTRY Entry,
			_In_ PVOID AddressOfEntry,
			_In_opt_ PVOID Context1,
			_In_opt_ PVOID Context2
			);

		using tfLdrLoadDll =
			LONG
			(__stdcall*)
			(
				_In_opt_ PWSTR DllPath,
				_In_opt_ PULONG DllCharacteristics,
				_In_ PUNICODE_STRING DllName,
				_Out_ PVOID* DllHandle
				);

		using tfLdrUnloadDll =
			LONG
			(__stdcall*)
			(
				_In_ PVOID DllHandle
				);

		using tfLdrGetDllHandle =
			LONG
			(__stdcall*)
			(
				_In_opt_ PWSTR DllPath,
				_In_opt_ PULONG DllCharacteristics,
				_In_ PUNICODE_STRING DllName,
				_Out_ PVOID* DllHandle
				);

		using tfRtlGetVersion =
			LONG
			(__stdcall*)(
				_Out_ PRTL_OSVERSIONINFOEXW VersionInformation // PRTL_OSVERSIONINFOW
				);

		using tfRtlGetCurrentPeb = PPEB(__stdcall*)	(VOID);
		using tfRtlAcquirePebLock = VOID(__stdcall*)	(VOID);
		using tfRtlReleasePebLock = VOID(__stdcall*)	(VOID);
		using tfRtlInitializeCriticalSection = LONG(__stdcall*)(_Out_ PRTL_CRITICAL_SECTION CriticalSection);
		using tfRtlInitializeCriticalSectionAndSpinCount =
			LONG
			(__stdcall*)(
				_Inout_ PRTL_CRITICAL_SECTION CriticalSection,
				_In_ ULONG SpinCount
				);

		using tfRtlDeleteCriticalSection =
			LONG
			(__stdcall*)(
				_Inout_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlEnterCriticalSection =
			LONG
			(__stdcall*)(
				_Inout_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlLeaveCriticalSection =
			LONG
			(__stdcall*)(
				_Inout_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlTryEnterCriticalSection =
			LOGICAL
			(__stdcall*)(
				_Inout_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlIsCriticalSectionLocked =
			LOGICAL
			(__stdcall*)(
				_In_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlIsCriticalSectionLockedByThread =
			LOGICAL
			(__stdcall*)(
				_In_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlGetCriticalSectionRecursionCount =
			ULONG
			(__stdcall*)(
				_In_ PRTL_CRITICAL_SECTION CriticalSection
				);

		using tfRtlSetCriticalSectionSpinCount =
			ULONG
			(__stdcall*)
			(
				_Inout_ PRTL_CRITICAL_SECTION CriticalSection,
				_In_ ULONG SpinCount
				);


		using tfNtQuerySystemInformation = LONG(__stdcall*)
			(
				_In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
				_Out_writes_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
				_In_ ULONG SystemInformationLength,
				_Out_opt_ PULONG ReturnLength
				);

		using tfNtOpenProcess = LONG(__stdcall*)
			(
				_Out_ PHANDLE ProcessHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_ POBJECT_ATTRIBUTES ObjectAttributes,
				_In_opt_ PCLIENT_ID ClientId
				);

		using tfNtClose = LONG(__stdcall*)
			(
				_In_ _Post_ptr_invalid_ HANDLE Handle
				);

		using tfNtTerminateProcess = LONG(__stdcall*)
			(
				_In_opt_ HANDLE ProcessHandle,
				_In_ LONG ExitStatus
				);

		using tfNtSuspendProcess =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle
				);

		using tfNtResumeProcess =
			LONG
			(__stdcall*)(
				_In_ HANDLE ProcessHandle
				);

		using tfRtlCreateUserProcess =
			LONG
			(__stdcall*)(
				_In_ PUNICODE_STRING NtImagePathName,
				_In_ ULONG AttributesDeprecated,
				_In_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
				_In_opt_ PSECURITY_DESCRIPTOR ProcessSecurityDescriptor,
				_In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
				_In_opt_ HANDLE ParentProcess,
				_In_ BOOLEAN InheritHandles,
				_In_opt_ HANDLE DebugPort,
				_In_opt_ HANDLE TokenHandle, // used to be ExceptionPort
				_Out_ PRTL_USER_PROCESS_INFORMATION ProcessInformation
				);

		using tfNtQueryInformationProcess = LONG(__stdcall*)
			(
				_In_ HANDLE ProcessHandle,
				_In_ PROCESSINFOCLASS ProcessInformationClass,
				_Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
				_In_ ULONG ProcessInformationLength,
				_Out_opt_ PULONG ReturnLength
				);

		using tfRtlAllocateHeap = PVOID(__stdcall*)
			(
				_In_ PVOID  HeapHandle,
				_In_opt_ ULONG  Flags,
				_In_ SIZE_T Size
				);

		using tfRtlCreateUnicodeString =
			BOOLEAN
			(__stdcall*)(
				_Out_ PUNICODE_STRING DestinationString,
				_In_z_ PCWSTR SourceString
				);

		using tfRtlInitUnicodeString =
			VOID
			(__stdcall*)(
				_Out_ PUNICODE_STRING DestinationString,
				_In_opt_z_ PCWSTR SourceString
				);

		using tfRtlFreeUnicodeString =
			VOID
			(__stdcall*)
			(
				_Inout_ _At_(UnicodeString->Buffer, _Frees_ptr_opt_) PUNICODE_STRING UnicodeString
				);


		typedef
			VOID(__stdcall* PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION)(
				_In_ LDR_DATA_TABLE_ENTRY* DataTableEntry,
				_In_ PVOID Context,
				_Inout_ BOOLEAN* StopEnumeration
				);
		using tfLdrEnumerateLoadedModules =
			LONG(__stdcall*)(
				_In_opt_ ULONG Flags,
				_In_ PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION CallbackFunction,
				_In_opt_ PVOID Context);

		using tfNtCreateToken =
			LONG
			(__stdcall*)
			(
				_Out_ PHANDLE TokenHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
				_In_ TOKEN_TYPE Type,
				_In_ PLUID AuthenticationId,
				_In_ PLARGE_INTEGER ExpirationTime,
				_In_ PTOKEN_USER User,
				_In_ PTOKEN_GROUPS Groups,
				_In_ PTOKEN_PRIVILEGES Privileges,
				_In_opt_ PTOKEN_OWNER Owner,
				_In_ PTOKEN_PRIMARY_GROUP PrimaryGroup,
				_In_opt_ PTOKEN_DEFAULT_DACL DefaultDacl,
				_In_ PTOKEN_SOURCE Source
				);

		using tfNtOpenProcessToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE ProcessHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_Out_ PHANDLE TokenHandle
				);

		using tfNtOpenProcessTokenEx =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE ProcessHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_ ULONG HandleAttributes,
				_Out_ PHANDLE TokenHandle
				);

		using tfNtOpenThreadToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE ThreadHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_ BOOLEAN OpenAsSelf,
				_Out_ PHANDLE TokenHandle
				);

		using tfNtOpenThreadTokenEx =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE ThreadHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_ BOOLEAN OpenAsSelf,
				_In_ ULONG HandleAttributes,
				_Out_ PHANDLE TokenHandle
				);

		using tfNtDuplicateToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE ExistingTokenHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
				_In_ BOOLEAN EffectiveOnly,
				_In_ TOKEN_TYPE Type,
				_Out_ PHANDLE NewTokenHandle
				);

		using tfNtQueryInformationToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE TokenHandle,
				_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
				_Out_writes_bytes_to_opt_(TokenInformationLength, *ReturnLength) PVOID TokenInformation,
				_In_ ULONG TokenInformationLength,
				_Out_ PULONG ReturnLength
				);

		using tfNtSetInformationToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE TokenHandle,
				_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
				_In_reads_bytes_(TokenInformationLength) PVOID TokenInformation,
				_In_ ULONG TokenInformationLength
				);

		using tfNtAdjustPrivilegesToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE TokenHandle,
				_In_ BOOLEAN DisableAllPrivileges,
				_In_opt_ PTOKEN_PRIVILEGES NewState,
				_In_ ULONG BufferLength,
				_Out_writes_bytes_to_opt_(BufferLength, *ReturnLength) PTOKEN_PRIVILEGES PreviousState,
				_Out_opt_ PULONG ReturnLength
				);

		using tfNtAdjustGroupsToken =
			LONG
			(__stdcall*)
			(
				_In_ HANDLE TokenHandle,
				_In_ BOOLEAN ResetToDefault,
				_In_opt_ PTOKEN_GROUPS NewState,
				_In_opt_ ULONG BufferLength,
				_Out_writes_bytes_to_opt_(BufferLength, *ReturnLength) PTOKEN_GROUPS PreviousState,
				_Out_opt_ PULONG ReturnLength
				);

		using tfZwOpenKey = LONG(__stdcall*)(
			_Out_ PHANDLE            KeyHandle,
			_In_  ACCESS_MASK        DesiredAccess,
			_In_  POBJECT_ATTRIBUTES ObjectAttributes
			);

		using tfZwCreateKey = LONG(__stdcall*)(
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			_In_ ULONG TitleIndex,
			_In_opt_  PUNICODE_STRING    Class,
			_In_ ULONG CreateOptions,
			_Out_opt_ PULONG Disposition
			);

		using tfZwQueryKey = LONG(__stdcall*)(
			_In_           HANDLE                KeyHandle,
			_In_           KEY_INFORMATION_CLASS KeyInformationClass,
			_In_opt_ PVOID                 KeyInformation,
			_In_        ULONG                 Length,
			_Out_         PULONG                ResultLength
			);

		using tfZwQueryValueKey = LONG(__stdcall*)(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_opt_ PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
			);

		typedef
			VOID
			(__stdcall* PIO_APC_ROUTINE) (
				_In_ PVOID ApcContext,
				_In_ PIO_STATUS_BLOCK IoStatusBlock,
				_In_ ULONG Reserved
				);

		VOID __stdcall GotIt(
			_In_ PVOID ApcContext,
			_In_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ ULONG Reserved
		);

		using tfZwCreateKey = NTSTATUS(__stdcall*) (
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			ULONG TitleIndex,
			_In_opt_  PUNICODE_STRING Class,
			_In_ ULONG CreateOptions,
			_Out_opt_ PULONG Disposition
			);
		using tfZwQueryKey = NTSTATUS(__stdcall*)(
			_In_            HANDLE                KeyHandle,
			_In_           shared::nt::KEY_INFORMATION_CLASS KeyInformationClass,
			_Out_opt_ PVOID                 KeyInformation,
			_In_          ULONG                 Length,
			_Out_           PULONG                ResultLength
			);

		using tfZwNotifyChangeKey = LONG(__stdcall*)(
			_In_            HANDLE           KeyHandle,
			_In_opt_  HANDLE           Event,
			_In_opt_  PIO_APC_ROUTINE  ApcRoutine,
			_In_opt_  PVOID            ApcContext,
			_Out_           PIO_STATUS_BLOCK IoStatusBlock,
			_In_            ULONG            CompletionFilter,
			_In_            BOOLEAN          WatchTree,
			_Out_opt_ PVOID            Buffer,
			_In_            ULONG            BufferSize,
			_In_            BOOLEAN          Asynchronous
			);

		using tfZwFlushKey = LONG(__stdcall*)(
			_In_ HANDLE KeyHandle
			);

		using tfZwSetValueKey = LONG(__stdcall*)(
			_In_           HANDLE          KeyHandle,
			_In_           PUNICODE_STRING ValueName,
			_In_opt_ ULONG           TitleIndex,
			_In_           ULONG           Type,
			_In_opt_ PVOID           Data,
			_In_           ULONG           DataSize
			);

		using tfZwDeleteKey = LONG(__stdcall*)(
			_In_ HANDLE KeyHandle
			);

		using tfZwDeleteValueKey = LONG(__stdcall*)(
			_In_ HANDLE          KeyHandle,
			_In_ PUNICODE_STRING ValueName
		);

		using tfZwEnumerateKey = LONG(__stdcall*)(
			_In_            HANDLE                KeyHandle,
			_In_            ULONG                 Index,
			_In_            KEY_INFORMATION_CLASS KeyInformationClass,
			_Out_opt_ PVOID                 KeyInformation,
			_In_            ULONG                 Length,
			_Out_           PULONG                ResultLength
			);

		using tfZwEnumerateValueKey = LONG(__stdcall*) (
			_In_            HANDLE                      KeyHandle,
			_In_            ULONG                       Index,
			_In_            KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_opt_ PVOID                       KeyValueInformation,
			_In_            ULONG                       Length,
			_Out_           PULONG                      ResultLength
			);

		using tfZwSetValueKey = LONG(__stdcall*)  (
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_opt_ ULONG TitleIndex,
			_In_ ULONG Type,
			_In_opt_ PVOID Data,
			_In_ ULONG DataSize
			);

		using tfZwQueryValueKey = LONG(__stdcall*)(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_opt_ PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
			);

		using tfZwClose = LONG(__stdcall*)(
			_In_ HANDLE Handle
			);

		using tfNtCreateProcessEx = NTSTATUS(NTAPI*)(
			PHANDLE,
			ACCESS_MASK,
			POBJECT_ATTRIBUTES,
			HANDLE,
			ULONG,
			HANDLE,
			HANDLE,
			HANDLE,
			BOOLEAN);

		using tfNtCreateThreadEx = NTSTATUS(NTAPI*)(
			PHANDLE,
			ACCESS_MASK,
			POBJECT_ATTRIBUTES,
			HANDLE,
			PVOID,
			PVOID,
			ULONG,
			ULONG_PTR,
			SIZE_T,
			SIZE_T,
			PVOID);

		using tfRtlCreateProcessParametersEx = NTSTATUS(NTAPI*)(
			PRTL_USER_PROCESS_PARAMETERS*,
			PUNICODE_STRING,
			PUNICODE_STRING,
			PUNICODE_STRING,
			PUNICODE_STRING,
			PVOID,
			PUNICODE_STRING,
			PUNICODE_STRING,
			PUNICODE_STRING,
			PUNICODE_STRING,
			ULONG);

		typedef VOID(NTAPI* PUSER_THREAD_START_ROUTINE)(
			IN PVOID ApcArgument1
			);

		using tfRtlCreateUserThread = NTSTATUS(NTAPI*)(
				IN HANDLE Process,
				IN PSECURITY_DESCRIPTOR ThreadSecurityDescriptor OPTIONAL,
				IN BOOLEAN CreateSuspended,
				IN ULONG_PTR ZeroBits OPTIONAL,
				IN SIZE_T MaximumStackSize OPTIONAL,
				IN SIZE_T CommittedStackSize OPTIONAL,
				IN PUSER_THREAD_START_ROUTINE StartAddress,
				IN PVOID Parameter OPTIONAL,
				OUT PHANDLE Thread OPTIONAL,
				OUT PCLIENT_ID ClientId OPTIONAL
			);

		typedef struct _FILE_DISPOSITION_INFORMATION {
			BOOLEAN DeleteFile;
		} FILE_DISPOSITION_INFORMATION, * PFILE_DISPOSITION_INFORMATION;

#pragma pack(push,1)
		//!@ Local data routing protocol
		//! 
		//! 
		//! 

		typedef struct tagLocalRouteDataInfo final {
			unsigned long data_identify;
			unsigned long data_postion;
			unsigned long data_size;

			tagLocalRouteDataInfo() { ::memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagLocalRouteDataInfo& obj) { ::memcpy(this, &obj, sizeof(*this)); }
		}ROUTEDATAINFO, * PROUTEDATAINFO;

		typedef struct tagLocalRouteData final {
			unsigned long flag_head;
			unsigned long data_count;
			unsigned long data_size;
			unsigned long flag_tail;

			tagLocalRouteData() {
				::memset(this, 0x00, sizeof(*this));
				flag_head = 0xFEFF;
				flag_tail = 0x200B;
			}
			void operator=(const tagLocalRouteData& obj) {
				::memcpy(this, &obj, sizeof(*this));
				flag_head = 0xFEFF;
				flag_tail = 0x200B;
			}
			bool Verify() const { return flag_head == 0xFEFF && flag_tail == 0x200B; }
		}LocalRouteData, LOCALROUTEDATA, * PLOCALROUTEDATA, LOCALDATA, * PLOCALDATA, ROUTEDATAHEAD, * PROUTEDATAHEAD;

#pragma pack(pop)

	}///namespace nt

	namespace kernel32 {

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

		//CreateProcessInternalW

	}///namespace kernel32

}///namespace shared

/// /*新生®（上海）**/
/// /*2022_02_19T06:04:25.7869608Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___8490570A_162F_4486_875D_C86829FB75D8__HEAD__