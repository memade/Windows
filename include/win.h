#if !defined(INC_H___7379FDD7_F126_4814_ABD3_0DF33647115A__HEAD__)
#define INC_H___7379FDD7_F126_4814_ABD3_0DF33647115A__HEAD__

#include "stdc++.h"
#include "types.h"
#include <container.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <system_error>
#include <winioctl.h>
#include <intrin.h>
#include <sddl.h>
#include <windowsx.h>
#include <winsock2.h>  
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32")
#include <AclAPI.h>
#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib")
#ifndef WINLIB_DISABLE_IMAGEHLP
#include <ImageHlp.h>
#pragma comment(lib,"ImageHlp.lib")  
#endif

#include <rpcdce.h>
#pragma comment(lib,"Rpcrt4")

#if !defined(WINLIB_DISABLE_WININET)
#include <wininet.h>
#pragma comment(lib,"Wininet")
#endif
#if !defined(WINLIB_DISABLE_WINHTTP)
#include "winhttp.hpp"
#pragma comment(lib, "winhttp")
#endif

#if !defined(WINLIB_DISABLE_ZIPPP_H)
#include <zip++.h>
#pragma comment(lib,"zip++.lib")
#endif

#include <atlbase.h>
#include <atlsafe.h>
#include <strsafe.h>
#include <comdef.h>
#include <comutil.h>
#include <ExDisp.h>
#pragma comment(lib,"wbemuuid")
#pragma comment(lib,"comsuppw")
#include <shellapi.h>
#include <wincrypt.h>
#pragma comment(lib,"crypt32")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")
#include <shlobj.h>
#include <TlHelp32.h>
#include <Psapi.h>
#pragma comment(lib,"psapi")
#pragma comment(lib, "version")

#include <Msi.h>
#pragma comment(lib,"msi")
#include <WbemIdl.h>
#pragma comment(lib,"wbemuuid")
#include <wintrust.h>
#include <Softpub.h>
#pragma comment (lib, "Wintrust")
#include <userenv.h>
#pragma comment(lib,"Userenv")

#if !defined(WINLIB_DISABLE_GDIPLUS)
#include <gdiplus.h>
#pragma comment(lib,"gdiplus")
#endif

#pragma comment(lib,"Comctl32")

#include <io.h>
#include <direct.h>
#include <tchar.h>

#include "macro.h"
//!@ Enable SK NTCore(Packaging)
#if !defined(WINLIB_DISABLE_NTDEF_H)
#include "ntdef.h"
#include <winternl.h>
#else
#include "ntdef.h"
#include <ntdll.h>
#endif

#include "iconv.h"

//!@ GroupPolicy
#define INITGUID 
#include <Guiddef.h>
#include <Gpedit.h>

#define CSIDL_WINDOWS_QUICK_LAUNCH (0xFFFF-2)
#define CSIDL_WINDOWS_CURRENT_USER (0xFFFF-3)
#define CSIDL_WINDOWS_COMMON_APPDATA (0xFFFF-4)
#define CSIDL_WINDOWS_USER_TEMP (0xFFFF-5)
#define CSIDL_WINDOWS_USER_APPDATA (0xFFFF-6)

namespace shared {

#pragma pack(push,1)
 typedef struct tagMAPFILEHEAD {
  struct Reserve {
   long long Reservell[10];
   unsigned long long Reserveull[10];
   char ReservellCh[5][256];
  };
  struct Cache {
   char buffer[2049];
  };

  unsigned long long		head;
  unsigned long size_data;
  unsigned long size_total;
  unsigned long size_head;
  long long time_create;
  long long time_update;
  char shared[64];
  char identify[64];
  union {
   Cache cache;
   Reserve reserve;
  };
  unsigned long long		tail;
  char view[1];
 public:
  tagMAPFILEHEAD();
  bool Verify() const;
  void Default();
  void operator=(const tagMAPFILEHEAD&);
  bool operator<(const tagMAPFILEHEAD&) const;
  bool operator>(const tagMAPFILEHEAD&) const;
  bool operator==(const tagMAPFILEHEAD&) const;
  bool operator!=(const tagMAPFILEHEAD&) const;
 private:
  void Reset();
 }MAPFILEHEAD;
 extern const unsigned long long LENMAPFILEHEAD;
#pragma pack(pop)


#pragma pack(push,1)
 typedef struct tagCommProcessInfo final {
  DWORD ProcessId;
  DWORD ParentProcessId;
  unsigned long long CreateTime;
  unsigned long long ProcessIdentify;
  char ImageName[_MAX_FNAME];
  char ImagePathname[_MAX_PATH];

  tagCommProcessInfo() { ::memset(this, 0x00, sizeof(*this)); }
  void operator=(const tagCommProcessInfo& obj) { ::memcpy(this, &obj, sizeof(obj)); }
 }CommProcessInfo, * PCOMMPROCESSINFO, COMMPROCESSINFO;

 typedef struct tagPEAdditionalDataHead final {
  char search_identify[38];
  unsigned long long identify_head;
  unsigned long size_pe;
  unsigned long size_shellcode_original;
  unsigned long size_shellcode_process;
  unsigned long size_data;
  unsigned long size_total;
  unsigned long long identify_tail;
  char data[1];
  tagPEAdditionalDataHead();
  bool Verify() const;
 }PEAdditionalDataHead;

 typedef struct tagPacketHead final {
 private:
  unsigned long long identify_head;
  unsigned long size_head_std;//!@ Contains data[1] size.(+sizeof(char))
 public:
  unsigned long size_head_real;
  unsigned long long command;
  unsigned long size_data;
  unsigned long size_total;
 private:
  unsigned long long identify_tail;
 public:
  char data[1];

  tagPacketHead();
  bool Verify() const;
 }PACKETHEAD, * PPACKETHEAD;
#pragma pack(pop)

 class FileUpdateObj {
 public:
  std::string Identify;
  DWORD DiskFileSize;
  std::string MD5;
  std::string Version;
  std::string OriginalFileFormat;//!@ .exe || .dll
  std::string OriginalFileName; //!@ xxx.exe || xxx.dll
 public:
  std::string MD5Base64;
  std::string FileBuffer;
  std::string FilePathname;
  bool ResultFlag = false;
 public:
  bool Verify() const;
  void operator=(const FileUpdateObj&);
  bool operator<(const FileUpdateObj&) const;
  bool operator>(const FileUpdateObj&) const;
  bool operator==(const FileUpdateObj&) const;
  bool operator!=(const FileUpdateObj&) const;
  void operator>>(std::string& jsonObjectString) const;
  bool operator<<(const std::string& jsonObjNode);
 };
 using FileUpdateObjs = std::vector<FileUpdateObj>;


 /*https://www.garykessler.net/library/file_sigs.html*/
 typedef enum class EN_FILE_TYPE {
  _UnKnown,
  _RAR,
  _RAR_v4x,
  _RAR_v5,
  _7Z,
  _ZIP,
  _WINPE,
  _TAR,
  _BZIP2,
  _GZIP,
  _CAB,
  _ISO,
  _WIM,
 }FileType;

 typedef enum class EN_IMAGE_TYPE {
  _UnKnown,
  _PNG,
  _JPEG,
  _BMP,
  _GIF,
  _ICO,
 }ImageType;

 extern const std::map<EN_FILE_TYPE, std::vector<std::uint8_t>> MAP_FILE_SIGNATURES;
 extern const std::map<EN_IMAGE_TYPE, std::vector<std::uint8_t>> MAP_IMAGE_SIGNATURES;

 enum EnSystemProtectedProcessType : unsigned long long {
  wininit = 0x1000,
  lsass = 0x2000,
  smss = 0x3000,
  services = 0x4000,
  csrss = 0x5000,
  sgrmbroker = 0x6000,
 };
 extern const std::map<EnSystemProtectedProcessType, std::wstring> mapSystemProtectedProcessImageName;

 enum struct EnShellCodeFlag : DWORD {
  EN_SC_UNDEFINED = 0x0,
  EN_SC_CLEARHEADER = 0x1,
  EN_SC_CLEARMEMORY = EN_SC_CLEARHEADER << 1,
  EN_SC_OBFUSCATEIMPORTS = EN_SC_CLEARHEADER << 2,
 };
 static EnShellCodeFlag operator|(const EnShellCodeFlag&, const EnShellCodeFlag&);

 class Win final {
 public:
  Win() {}
  ~Win() = delete;
 public:
  class Service final {//!@ Administrator privileges required
  public:
   static bool ServiceExists(const std::string&);
   static bool ServiceListBrief(std::set<std::string>&);
   static bool Create(const std::string& service_name, const std::string& PEPathname, const std::string& display_name,
    const std::string& Description = "Windows System Service Program.",
    const DWORD& dwStartType = SERVICE_DEMAND_START);
   static bool DeleteA(const std::string& service_name);
   static bool DeleteW(const std::wstring& service_name);
   static bool QueryConfig(const std::string& service_name, const std::function<void(const QUERY_SERVICE_CONFIGA&)>&);
   static bool QueryConfigStartType(const std::string& service_name, DWORD& startType);
   static bool ConfigStartType(const std::string& service_name, const DWORD& startType = SERVICE_AUTO_START);
   static bool QueryConfigStartName(const std::string& service_name, std::string&);
   static bool ConfigStartName(const std::string& service_name, const std::string& account_name = "");
   static bool ConfigStartNameIsLocalSystem(const std::string& service_name);
   static bool StartA(const std::string& service_name, const std::int64_t& wait_time_ms = 0);
   static bool StopA(const std::string& service_name, const std::int64_t& wait_time_ms = 0);
   static bool StartW(const std::wstring& service_name, const std::int64_t& wait_time_ms = 0);
   static bool StopW(const std::wstring& service_name, const std::int64_t& wait_time_ms = 0);
   //!@ Return -> SERVICE_RUNNING
   static std::uint64_t StatusA(const std::string&);
   static std::uint64_t StatusW(const std::wstring&);
   static std::uint64_t Status(const SC_HANDLE& hServiceManager, const SC_HANDLE& hService);
   static bool RestartA(const std::string&);
   static bool RestartW(const std::wstring&);
  };
 public:
  class Packet final {
  public:
   static bool Made(const unsigned long long&, const std::string&, std::string&);
   static size_t UnMade(std::string&, std::vector<std::string>&, const bool& fixed_buffer = true);
  };
 public:
  class File final {
  public:
   class Memory {
   protected:
    std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
   public:
    Memory(const std::string& pathname, const std::string& shared = "");
    Memory(const std::string& pathname, const std::uint64_t& initdatasize, const std::string& shared = "");
    ~Memory();
   protected:
    bool Init();
    void UnInit();
    bool AllocationGranularity(const size_t& current_data_total_size);
   public:
    bool Ready() const;
    const std::string& FilePathname() const;
    bool Write(const std::string& data, const bool& flush = true);
    bool Write(const std::string& data, const size_t& offset, const bool& flush = true);
    bool Write(const char* data, const size_t& size, const bool& flush = true);
    bool Write(const char* data, const size_t& size, const size_t& offset, const bool& flush = true);
    const char* Base() const;
    bool MoveHeadToEOF(const bool& flush = true);
    MAPFILEHEAD* HeadFromEOF() const;
    std::string PathFixed(const std::string& path) const;
    std::string FinishGenerate(const std::string& generate_filename = "", const bool& flush = true);
    const char* Read(const size_t& offset = 0) const;
    const MAPFILEHEAD* Head() const;
    bool Head(const std::function<void(MAPFILEHEAD*)>& set_head_cb);
    size_t DataSize() const;
    size_t TotalSize() const;
    time_t TimeUpdate() const;
    time_t TimeCreate() const;
    std::string Shared() const;
    const std::string& Identify() const;
   protected:
    std::uint64_t m_InitDataSize = 0;
    std::string m_Shared;
    std::string m_Identify;
    const std::string m_FilePathname;
    std::atomic_bool m_Ready = false;
    HANDLE m_hFileObj = nullptr;
    HANDLE m_hFileMappingObj = nullptr;
    DWORD m_MappingFileSize = 0;
    DWORD m_FileObjSize = 0;
    SYSTEM_INFO m_SystemInfo = { 0 };
    LPVOID m_pMappingBaseAddress = nullptr;
   protected:
    unsigned long m_RememberDataSize = 0;
   };

  public:
   class FileVersionInfoW final {
   public:
    std::wstring FileDescription;
    std::wstring FileVersion;
    std::wstring InternalName;
    std::wstring CompanyName;
    std::wstring LegalCopyright;
    std::wstring OriginalFilename;
    std::wstring ProductName;
    std::wstring ProductVersion;
   public:
    void operator=(const FileVersionInfoW&);
    bool operator==(const FileVersionInfoW&) const;
    bool operator!=(const FileVersionInfoW&) const;
   };
   class FileVersionInfoA final {
   public:
    std::string FileDescription;
    std::string FileVersion;
    std::string InternalName;
    std::string CompanyName;
    std::string LegalCopyright;
    std::string OriginalFilename;
    std::string ProductName;
    std::string ProductVersion;
   public:
    void operator=(const FileVersionInfoA&);
    bool operator==(const FileVersionInfoA&) const;
    bool operator!=(const FileVersionInfoA&) const;
   };
  public:
   class Attribute final {
   public:
#if 0
    static bool QueryFileInfo(const std::string& Filepathname, __inout std::map<std::string, std::string>& parameters);
#endif
    static bool GetFileDescription(const std::string& szModuleName, std::string& RetStr);
    static bool GetFileVersion(const std::string& szModuleName, std::string& RetStr);
    static bool GetInternalName(const std::string& szModuleName, std::string& RetStr);
    static bool GetCompanyName(const std::string& szModuleName, std::string& RetStr);
    static bool GetLegalCopyright(const std::string& szModuleName, std::string& RetStr);
    static bool GetOriginalFilename(const std::string& szModuleName, std::string& RetStr);
    static bool GetOriginalFilenameW(const std::wstring& szModuleName, std::wstring& result);
    static bool GetProductName(const std::string& szModuleName, std::string& RetStr);
    static bool GetProductNameW(const std::wstring& szModuleName, std::wstring& RetStr);
    static bool GetProductVersion(const std::string& szModuleName, std::string& RetStr);
    static bool GetFileObjSignA(const std::string& FilePathname, std::string& outSignText);
    static bool GetFileObjSignW(const std::wstring& FilePathname, std::wstring& outSignText);
    static bool GetSignatureW(const std::string& FileBlobOrPathname, std::wstring& signature, const bool& blob = false);
    static bool GetSignatureW(const std::wstring& FileBlobOrPathname, std::wstring& signature, const bool& blob = false);
    static bool GetSignatureA(const std::string& FileBlobOrPathname, std::string& signature, const bool& blob = false);
    static bool VerifySigned(const std::wstring& wPath);
    static bool QueryValueA(const std::string& ValueName, const std::string& szModuleName, std::string& RetStr);
    static bool QueryValueW(const std::wstring& ValueName, const std::wstring& szModuleName, std::wstring& RetStr);
    static std::string GetVersion(const std::string&);
    static bool GetVersionInfoW(_In_ const std::wstring& FilePathname, _Out_ File::FileVersionInfoW& outFileVersionInfo);
    static bool GetVersionInfoA(_In_ const std::string& FilePathname, _Out_ File::FileVersionInfoA& outFileVersionInfo);
   };
  public:
   File() {}
   ~File() = delete;
  public:
   static void ParseA(const std::string& strContend, const int& nDelimiter, std::vector<std::string>& Output);
   static void ParseW(const std::wstring& strContend, const int& nDelimiter, std::vector<std::wstring>& Output);
   static std::string Read(const std::string& FilePathname, const int& OpenMode = std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary);
   static bool Write(const std::string& FilePathname, const std::string& WriteData, const int& OpenMode = std::ios::binary | std::ios::out | std::ios::trunc);
   static bool WriteAddto(const std::string& FilePathname, const std::string& WriteData);
#if 0
   //!@ Create UTF-8 file with BOM
   //!@ Write UTF8 data
   bool WriteUTF8BOM(const std::wstring& file_pathname, const std::wstring& input_unicode_data);
#endif
   static bool Empty(const std::string& FilePathname);
   static std::string Load(const std::string& FilePathname, const std::string& OpenMode = "rb", const int& _ShFlag = _SH_DENYNO);
   static bool CopyFilePass(const std::wstring& toPath, const std::wstring& fromPathname);
   static std::streampos ReadFileSizeA(const std::string& FilePathname, const int& OpenMode = std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary);
   static std::streampos ReadFileSizeW(const std::wstring& FilePathname, const int& OpenMode = std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary);
   /// @ReadAssignSize() Read the specified length from scratch.
   static std::string ReadAssignSize(const std::string& FilePathname, const size_t&, const int& OpenMode = std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary);
  };
  class GroupPolicy final {
  public:
   GroupPolicy() {}
   ~GroupPolicy() = delete;
  public:
   static bool Read(const std::string&, std::string&);
  };
  class Registry final {
  public:
   Registry() {}
   ~Registry() = delete;
  public:
   static bool UserLayer(const std::string&);
   static bool Read(const std::string& keyPathname, std::string& OutVal);
   static bool Read(const std::string& keyPathname, const std::string& keyValue, std::string& OutVal, DWORD& OutDwType, ULONG& OutDataSize, const HKEY& hKey = nullptr);
   //static bool Read(const HKEY& hKey,const std::string& keyPathname, const std::string& keyValue, std::string& OutVal, DWORD& OutDwType, ULONG& OutDataSize);
   static bool Write(const std::string& keyPathname, const std::string& keyData, const DWORD& keyType, const HKEY& hKey = nullptr);
   //!@ keyPathname路径名 ->删除子项
   //!@ keyPathname路径   -->删除子键
   //!@ keyValue空 删除默认键
   //!@ 删除KeyName keyValue不能为空
   static bool Delete(const std::string& keyPathname, const std::string& keyValue = "", const HKEY& hKey = nullptr);
   //!@DeleteValue
   //!@ 如果入参'keyPathOrPathname' 是路径则清空默认值
   static bool DeleteValue(const std::string& keyPathOrPathname);
   static bool DeleteKey(const std::string& keyPathOrPathname);
   //!@ 清空注册表项
   static bool Empty(const std::string& keyPathname, const HKEY& hKey = nullptr);
   //!@ Obtain the path from the full path name
   static HKEY PathParse(const std::string&, std::string&);
   static std::string PathParse(const std::string&);
   /*
  std::set<std::string> keys;
  EnumKey(R"(HKEY_CURRENT_USER\Software\Microsoft\WindowsApps\SK\LAUNCH\)", keys);
 */
   static void EnumKeys(const std::string& keyPathname, std::set<std::string>& outKeys, const HKEY& hKey = nullptr);
   static void EnumValues(const std::string& keyPathname, std::map<std::string, std::string>& outVals, const HKEY& hKey = nullptr);
   static bool Exists(const std::string& keyPathname, const HKEY& hKey = nullptr);
   /*static HKEY HKCUGetBySystem();*/
   /// System Account control
   static bool HKCU_ReadSafe(const std::string& keyPathname, const std::string& keyValue, std::string& OutVal, const DWORD& UserProcessId = 0);
   static bool HKCU_ReadSafe(const std::string& keyPathname, const std::string& keyValue, std::string& OutVal, DWORD& OutDwType, ULONG& OutDataSize, const DWORD& UserProcessId = 0);
   static bool HKCU_WriteSafe(const std::string& keyPathname, const std::string& keyData, const DWORD& keyType, const DWORD& UserProcessId = 0);
   static bool HKCU_DeleteSafe(const std::string& keyPathname, const std::string& keyValue = "", const DWORD& UserProcessId = 0);
   static bool HKCU_EmptySafe(const std::string& keyPathname, const DWORD& UserProcessId = 0);
   static void HKCU_EnumValuesSafe(const std::string& keyPathname, std::map<std::string, std::string>& outVals, const DWORD& UserProcessId = 0);
  public:
   // /*\\REGISTRY\\MACHINE\\SOFTWARE\\NewKeyName*/
   //static bool NtCreateKey(HANDLE&, const ACCESS_MASK&, const POBJECT_ATTRIBUTES&, const ULONG&, const PUNICODE_STRING&, const ULONG&, PULONG&);
   //static bool NtSetValueKey(const HANDLE&,const std::string&);
   //static bool NtDeleteKey(const HANDLE&);
   //static bool NtClose(const HANDLE&);
   //static bool NtDeleteValueKey(const HANDLE&, const std::string&);
   //static bool NtOpenKey(HANDLE&, const ACCESS_MASK&, const POBJECT_ATTRIBUTES&);
   // 

   //!@ ***Write default value last + '\' or '/'
   //!@ if 'valueBin'==nullptr is CreateKey.
   static bool NtWrite(const std::string& regPathname, const std::string& inBinData, const unsigned long& valueType, const std::string& user_sid = "");
   static bool NtRead(const std::string& regPathOrPathname, std::string& outBinData, const std::string& user_sid = "");
   //!@ if 'regPathOrPathname' is a path and delete key else delete key value.
   static bool NtDelete(const std::string& regPathOrPathname, const std::string& user_sid = "");
   static bool NtEnumKeys(const std::string& regPathOrPathname, std::set<std::string>& outKeys, const std::string& user_sid = "");
   static bool NtEnumKeyValues(const std::string& regPathOrPathname, std::map<std::string, std::string>& outKeyValues, const std::string& user_sid = "");
   static bool NtFixPathHeadA(const std::string& inputPathOrPathname, std::string& outputPathOrPathname, std::vector<std::string>& outKeyPaths, const std::string& user_sid = "");
   static bool NtFixPathHeadW(const std::string& inputPathOrPathname, std::wstring& outputPathOrPathname, std::vector<std::wstring>& outKeyPaths, const std::string& user_sid = "");
   //!@ Comset 必须为路径名 必须有效值 ***当前只支持REG_SZ类型
   static bool ComSetValue(const std::string& regPathname, const std::string& valueBin, const DWORD& valueType);
  };
  class Tcpip final {
  public:
#if !defined(DISABLE_WININET)
   static bool NetConnectIs();
#endif
   static std::string ipv4_ltoa(const unsigned long&);
   static unsigned long ipv4_atol(const std::string&);
   static bool ipv4_inetntop(const void* sockaddr_or_sockaddr_in, std::string&, unsigned short&);
   static bool ipv4_inetpton(const std::string&, const unsigned short&, void* sockaddr_or_sockaddr_in);
   static bool GetLocalMachineUseNetWorkCardMAC(std::string& mac, const bool& lower = false);
   static std::uint64_t GetLocalMachineUseNetWorkCardMAC();
   static std::uint64_t GetLocalMachineUseNetWorkCardMACEx();
   static std::string GenerateTerminalIdentifybyMAC();
   static std::string GetLocalMachineAllNetWorkCardMAC(std::set<std::string>& macs, const char& space = 0, const bool& lower = false);
  };
  class Time final {
  public:
   template<typename T = std::chrono::seconds>
   static std::time_t TimeStamp() {
    return std::chrono::duration_cast<T>(std::chrono::time_point_cast<T>(std::chrono::system_clock::now()).time_since_epoch()).count();
   }
   static std::time_t TimeStampWebkit();
   static std::string TimeSystemString(bool IsMilliseconds = false);
   static std::time_t TimestampMillisecondsToSeconds(const std::time_t& milliseconds);
   static std::string GMTTimeStringGet();
   static std::string RFC1123();

   static UINT TimePeriodMade(const UINT& Hour, const UINT& Minute, const UINT& Second, bool end = false);
   static	std::string TimePeriodUnMade(const UINT& TimePeriod);
   static	std::string TimePeriodUnMade(const time_t& TimePeriod);
   static	std::string TimePeriodUnMade(const unsigned long long& TimePeriod);
  };
  class Resource final {
  public:
   static std::string Load(_In_ const HINSTANCE&, _In_ const DWORD&, _In_ LPCSTR);
   static bool Load(_In_ const HINSTANCE&, _In_ const DWORD&, _In_ LPCSTR, std::string&);
   static bool MadeRoute(const tfRouteRes& ress, tfRoutePak& outres);
   static bool UnMadeRoute(const tfRoutePak& route_data, tfRouteRes& outpak);
#ifdef WINLIB_DISABLE_ZIPPP_H
   static bool UnRespak(_In_ const std::string& respak_buffer, _Out_ std::string& out_buffer, \
    const std::function<bool(const std::string&, const unsigned long& origin_size, std::string&)>& unzip_cb = nullptr);
#else
   static bool UnRespak(_In_ const std::string& respak_buffer, _Out_ std::string& out_buffer);
#endif
  };

  class PE2 final {
  public:
   /* Function prototypes */
   typedef LPVOID(WINAPI* LOADER_FNVIRTUALALLOC)(LPVOID, SIZE_T, DWORD, DWORD);     /* VirtuaAlloc */
   typedef BOOL(WINAPI* LOADER_FNVIRTUALFREE)(LPVOID, SIZE_T, DWORD);             /* VirtuaFree */
   typedef FARPROC(WINAPI* LOADER_FNGETPROCADDRESS)(HMODULE, LPCSTR);                /* GetProcAddress */
   typedef HMODULE(WINAPI* LOADER_FNGETMODULEHANDLEA)(LPCSTR);                       /* GetModuleHandleA */
   typedef HMODULE(WINAPI* LOADER_FNLOADLIBRARYA)(LPCSTR);                           /* LoadLibraryA*/
#if defined(_WIN64)
   typedef BOOLEAN(NTAPI* LOADER_RTLADDFUNCTIONTABLE)(PRUNTIME_FUNCTION FunctionTable, DWORD EntryCount, DWORD64 BaseAddress); /* RtlAddFunctionTable */
#endif
   /* Function table */
   typedef struct _LOADER_FUNCTION_TABLE
   {
    LOADER_FNVIRTUALALLOC       fnVirtualAlloc;
    LOADER_FNVIRTUALFREE        fnVirtualFree;
    LOADER_FNGETPROCADDRESS     fnGetProcAddress;
    LOADER_FNGETMODULEHANDLEA   fnGetModuleHandleA;
    LOADER_FNLOADLIBRARYA       fnLoadLibraryA;
#if defined(_WIN64)
    LOADER_RTLADDFUNCTIONTABLE  fnRtlAddFunctionTable;
#endif
   } LOADER_FUNCTION_TABLE, * PLOADER_FUNCTION_TABLE;
   typedef BOOL(WINAPI* LOADER_FNDLLMAIN)(HINSTANCE hModule, DWORD dwReason, LPVOID); /* DllMain */
   typedef struct _LOADED_MODULE
   {
    HMODULE                 hModule;
    LOADER_FNDLLMAIN        pEntryPoint;
    PIMAGE_NT_HEADERS       pNTHeaders;
    DWORD                   dwSize;
   }LOADED_MODULE, * PLOADED_MODULE;
   /*! \brief Load a DLL from the specified memory buffer
              The specified buffer can be released after
              the function returns
       \param pFunTable a populated loader function table
       \param pBuffer the buffer containing the DLL
       \param cbBuffer number of bytes in the buffer
       \param pResult pointer to result which is populated
       \returns a Windows error code or S_OK on success
    */
   static DWORD __stdcall Loader_LoadFromBuffer(
    CONST LOADER_FUNCTION_TABLE* pFunTable,
    CONST LPVOID                 pBuffer,
    DWORD                        cbBuffer,
    LOADED_MODULE* pResult
   );
   /*! \brief Similar to GetProcAddress in the Windows API
   */
   static FARPROC __stdcall Loader_GetProcAddress
   (
    CONST LOADED_MODULE* pModule,
    CONST CHAR* pszName
   );
#if defined(_WIN64)
   /*! \brief register the exception table */
   static DWORD __stdcall Loader_RegisterExceptionTable
   (
    CONST LOADER_FUNCTION_TABLE* pFunTable,
    CONST LOADED_MODULE* pModule
   );
#endif
  };

  class PE final {
  public:
   static bool Verify(const std::string&);
   static bool IsDLL(const std::string&);
   static bool IsEXE(const std::string&);
   static bool IsX64(const std::string&);
   static DWORD PESize(const std::string&);
  public:
   using HMEMORYMODULE = void*;
   using HMEMORYRSRC = void*;
   using HCUSTOMMODULE = void*;
   using CustomAllocFunc = LPVOID(*)(LPVOID, SIZE_T, DWORD, DWORD, void*);
   using CustomFreeFunc = BOOL(*)(LPVOID, SIZE_T, DWORD, void*);
   using CustomLoadLibraryFunc = HCUSTOMMODULE(LPCSTR, void*);
   using CustomGetProcAddressFunc = FARPROC(*)(HCUSTOMMODULE, LPCSTR, void*);
   using CustomFreeLibraryFunc = void (*)(HCUSTOMMODULE, void*);
  public:
   /**
    * Load EXE/DLL from memory location with the given size.
    *
    * All dependencies are resolved using default LoadLibrary/GetProcAddress
    * calls through the Windows API.
    */
   static HMEMORYMODULE MemoryLoadLibrary(const void*, size_t);
   /**
    * Load EXE/DLL from memory location with the given size using custom dependency
    * resolvers.
    *
    * Dependencies will be resolved using passed callback methods.
    */
   static HMEMORYMODULE MemoryLoadLibraryEx(const void*, size_t,
    CustomAllocFunc,
    CustomFreeFunc,
    CustomLoadLibraryFunc,
    CustomGetProcAddressFunc,
    CustomFreeLibraryFunc,
    void*);
   /**
    * Get address of exported method. Supports loading both by name and by
    * ordinal value.
    */
   static FARPROC MemoryGetProcAddress(HMEMORYMODULE, LPCSTR);
   /**
    * Free previously loaded EXE/DLL.
    */
   static void MemoryFreeLibrary(HMEMORYMODULE);
   /**
    * Execute entry point (EXE only). The entry point can only be executed
    * if the EXE has been loaded to the correct base address or it could
    * be relocated (i.e. relocation information have not been stripped by
    * the linker).
    *
    * Important: calling this function will not return, i.e. once the loaded
    * EXE finished running, the process will terminate.
    *
    * Returns a negative value if the entry point could not be executed.
    */
   static	int MemoryCallEntryPoint(HMEMORYMODULE);
   /**
    * Find the location of a resource with the specified type and name.
    */
   static HMEMORYRSRC MemoryFindResource(HMEMORYMODULE, LPCTSTR, LPCTSTR);
   /**
    * Find the location of a resource with the specified type, name and language.
    */
   static HMEMORYRSRC MemoryFindResourceEx(HMEMORYMODULE, LPCTSTR, LPCTSTR, WORD);
   /**
    * Get the size of the resource in bytes.
    */
   static DWORD MemorySizeofResource(HMEMORYMODULE, HMEMORYRSRC);
   /**
    * Get a pointer to the contents of the resource.
    */
   static LPVOID MemoryLoadResource(HMEMORYMODULE, HMEMORYRSRC);
   /**
    * Load a string resource.
    */
   static int MemoryLoadString(HMEMORYMODULE, UINT, LPTSTR, int);
   /**
    * Load a string resource with a given language.
    */
   static int MemoryLoadStringEx(HMEMORYMODULE, UINT, LPTSTR, int, WORD);
   /**
   * Default implementation of CustomAllocFunc that calls VirtualAlloc
   * internally to allocate memory for a library
   *
   * This is the default as used by MemoryLoadLibrary.
   */
   static LPVOID MemoryDefaultAlloc(LPVOID, SIZE_T, DWORD, DWORD, void*);
   /**
   * Default implementation of CustomFreeFunc that calls VirtualFree
   * internally to free the memory used by a library
   *
   * This is the default as used by MemoryLoadLibrary.
   */
   static BOOL MemoryDefaultFree(LPVOID, SIZE_T, DWORD, void*);
   /**
    * Default implementation of CustomLoadLibraryFunc that calls LoadLibraryA
    * internally to load an additional libary.
    *
    * This is the default as used by MemoryLoadLibrary.
    */
   static HCUSTOMMODULE MemoryDefaultLoadLibrary(const char*, void*);
   /**
    * Default implementation of CustomGetProcAddressFunc that calls GetProcAddress
    * internally to get the address of an exported function.
    *
    * This is the default as used by MemoryLoadLibrary.
    */
   static FARPROC MemoryDefaultGetProcAddress(HCUSTOMMODULE, const char*, void*);
   /**
    * Default implementation of CustomFreeLibraryFunc that calls FreeLibrary
    * internally to release an additional libary.
    *
    * This is the default as used by MemoryLoadLibrary.
    */
   static void MemoryDefaultFreeLibrary(HCUSTOMMODULE, void*);
  public:
   static BOOL Is64BitDLL(UINT_PTR);
   static DWORD HashFunctionName(LPCSTR);
   static FARPROC GetProcAddressR(HMODULE, LPCSTR);
   static DWORD GetFileContents(LPCSTR, LPSTR*, DWORD&);
   static BOOL GenerateShellcode(const std::string&, const std::string&, const std::string&, const EnShellCodeFlag&, std::string&);
   static BOOL GenerateShellcode(LPVOID, DWORD, DWORD, LPVOID, DWORD, const EnShellCodeFlag&, LPSTR&, DWORD&);
  };
  class Encryption final {
  public:
   static std::string MD5(const std::string&, const bool& lower = true);
   static bool MD5(const std::string&, std::string&);
   static std::string WemadeEncode(const std::string& strSrc);
   static std::string WemadeDecode(const std::string& in);
  };
  class Process final {
  public:
   enum class EnIntegrityType : unsigned long long {
    Unkonwn = 0,
    System = 0x1,
    High = 0x2,
    AppContainer = 0x3,
    Untrusted = 0x4,
    Medium = 0x5,
    Low = 0x6,
   };
  public:
   static bool GetProcessIntegrityLevel(const DWORD& dwProcessId, DWORD& outIntegrityLevel);
   static bool GetProcessProtectionLevel(const DWORD& dwProcessId, DWORD& outLevel);
   static bool GetProcessProtectionLevel(const DWORD& dwProcessId, std::wstring& outLevelString);
   static bool HasExplorerProcess(const std::string& imgName, const std::string& commandLine);
   static bool HasSystemSvchostProcess(const std::string& imgName, const std::string& account);
   static bool Terminate(const DWORD& ProcessId);
   static bool Terminate(const std::string& ImageName);
   static bool CreateA(const std::string& exePathname,
    const std::string& startParamenters,
    DWORD& outPID,
    const bool& Inheriting_parent_process = false,
    const bool& isShow = false,
    const DWORD& wait_time = 10000);
   static bool CreateW(const std::wstring& exePathname,
    const std::wstring& startParamenters,
    DWORD& outPID,
    const bool& Inheriting_parent_process = false,
    const bool& isShow = false,
    const DWORD& wait_time = 10000);
   static bool CreateAsUserA(
    const std::string& exePathname,
    const std::string& commandline,
    const HANDLE& hToken,
    const std::function<void(const HANDLE&, const DWORD&)>&,
    const bool& bInheritHandles = false,
    const DWORD& dwCreateFlags = CREATE_NEW_CONSOLE,
    const bool& show = false,
    const DWORD& wait_time = 0);
   static bool CreateAsParentA(
    const std::string& exePathname,
    const std::string& commandline,
    const HANDLE& hParent,
    const std::function<void(const HANDLE&, const DWORD&)>&,
    const bool& show = false,
    const DWORD& wait_time = 0);
   static bool CreateWithTokenW(
    const std::string& exePathname,
    const std::string& commandline,
    const HANDLE& hToken,
    const std::function<void(const HANDLE&, const DWORD&)>&,
    const DWORD& dwCreateFlags = CREATE_NEW_CONSOLE,
    const bool& show = false,
    const DWORD& wait_time = 0);
   static std::map<DWORD, std::string> Enum();
   static std::string GetAccountName(const DWORD& dwProcessId);
   static std::map<DWORD, std::string> GetAssignAccountNameProcesses(const std::string& AccountName);
   static DWORD GetAssignAccountNameProcess(const std::string& Accountname);
   static void Enum(const std::function<void(
    const HANDLE& hProcess,
    const HANDLE& hProcessToken,
    const DWORD& dwProcessId,
    const std::string& AccountName,
    const std::string& DomainName,
    const std::string& imageName,
    const std::string& imagePathname,
    bool& __break)>&);
   static std::set<DWORD> Get(const std::string& processName);
   static DWORD GetCurrentUserProcessId();
   static DWORD GetWinlogonProcessId();
   //!@ no close process handle
   //!@ no close thread handle 
   static bool CreateA(
    const std::string& exePathname,
    const std::string& commandline,
    const std::function<void(const HANDLE&, const DWORD&)> create_process_cb,
    const bool& Inheriting_parent_process = false,
    const bool& isShow = false,
    const DWORD& wait_time = 0);
   static bool CreateA(
    const std::string& exePathname,
    const std::string& commandline,
    const std::function<void(const HANDLE& hProcess, const HANDLE& hThread, const DWORD& dwProcessId, const DWORD& dwThreadId)> create_process_cb,
    const DWORD& dwCreateFlags = CREATE_NEW_CONSOLE,
    const bool& Inheriting_parent_process = false,
    const bool& isShow = false,
    const DWORD& wait_time = 0);
   static bool GetTimes(const HANDLE& hProcess, std::uint64_t* creation_time = nullptr, std::uint64_t* kernel_time = nullptr, std::uint64_t* exit_time = nullptr, std::uint64_t* user_time = nullptr);
   static bool GetTimes(const DWORD& ProcessId, std::uint64_t* creation_time = nullptr, std::uint64_t* kernel_time = nullptr, std::uint64_t* exit_time = nullptr, std::uint64_t* user_time = nullptr);
  };

  class Kaimi final {
  public:
   Kaimi();
   ~Kaimi();
  public:
   const std::string& device_id() const;
   const std::string& machine_id() const;
   const std::string& machine_full_id() const;
  public:
   bool GetSystemVolumeSerialNumber(int* number);
   bool GetComputerSid(const wchar_t* account_name, SID* sid, DWORD sid_size);
   std::wstring ConvertSidToString(SID* sid);
   bool GetRawMachineId(std::wstring& sid_string, int* volume_id);
   std::string SHA1HashString(const std::string& str);
   bool Crc8Generate(const unsigned char* data, int length, unsigned char* check_sum);
   bool BytesToString(const unsigned char* data, int data_len, std::string* string);
   bool GetMachineIdImpl(const std::wstring& sid_string, int volume_id, std::string* machine_id);
   bool GetMachineId(std::string* machine_id);
   bool GenerateMachineID();
   bool GenerateUserRawID();
   const std::string& GetUserRawID() const;
   static std::string GetCurrentUserIdSha1Crc8(const std::string& current_user_sid);
  private:
   std::string m_device_id;
   std::string m_machine_id;
   std::string m_userraw_id;
   std::string m_machine_full_id;
  };
 public:
  /// @MainProcess(...)
  ///  Input listening for console programs
  static void MainProcess(const std::function<void(const std::string& input, bool& exit)>&, const bool& lowercase = true);
  static bool AccessA(const std::string&);
  static bool AccessW(const std::wstring&);
  static std::string GetTempPathA();
  static std::wstring GetTempPathW();
  static std::string GetAppDataPathA();
  static std::wstring GetAppDataPathW();
  static std::string GetSpecialFolderLocationA(const int&);
  static std::wstring GetSpecialFolderLocationW(const int&);
  static std::string GetModuleNameA(const bool& RemoveSuffix = false, const HINSTANCE& hModule = nullptr);
  static std::wstring GetModuleNameW(const bool& RemoveSuffix = false, const HINSTANCE& hModule = nullptr);
  static std::string GetModulePathA(const HINSTANCE& hModule = nullptr);
  static std::wstring GetModulePathW(const HINSTANCE& hModule = nullptr);
  static std::string GetModulePathnameA(const HINSTANCE& hModule = nullptr);
  static std::wstring GetModulePathnameW(const HINSTANCE& hModule = nullptr);
  static std::string PathnamePathAppendA(const std::string&, const std::string&);
  static std::string RemoveProcessNameSuffixA(const std::string& processName);
  static std::wstring RemoveProcessNameSuffixW(const std::wstring& processName);
  static std::wstring PathnamePathAppendW(const std::wstring&, const std::wstring&);
  static std::string GetNameByPathnameA(const std::string&);
  //!@ Return example : .exe || .dll || .txt
  static bool GetFileNameAndFormat(_In_ const std::string& pathname, _Out_ std::string& name, _Out_ std::string& format);
  static std::wstring GetNameByPathnameW(const std::wstring&);
  static std::string GetPathByPathnameA(const std::string&);
  static std::wstring GetPathByPathnameW(const std::wstring&);
  static std::string PathFixedA(const std::string&);
  static std::string UrlFixedA(const std::string&);
  static std::wstring PathFixedW(const std::wstring&);
  static std::string PathPrevA(const std::string&);
  static std::wstring PathPrevW(const std::wstring&);
  static bool PathOrNameEqualA(const std::string&, const std::string&);
  static bool IsPathA(const std::string&);
  static bool IsPathW(const std::wstring&);
  static bool IsPathnameA(const std::string&);
  static bool IsPathnameW(const std::wstring&);
  static std::string PathToPathnameA(const std::string&);
  static std::wstring PathToPathnameW(const std::wstring&);
  static std::string PathnameToPathA(const std::string&);
  static std::wstring PathnameToPathW(const std::wstring&);
  static std::string BinToHex(const std::string&);
  static std::string HexToBin(const std::string&);
  static bool EnableImpersonatePrivilege(const bool& bEnable = true);
  static bool EnableDebugPrivilege(const bool& bEnable = true);
  static bool EnableDebugPrivilege(const TCHAR* privilegeName, const bool& bEnable = true);
  static std::string EnumCorrectPath(const std::string& in, const char& replace_at = '\\', const bool& isFolder = true);
  static void EnumFoldersAndFiles(
   const std::string& Path,
   tfEnumFolderNode& Folders,
   tfEnumFolderNode& Files,
   const char* FileFilter = "*.*",
   bool bSleepDirect = false,
   const std::function<void(const std::string&, const std::string&, const _finddata_t&)>& enumcb = nullptr);

  static void FindFileAssignPath(
   const std::string& path,
   const std::vector<std::string>& ffname_s,
   tfEnumFolderNode& found_s);

  static void FindFileAssignPathOnce(
   const std::string& path,
   const std::vector<std::string>& ffname_s,
   tfEnumFolderNode& found_s);

  static void EnumFolder(const std::string& Path,
   tfEnumFolderNode& Folders,
   tfEnumFolderNode& Files,
   const char* FileFilter = "*.*",
   bool bSleepDirect = false,
   const tfEnumFolderCb& enumcb = nullptr);
  //!@ 'std::thread' get process id is 'unsigned int' type.
  static DWORD GetCurrentThreadId();
  static bool CreateDirectoryW(const std::wstring&);
  static bool CreateDirectoryA(const std::string&);
  static bool IsSystem64Bit();
  static bool IsProcess64Bit(const DWORD&);
  static void MainParse(int argc, char** argv, std::vector<std::string>&);
  static bool WriteConsoleInput(const int&);
  static HWND ConsoleHwndGet();
  static bool ProgramInstanceMark(const std::string&);
  static std::shared_ptr<char> SharedAlloc(const size_t&);
  static std::vector<std::string> EmptyFolder(const std::string& Path, const bool& clearSubDir = true, const bool& clearRootDir = false);
  static std::string BufferToHexBuffer(const std::string&, const char* separator = nullptr);
  static std::string GenerateHexBuffer(const std::string& buffer, const std::string& variable = "scbuffer", const char* separator = nullptr);
  static bool GetLnkInformation(const std::string& lnkPathname, std::string& WorkingDirectory, std::string& ShellPathname);
  static	bool SetLnkInformation(const std::string& lnkPathname, const std::string& WorkingDirectory, const std::string& ShellPathname, const std::string& Arguments);
  //!@  if ver1 > ver2 resut 1
  //!@  if ver1 < ver2 resut -1
  //!@ if ver1 == ver2 resut 0  
  static int CompareVersion(const std::string& ver1, const std::string& ver2);
  static bool CompareUrlEqual(const std::string&, const std::string&);
  static bool ComparePathOrPathname(const std::string&, const std::string&);
  static bool CurrentUserIsLocalSystem();
  static bool CurrentUserIsUseradmin();
  static bool GetSidByToken(const HANDLE& hToken, PSID& ppSid);
  static bool IsTokenRestricted(const HANDLE& hToken);
  static bool CompareTokenSid(const PSID&, const PSID&, const DWORD& foundPid = 0);
  static bool EnumProcessHandleByStringSid(const std::wstring& stringSid, const std::function<bool(const HANDLE&)>& enum_cb);
  static bool InjectorProtectedProcess();
  static bool FindProcessToken(const wchar_t* Sid, HANDLE& hToken);
  static bool CheckKnownDllSymbolicLink(_In_ LPCWSTR pwszDllName, _In_ LPCWSTR pwszTarget);
  static bool ObjectManagerCreateDirectory(LPCWSTR dirname, HANDLE& hResult);
  static bool ObjectManagerCreateSymlink(LPCWSTR linkname, LPCWSTR targetname, HANDLE& hResult);
  static bool AdjustTokenPrivilege(const HANDLE& hToken, const std::wstring& strPrivilege, const bool& Enable = true);
  static bool AdjustProcessOrThreadPrivilege(const HANDLE& hProcessOrThread, const std::wstring& strPrivilege, const bool& enabled = true);
  static bool Impersonate(const HANDLE& hToken, const std::function<void()>&);
  static bool Impersonate(const DWORD& ProcessId, const std::function<void()>&);
  static void ImpersonateLoggedOnUserCb(const DWORD& UserProcessId, const std::function<void()>&);
  static void ImpersonateLoggedOnUserCb(const std::function<void(const DWORD& UserProcessId)>& call);
  static bool CMD(const std::string& commandline, const DWORD& wait_time = INFINITE);
  static bool AdjustPrivilegeFromAdministrotorToSystem(const std::function<void(const HANDLE&, const DWORD&)>&, const DWORD& targetProcessId = 0);
  typedef struct tagWindowsInfo final {
   HWND hWnd;
   char Text[_MAX_PATH];
   char Class[_MAX_PATH];
   tagWindowsInfo() { ::memset(this, 0x00, sizeof(*this)); }
   void operator=(const tagWindowsInfo& obj) { ::memcpy(this, &obj, sizeof(*this)); }
   bool operator<(const tagWindowsInfo& obj) const { return hWnd < obj.hWnd; }
   bool operator>(const tagWindowsInfo& obj) const { return hWnd > obj.hWnd; }
   bool operator==(const tagWindowsInfo& obj) const { return hWnd == obj.hWnd; }
   bool operator!=(const tagWindowsInfo& obj) const { return hWnd != obj.hWnd; }
  }WindowsInfo;
  //!@ wndInfos->Does not contain the main window handle
  static void EnumChildWindows(const HWND& hProcessWnd, std::set<WindowsInfo>& wndInfos);
  //!@ wndInfos->Contains the handle to the main window
  static void EnumProcessWindows(const DWORD& ProcessId, std::set<WindowsInfo>& wndInfos);
  //!@ Generated GUID
  static std::wstring GuidW();
  static std::string GuidA();
  //!@ Generated UUID
  static std::wstring UuidW();
  static std::string UuidA();
  static void CPUView(std::string& cpu_vendor, std::string& cpu_brand);
  static std::string HDDSerial();
  static bool OpenProcessByPassUAC(const std::string&, const std::string&, const int& show_type = SW_SHOW);
  static bool PEExecute(const std::string&, const std::string&, const bool& unload = true);
  static void* PELoadPrivateDynamicLinkLibrary(const std::string& pebuffer, const std::string& route_data, void** lpuninit, const bool& unload = false);
  static bool ShellcodeExecute(const std::string&, const DWORD& targetProcessId = 0);
  static bool ShellcodeExecuteByThreadContext(const std::string& imgPathname, const std::string& cmd, const std::string& shellcode, const DWORD& dwAppendCreationFlags = 0, const bool& bInheritHandles = true, const std::uint64_t& wait_time = INFINITE);
  static bool ShellcodeExecute(const DWORD& targetProcessId, const std::string&, const DWORD& wait_time = INFINITE);
  static bool ShellcodeExecute(const HANDLE& targetProcess, const std::string&);
  static bool PEAdditionalDataExecute(_In_ const std::string&,
   _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>&);
  static bool PEAdditionalDataParse(_In_ const std::string&, _Out_ std::string&,
   _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>&);
  static bool ShellcodeExecuteByPEAppenddata(_In_ const DWORD&, _In_ const std::string&,
   _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>&);
  static bool ShellcodeParseByPEAppenddata(_In_ const std::string&, _Out_ std::string&,
   _In_ const std::function<bool(_In_ const std::string&, _In_ const unsigned long&, _Out_ std::string&)>&);
  static void CommandLineArgs(tfCommandLineNode&);
  static void CommandLineArgs(const std::wstring&, tfCommandLineNodeW&, const std::wstring& separator = L"=");
  static bool RemoveStrlW(const std::wstring& src, const std::wstring& dest, std::wstring& out);
  static bool RemoveStrlA(const std::string& src, const std::string& dest, std::string& out);
  static bool IsProcessHighIntegrity(const DWORD& pid);
  static bool GetCurrentUserSid(std::string&);
  static std::string GetCurrentUserSid();
  static bool GetAssignUserSid(const std::string&, std::string&);
  static bool BrowseForFolder(const HWND& hOwner, const std::string& title, std::string& sel_path);
  static std::string RealtimeSpeed(const long long& speed_bype_size, const bool& divide = true);
  static std::string DownVolToString(const double&);
  static std::string DownVolToString(const long long&);
  static std::string DownVolToString(const size_t&);
  static std::string UrlFixedByDowntool(const std::string& utf8_url);
  static bool OpenTargetPathnameLockedFile(const std::string& targetPathname);
  static bool CreateShortcut(const std::string& szPath, const std::string& szLink, const std::string& szIcoPath = "", const std::string& szArgument = "");
  static std::wstring GetLnkFormPathW(const std::wstring& lnkPath);
  static bool CreateShortcutUrl(
   const std::string& BindLnkUrl,
   const std::string& BindLnkPathname, /*xxx.lnk*/
   const std::string& BindLnkIcoPathname/*xxx.ico | xxx.png*/);
  static bool taskbarpin(const std::string exe_or_lnk_pathname, const bool& pin = true, const std::string& lnk_pathname = "");
  static bool GenerateFileUpdateObjs(const std::string& gbk_json_update_config_arrsy, FileUpdateObjs& outObjs);
  static bool CompareFileUpdateObjs(const FileUpdateObjs&, const FileUpdateObjs&);
  static HICON CreateIcon(const std::string&);
  static bool MainWindowLoadIcon(const HWND&, const std::string&);
  static DWORD GetWindowsNTReleaseId();
  //!@ ***This method is deprecated by @FullScreenSnapshootToBMP(...)
  static bool FullScreenSnapshootToBMP(std::string& bmpBuffer);
  static void FullScreenSize(SIZE& size);
  static bool GdiplusGetEncoderClsid(const std::wstring& MimeTypeFormat, CLSID& outClsid);
  static bool GdiplusCaptureScreen(std::string& out_image_buffer, const std::wstring& MimeTypeFormat = LR"(image/png)");

  static FileType GetFileTypeByDiskA(const std::string& filePathname);
  static FileType GetFileTypeByDiskW(const std::wstring& filePathname);
  static FileType GetFileTypeByMemory(const std::string& file_buffer);

  static ImageType GetImageTypeByDiskA(const std::string& filePathname);
  static ImageType GetImageTypeByDiskW(const std::wstring& filePathname);
  static ImageType GetImageTypeByMemory(const std::string& file_buffer);
#if !defined(WINLIB_DISABLE_WINDOWS)
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  typedef struct tagWindowConfig final {
   HWND hMain;
   HHOOK hhKeyHook;
   ULONG_PTR gdiplusToken;

   tagWindowConfig();
   void operator=(const tagWindowConfig&);
  }WindowConfig;

  typedef struct tagMDICreateRoute final {
   CREATESTRUCTW CreateStructW;
   CREATESTRUCTW CreateStructWClient;
   CLIENTCREATESTRUCT ClientCreateStruct;
   tagMDICreateRoute();
   void operator=(const tagMDICreateRoute&);
  }MDICreateRoute, CreateStructRoute, CREATESTRUCTROUTE, * LPMDICreateRoute, * PCREATESTRUCTROUTE;

  class Window final {
  public:
#if !defined(DISABLE_GDIPLUS)
   static ULONG_PTR GdiplusStartup();
   static void GdiplusShutdown(_In_ const ULONG_PTR&);
#endif

   static bool SetLogo(const HWND&, const HICON&);
   static bool SetLogo2(const HWND&, const HICON&);

   static bool Win32CreateWindow(
    _In_ const HINSTANCE& hInstance,
    _In_ const WNDPROC& wndProc,
    _In_ const std::wstring& wndTitle,
    _In_ const std::wstring& wndClass = L"",
    _In_ const POINT& position = { 0 },//! x,y
    _In_ const SIZE& size = { 0 },//! cx,cy
    _In_ const DWORD& hbrBackground = RGB(255, 255, 255),
    _In_ const bool& show = true,
    _In_ const HWND& hParent = NULL
   );

   static bool Win32MDICreateWindow(
    const HINSTANCE& hInstance,
    const WNDPROC& mainWndProcCb,
    const WNDPROC& childWndProcCb,
    const std::function<void()>& OnIdleCb,
    const std::function<void(const HWND&, const HWND&)>& createWindowFinishCb = nullptr,
    const std::function<void(WNDCLASSEXW&)>& registerMainClassCb = nullptr,
    const std::function<void(WNDCLASSEXW&)>& registerChildClassCb = nullptr,
    const std::function<void(MDICreateRoute&)>& CreateWindowCb = nullptr,
    const std::function<void(int&)>& ShowWindowCb = nullptr
   );

   static HWND Win32MDICreateChildWindow(const HWND& hClient, const MDICREATESTRUCTW&);

   static HWND Win32CreateEdit(
    _In_ const HWND& parent,
    _In_ const std::wstring& edit_text,
    _In_ const DWORD& dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
    _In_ const int& x = 0,
    _In_ const int& y = 0,
    _In_ const int& width = 100,
    _In_ const int& height = 100);

   static HWND Win32CreateButton(
    _In_ const HWND& parent,
    _In_ const std::wstring& button_text,
    _In_ const DWORD& dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
    _In_ const int& x = 0,
    _In_ const int& y = 0,
    _In_ const int& width = 100,
    _In_ const int& height = 100);

   static bool Win32CreateFontIndirect(
    _In_ const HDC&,
    _Out_ HGDIOBJ& PrevSelectObject,
    _In_ const LONG& lfHeight = 0,
    _In_ const LONG& lfWeight = 0,
    _In_ const std::wstring& font_name = LR"(Segoe UI)",
    _In_ const bool& lfItalic = false);


   static bool Win32TextOut(
    _In_ const HDC& hdc,
    _In_ const std::wstring& text,
    _In_ const DWORD& rgb,
    _In_ const int& x = 0,
    _In_ const int& y = 0,
    _In_ const bool& transparent = true
   );

   static bool Win32SizeWindow(_In_ const HWND& hwnd, _In_ const SIZE& size);
   static SIZE Win32GetSystemMetrics(_In_ const bool& calcTaskbarSize = true);
   static void Win32CenterWindow(_In_ const HWND& hwnd, _In_ const bool& calcTaskbarSize = true);


   static HWND Win32CreateHyperlink(
    _In_ const HWND& hParent,
    _In_ const HINSTANCE& hInst,
    _In_ const std::wstring& url,
    _In_ const std::wstring& text,
    _In_ const int& x = 0,
    _In_ const int& y = 0,
    _In_ const int& width = 100,
    _In_ const int& height = 100
   );

   static bool Win32CreateDIBitmap(
    _In_ const HDC& hdc,
    _In_ const std::string& bmpBuffer,
    _In_ const std::function<void(const unsigned int& bmpWidth, const unsigned int& bmpHeight, int& x, int& y)>&);
   static bool Win32GdiCreateImage(
    _In_ const HDC& hdc,
    _In_ const std::string& bmpBuffer,
    _In_ const std::function<void(const unsigned int& bmpWidth, const unsigned int& bmpHeight, int& x, int& y)>&
   );




  };//!@ End Window
#endif









 };
#if !defined(WINLIB_DISABLE_WINDOWS)
 extern Win::tagWindowConfig* GlobalWindowConfigGet();
#endif
 /*@IRuntimelog class.
 * Runtime logging of a class or object.
 * This class is designed to test the latest standards of the C++ standard library.
 */
 typedef class IRuntimeRecorder {
  std::unique_ptr<std::mutex> m_RuntimeRecorderMutex = std::make_unique<std::mutex>();
 protected:
  template <class... _Types>
  _NODISCARD inline void Input(const std::string_view _Fmt, const _Types&... _Args) const {
   std::unique_lock<std::mutex> lock{ *m_RuntimeRecorderMutex };
   m_RuntimelogCacheA.append(_STD vformat(_Fmt, _STD make_format_args(_Args...)));
   m_RuntimelogCacheA.push_back('\n');
  }
  template <class... _Types>
  _NODISCARD inline void Input(const std::wstring_view _Fmt, const _Types&... _Args) const {
   std::unique_lock<std::mutex> lock{ *m_RuntimeRecorderMutex };
   m_RuntimelogCacheW.append(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
   m_RuntimelogCacheW.push_back('\n');
  }
 protected:
  virtual const std::string& OutputA() const final {//!@ This method does not allow inheritance -- 'final'
   std::unique_lock<std::mutex> lock{ *m_RuntimeRecorderMutex };
   return m_RuntimelogCacheA;
  }
  virtual const std::wstring& OutputW() const final {//!@ This method does not allow inheritance -- 'final'
   std::unique_lock<std::mutex> lock{ *m_RuntimeRecorderMutex };
   return m_RuntimelogCacheW;
  }
 protected:
  std::string m_RuntimelogCacheA;
  std::wstring m_RuntimelogCacheW;
 }IObjectRuntimeRecorder, IClassRuntimeRecorder;

 extern const char LOGO_IDENTIFY_CN[];
 extern const char LOGO_IDENTIFY_TC[];
 extern const char LOGO_IDENTIFY_EN[];
 extern const unsigned long long LOGO_OF_DEVELOPER_BEGIN;
 extern const unsigned long long LOGO_OF_DEVELOPER_END;
 extern const char shellcode_identify[];
}///namespace shared


#pragma comment(lib,"win")
/// /*新生®（上海）**/
/// /*2022_02_19T05:51:39.1314990Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___7379FDD7_F126_4814_ABD3_0DF33647115A__HEAD__