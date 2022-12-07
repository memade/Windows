#include "stdafx.h"

namespace shared {
 const unsigned long long LENMAPFILEHEAD = sizeof(MAPFILEHEAD);
 tagMAPFILEHEAD::tagMAPFILEHEAD() { Default(); }
 bool tagMAPFILEHEAD::Verify() const {
  return head == LOGO_OF_DEVELOPER_BEGIN && tail == LOGO_OF_DEVELOPER_END;
 }
 void tagMAPFILEHEAD::Default() { Reset(); }
 void tagMAPFILEHEAD::operator=(const tagMAPFILEHEAD& obj) {
  ::memcpy(this, &obj, sizeof(*this));
 }
 bool tagMAPFILEHEAD::operator<(const tagMAPFILEHEAD& obj) const {
  return ::memcmp(this, &obj, sizeof(*this)) < 0;
 }
 bool tagMAPFILEHEAD::operator>(const tagMAPFILEHEAD& obj) const {
  return ::memcmp(this, &obj, sizeof(*this)) > 0;
 }
 bool tagMAPFILEHEAD::operator==(const tagMAPFILEHEAD& obj) const {
  return ::memcmp(this, &obj, sizeof(*this)) == 0;
 }
 bool tagMAPFILEHEAD::operator!=(const tagMAPFILEHEAD& obj) const {
  return ::memcmp(this, &obj, sizeof(*this)) != 0;
 }
 void tagMAPFILEHEAD::Reset() {
  ::memset(this, 0x00, sizeof(*this));
  head = LOGO_OF_DEVELOPER_BEGIN;
  tail = LOGO_OF_DEVELOPER_END;
  ::memcpy(identify, LOGO_IDENTIFY_CN, __min(sizeof(identify), ::strlen(LOGO_IDENTIFY_CN)));
  size_head = sizeof(*this);
  size_total = size_head;
  size_data = 0;
  time_create = ::time(0);
  time_update = time_create;
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 Win::File::Memory::Memory(const std::string& pathname, const std::string& shared /*= ""*/) :
  m_FilePathname(pathname),
  m_Shared(shared) {
  Init();
 }
 Win::File::Memory::Memory(const std::string& pathname, const std::uint64_t& initdatasize, const std::string& shared /*= ""*/) :
  m_FilePathname(pathname),
  m_Shared(shared),
  m_InitDataSize(initdatasize) {
  Init();
 }
 Win::File::Memory::~Memory() {
  UnInit();
 }
 bool Win::File::Memory::Init() {
  if (m_FilePathname.empty())
   return false;
  if (m_Ready.load())
   return true;
  ::GetSystemInfo(&m_SystemInfo);
  do {
   m_hFileObj = ::CreateFileA(m_FilePathname.c_str(),
    GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_ALWAYS,
    FILE_FLAG_SEQUENTIAL_SCAN,
    NULL);
   if (INVALID_HANDLE_VALUE == m_hFileObj || NULL == m_hFileObj)
    break;
   if (::SetFilePointer(m_hFileObj, 0, NULL, FILE_BEGIN) == HFILE_ERROR)
    break;
   DWORD dwReadFileSize = 0;
   MAPFILEHEAD head;
   if (!::ReadFile(m_hFileObj, &head, sizeof(MAPFILEHEAD), &dwReadFileSize, NULL))
    break;
   if (!head.Verify())
    head.Default();
   if (m_InitDataSize > 0) {
    head.size_data = static_cast<decltype(head.size_data)>(m_InitDataSize);
    head.size_total += head.size_data;
   }
   if (!head.shared[0]) {
    if (m_Shared.empty())
     m_Shared = Win::Encryption::MD5(std::string((char*)&head.time_create, sizeof(head.time_create)));// md5.Encode(std::string((char*)&head.time_create, sizeof(head.time_create)));
    ::memcpy(head.shared, m_Shared.c_str(), __min(sizeof(head.shared), m_Shared.size()));
   }
   m_Identify = Win::Encryption::MD5(m_FilePathname);
   if (head.size_total <= m_SystemInfo.dwAllocationGranularity)
    m_MappingFileSize = m_SystemInfo.dwAllocationGranularity;
   else
    m_MappingFileSize = (static_cast<size_t>(head.size_total / m_SystemInfo.dwAllocationGranularity) + 1) * m_SystemInfo.dwAllocationGranularity;
   m_hFileMappingObj = ::CreateFileMappingA(m_hFileObj, NULL,
    PAGE_READWRITE, 0, static_cast<DWORD>(m_MappingFileSize), (LPCSTR)head.shared);
   if (INVALID_HANDLE_VALUE == m_hFileMappingObj || NULL == m_hFileMappingObj)
    break;
   m_pMappingBaseAddress = ::MapViewOfFile(
    m_hFileMappingObj,
    FILE_MAP_ALL_ACCESS,
    static_cast<DWORD>((static_cast<long long>(head.size_total) >> 32)),
    /*(DWORD)(m_MappingFileSize & 0xFFFFFFFF)*/0,
    static_cast<DWORD>(m_MappingFileSize));
   if (NULL == m_pMappingBaseAddress || INVALID_HANDLE_VALUE == m_pMappingBaseAddress)
    break;
   ::memcpy(m_pMappingBaseAddress, &head, LENMAPFILEHEAD);
   ::FlushViewOfFile(m_pMappingBaseAddress, static_cast<DWORD>(head.size_total));
   m_FileObjSize = ::GetFileSize(m_hFileObj, NULL);

   m_Ready.store(true);
  } while (0);
  if (!m_Ready.load()) {
   SK_CLOSE_HANDLE(m_pMappingBaseAddress);
   SK_CLOSE_HANDLE(m_hFileMappingObj);
   SK_CLOSE_HANDLE(m_hFileObj);
  }
  return m_Ready.load();
 }
 void Win::File::Memory::UnInit() {
  if (!m_Ready.load())
   return;
  HANDLE hTempObjectFile = nullptr;
  HANDLE hTempObjectFileMapping = nullptr;
  do {
   if (!m_pMappingBaseAddress)
    break;
   MAPFILEHEAD head;
   ::memcpy(&head, m_pMappingBaseAddress, LENMAPFILEHEAD);
   ::FlushViewOfFile(m_pMappingBaseAddress, static_cast<DWORD>(head.size_total));
   if (!::UnmapViewOfFile(m_pMappingBaseAddress))
    break;
   SK_CLOSE_HANDLE(m_hFileMappingObj);
   SK_CLOSE_HANDLE(m_hFileObj);
   hTempObjectFile = ::CreateFileA(
    m_FilePathname.c_str(), GENERIC_WRITE | GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
    FILE_FLAG_SEQUENTIAL_SCAN, NULL);
   if (INVALID_HANDLE_VALUE == hTempObjectFile || NULL == hTempObjectFile)
    break;
   hTempObjectFileMapping = ::CreateFileMappingA(
    hTempObjectFile,
    NULL,
    PAGE_READWRITE,
    0,
    static_cast<DWORD>((static_cast<long long>(head.size_total) & 0xFFFFFFFF)),
    NULL);
   if (INVALID_HANDLE_VALUE == hTempObjectFileMapping || NULL == hTempObjectFileMapping) {
    SK_CLOSE_HANDLE(hTempObjectFile);
    break;
   }
   SK_CLOSE_HANDLE(hTempObjectFile);
   LPVOID pTempMappingBaseAddress = ::MapViewOfFile(
    hTempObjectFileMapping,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    (SIZE_T)head.size_total);

   if (NULL == pTempMappingBaseAddress) {
    SK_CLOSE_HANDLE(hTempObjectFileMapping);
    break;
   }
   if (!::UnmapViewOfFile(pTempMappingBaseAddress)) {
    SK_CLOSE_HANDLE(hTempObjectFileMapping);
    break;
   }
   SK_CLOSE_HANDLE(hTempObjectFileMapping);
  } while (0);
  SK_CLOSE_HANDLE(hTempObjectFileMapping);
  SK_CLOSE_HANDLE(hTempObjectFile);
  m_Ready.store(false);
 }
 bool Win::File::Memory::AllocationGranularity(const size_t& current_data_total_size) {
  bool result = false;
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   const MAPFILEHEAD* pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   size_t AllocationGranularitySize = 0;
   if (current_data_total_size <= m_SystemInfo.dwAllocationGranularity)
    AllocationGranularitySize = m_SystemInfo.dwAllocationGranularity;
   else
    AllocationGranularitySize = (static_cast<size_t>(current_data_total_size / m_SystemInfo.dwAllocationGranularity) + 1) * m_SystemInfo.dwAllocationGranularity;
   ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total);
   SK_CLOSE_HANDLE(m_hFileMappingObj);
   ::UnmapViewOfFile(m_pMappingBaseAddress);
   m_pMappingBaseAddress = nullptr;
   SK_CLOSE_HANDLE(m_hFileObj);
   m_hFileObj = ::CreateFileA(
    m_FilePathname.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_ALWAYS,
    FILE_FLAG_SEQUENTIAL_SCAN,
    NULL);
   if (INVALID_HANDLE_VALUE == m_hFileObj || NULL == m_hFileObj)
    break;
   m_MappingFileSize += static_cast<decltype(m_MappingFileSize)>(AllocationGranularitySize);
   m_hFileMappingObj = ::CreateFileMappingA(
    m_hFileObj,
    NULL,
    PAGE_READWRITE,
    0,
    m_MappingFileSize,
    (LPCSTR)m_Shared.c_str());
   if (INVALID_HANDLE_VALUE == m_hFileMappingObj || NULL == m_hFileMappingObj) {
    SK_CLOSE_HANDLE(m_hFileObj);
    break;
   }
   m_pMappingBaseAddress = ::MapViewOfFile(
    m_hFileMappingObj,
    FILE_MAP_ALL_ACCESS,
    static_cast<DWORD>((static_cast<long long>(current_data_total_size) >> 32)),
    /*(DWORD)(m_MappingFileSize & 0xFFFFFFFF)*/0,
    static_cast<DWORD>(m_MappingFileSize));
   if (!m_pMappingBaseAddress) {
    DWORD error_code = ::GetLastError();
    switch (error_code) {
    case ERROR_NOT_ENOUGH_MEMORY: {
     /*Not enough memory resources are available to process this command.*/
     auto error = 0;
    }break;
    default:
     break;
    }
    SK_CLOSE_HANDLE(m_hFileObj);
    SK_CLOSE_HANDLE(m_hFileMappingObj);
    break;
   }
   m_FileObjSize = ::GetFileSize(m_hFileObj, NULL);
   result = true;
  } while (0);
  return result;
 }
 bool Win::File::Memory::Ready() const {
  return m_Ready.load();
 }
 const std::string& Win::File::Memory::FilePathname() const {
  return m_FilePathname;
 }
 bool Win::File::Memory::Write(const std::string& data, const bool& flush /*= true*/) {
  if (data.empty())
   return false;
  return Write(data.data(), data.size());
 }
 bool Win::File::Memory::Write(const std::string& data, const size_t& offset, const bool& flush /*= true*/) {
  if (data.empty())
   return false;
  return Write(data.data(), data.size(), offset);
 }
 bool Win::File::Memory::Write(const char* data, const size_t& size, const bool& flush /*= true*/) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_Ready.load())
    break;
   if (!data || size <= 0)
    break;
   if (!m_pMappingBaseAddress)
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (m_MappingFileSize < (size + pHead->size_total))
    if (!AllocationGranularity(size + pHead->size_total))
     break;
   pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   ::memcpy((char*)m_pMappingBaseAddress + pHead->size_total, data, size);
   pHead->size_total += static_cast<decltype(pHead->size_total)>(size);
   pHead->size_data += static_cast<decltype(pHead->size_data)>(size);
   pHead->time_update = ::time(NULL);

   result = true;
   if (flush)
    result = TRUE == ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total) != 0;
  } while (0);
  return result;
 }
 bool Win::File::Memory::Write(const char* data, const size_t& size, const size_t& offset, const bool& flush /*= true*/) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_Ready.load())
    break;
   if (!data || size <= 0)
    break;
   if (!m_pMappingBaseAddress)
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (m_MappingFileSize < (size + offset + LENMAPFILEHEAD))
    if (!AllocationGranularity(size + offset + LENMAPFILEHEAD))
     break;
   pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   ::memcpy((char*)m_pMappingBaseAddress + LENMAPFILEHEAD + offset, data, size);
   pHead->size_total = __max(pHead->size_total, static_cast<decltype(pHead->size_total)>(LENMAPFILEHEAD + offset + size));
   pHead->size_data = __max(pHead->size_data, static_cast<decltype(pHead->size_data)>(offset + size));
   pHead->time_update = ::time(NULL);

   result = true;
   if (flush)
    result = TRUE == ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total) != 0;
  } while (0);
  return result;
 }
 const char* Win::File::Memory::Base() const {
  const char* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   result = reinterpret_cast<const char*>(m_pMappingBaseAddress);
  } while (0);
  return result;
 }
 bool Win::File::Memory::MoveHeadToEOF(const bool& flush /*= true*/) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (!pHead->Verify())
    break;
   if (pHead->size_data <= 0)
    break;
   MAPFILEHEAD head;
   ::memcpy(&head, m_pMappingBaseAddress, LENMAPFILEHEAD);
   ::memmove((char*)m_pMappingBaseAddress, (char*)m_pMappingBaseAddress + LENMAPFILEHEAD, head.size_data);
   ::memcpy((char*)m_pMappingBaseAddress + head.size_data, &head, LENMAPFILEHEAD);
   m_RememberDataSize = head.size_data;

   result = true;
   if (flush)
    result = TRUE == ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total + LENMAPFILEHEAD) != 0;
  } while (0);
  return result;
 }
 MAPFILEHEAD* Win::File::Memory::HeadFromEOF() const {
  MAPFILEHEAD* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (m_RememberDataSize <= 0 || m_FileObjSize <= 0)
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>((char*)m_pMappingBaseAddress + m_RememberDataSize);
   if (!pHead->Verify())
    break;
   result = pHead;
  } while (0);
  return result;
 }
 std::string Win::File::Memory::PathFixed(const std::string& path) const {
  std::string result{ path };
  if (path.empty())
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
 std::string Win::File::Memory::FinishGenerate(const std::string& generate_filename /*= ""*/, const bool& flush /*= true*/) {
  std::string result;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (!m_pMappingBaseAddress || !m_Ready.load())
   return "";
  auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
  if (!pHead->Verify())
   return "";
  if (pHead->size_data <= 0)
   return "";
  MAPFILEHEAD head;
  ::memcpy(&head, m_pMappingBaseAddress, LENMAPFILEHEAD);
  m_RememberDataSize = head.size_data;
  ::memmove((char*)m_pMappingBaseAddress, (char*)m_pMappingBaseAddress + LENMAPFILEHEAD, head.size_data);
  if (flush)
   ::FlushViewOfFile(m_pMappingBaseAddress, m_RememberDataSize);
  UnInit();
  bool success = false;
  HANDLE hFile = NULL;
  do {
   hFile = ::CreateFileA(m_FilePathname.c_str(),
    GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
   if (hFile == INVALID_HANDLE_VALUE || hFile == NULL)
    break;
   LARGE_INTEGER liDistanceToMove = { 0 };
   if (!::GetFileSizeEx(hFile, &liDistanceToMove))
    break;
   if (liDistanceToMove.QuadPart < m_RememberDataSize)
    break;
   ::memset(&liDistanceToMove, 0x00, sizeof(liDistanceToMove));
   liDistanceToMove.QuadPart = m_RememberDataSize;
   if (!::SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_BEGIN))
    break;
   if (!::SetEndOfFile(hFile))
    break;
   success = true;
  } while (0);
  SK_CLOSE_HANDLE(hFile);
  if (success) {
   if (generate_filename.empty())
    result = m_FilePathname + (generate_filename.empty() ? "mem.finish" : generate_filename);
   else {
    result = m_FilePathname;
    for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
     if (*rit == '\\' || *rit == '/') {
      *rit = 0;
      result.resize(std::distance(result.begin(), rit.base()) - 1);
      break;
     }
    }
    result = PathFixed(result + "\\" + generate_filename);
   }
   ::remove(result.c_str());
   success = ::rename(PathFixed(m_FilePathname).c_str(), result.c_str()) == 0;
  }
  return success ? result : "";
 }
#if 0
 bool Generate(const std::string& filename) {
  if (filename.empty())
   return false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (!m_pMappingBaseAddress || !m_Ready.load() || !m_hFileObj)
   return false;
  MAPFILEHEAD head = *reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
  if (!head.Verify())
   return false;
  if (head.size_data <= 0)
   return false;
  ::memset(m_pMappingBaseAddress, 0x00, LENMAPFILEHEAD);
  ::memmove(m_pMappingBaseAddress, (char*)m_pMappingBaseAddress + LENMAPFILEHEAD, head.size_data);
  LARGE_INTEGER liDistanceToMove = { 0 };
  liDistanceToMove.QuadPart = head.size_data;
  if (!::SetFilePointerEx(m_hFileObj, liDistanceToMove, NULL, FILE_BEGIN))
   return false;
  if (!::SetEndOfFile(m_hFileObj))
   return false;
  return true;
 }
#endif
 const char* Win::File::Memory::Read(const size_t& offset /*= 0*/) const {
  const char* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (pHead->size_total <= (offset + LENMAPFILEHEAD))
    break;
   result = reinterpret_cast<char*>(m_pMappingBaseAddress) + LENMAPFILEHEAD + offset;
  } while (0);
  return result;
 }
 const MAPFILEHEAD* Win::File::Memory::Head() const {
  const MAPFILEHEAD* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   result = reinterpret_cast<const MAPFILEHEAD*>(m_pMappingBaseAddress);
  } while (0);
  return result;
 }
 bool Win::File::Memory::Head(const std::function<void(MAPFILEHEAD*)>& set_head_cb) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!set_head_cb)
    break;
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   set_head_cb(reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress));
   result = TRUE == ::FlushViewOfFile(m_pMappingBaseAddress, LENMAPFILEHEAD);
  } while (0);
  return result;
 }
 size_t Win::File::Memory::DataSize() const {
  size_t result = 0;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (!pHead || !pHead->Verify())
    break;
   result = pHead->size_data;
  } while (0);
  return result;
 }
 size_t Win::File::Memory::TotalSize() const {
  size_t result = 0;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (!pHead || !pHead->Verify())
    break;
   result = pHead->size_total;
  } while (0);
  return result;
 }
 time_t Win::File::Memory::TimeUpdate() const {
  time_t result = 0;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (!pHead || !pHead->Verify())
    break;
   result = pHead->time_update;
  } while (0);
  return result;
 }
 time_t Win::File::Memory::TimeCreate() const {
  time_t result = 0;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (!pHead || !pHead->Verify())
    break;
   result = pHead->time_create;
  } while (0);
  return result;
 }
 std::string Win::File::Memory::Shared() const {
  std::string result;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pMappingBaseAddress || !m_Ready.load())
    break;
   auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
   if (!pHead || !pHead->Verify())
    break;
   if (!pHead->shared[0])
    break;
   result = std::string(pHead->shared, __min(sizeof(pHead->shared), strlen(pHead->shared)));
  } while (0);
  return result;
 }
 const std::string& Win::File::Memory::Identify() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_Identify;
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void Win::File::FileVersionInfoW::operator=(const Win::File::FileVersionInfoW& obj) {
  FileDescription = obj.FileDescription;
  FileVersion = obj.FileVersion;
  InternalName = obj.InternalName;
  CompanyName = obj.CompanyName;
  LegalCopyright = obj.LegalCopyright;
  OriginalFilename = obj.OriginalFilename;
  ProductName = obj.ProductName;
  ProductVersion = obj.ProductVersion;
 }
 bool Win::File::FileVersionInfoW::operator==(const Win::File::FileVersionInfoW& obj) const {
  bool result = false;
  do {
   if (FileDescription.compare(obj.FileDescription))
    break;
   if (FileVersion.compare(obj.FileVersion))
    break;
   if (InternalName.compare(obj.InternalName))
    break;
   if (CompanyName.compare(obj.CompanyName))
    break;
   if (LegalCopyright.compare(obj.LegalCopyright))
    break;
   if (OriginalFilename.compare(obj.OriginalFilename))
    break;
   if (ProductName.compare(obj.ProductName))
    break;
   if (ProductVersion.compare(obj.ProductVersion))
    break;
  } while (0);
  return result;
 }
 bool Win::File::FileVersionInfoW::operator!=(const Win::File::FileVersionInfoW& obj) const {
  return !(*this == obj);
 }
 void Win::File::FileVersionInfoA::operator=(const Win::File::FileVersionInfoA& obj) {
  FileDescription = obj.FileDescription;
  FileVersion = obj.FileVersion;
  InternalName = obj.InternalName;
  CompanyName = obj.CompanyName;
  LegalCopyright = obj.LegalCopyright;
  OriginalFilename = obj.OriginalFilename;
  ProductName = obj.ProductName;
  ProductVersion = obj.ProductVersion;
 }
 bool Win::File::FileVersionInfoA::operator==(const Win::File::FileVersionInfoA& obj) const {
  bool result = false;
  do {
   if (FileDescription.compare(obj.FileDescription))
    break;
   if (FileVersion.compare(obj.FileVersion))
    break;
   if (InternalName.compare(obj.InternalName))
    break;
   if (CompanyName.compare(obj.CompanyName))
    break;
   if (LegalCopyright.compare(obj.LegalCopyright))
    break;
   if (OriginalFilename.compare(obj.OriginalFilename))
    break;
   if (ProductName.compare(obj.ProductName))
    break;
   if (ProductVersion.compare(obj.ProductVersion))
    break;
  } while (0);
  return result;
 }
 bool Win::File::FileVersionInfoA::operator!=(const Win::File::FileVersionInfoA& obj) const {
  return !(*this == obj);
 }

 void Win::File::ParseA(const std::string& inStrContend, const int& nDelimiter, std::vector<std::string>& Output) {
  Output.clear();
  do {
   if (inStrContend.empty())
    break;
   std::string tempContent{ inStrContend };
   size_t found = 0;
   while ((found = tempContent.find(nDelimiter)) != std::string::npos) {
    std::string strSession;
    strSession.insert(strSession.length(), tempContent.data(), found);
    tempContent.erase(0, found + 1);
    Output.push_back(strSession);
   }
   if (tempContent.empty())
    break;
   Output.push_back(tempContent);
  } while (0);
 }
 void Win::File::ParseW(const std::wstring& inStrContend, const int& nDelimiter, std::vector<std::wstring>& Output) {
  Output.clear();
  do {
   if (inStrContend.empty())
    break;
   std::wstring tempContent{ inStrContend };
   size_t found = 0;
   while ((found = tempContent.find(nDelimiter)) != std::wstring::npos) {
    std::wstring strSession;
    strSession.insert(strSession.length(), tempContent.data(), found);
    tempContent.erase(0, found + 1);
    Output.push_back(strSession);
   }
   if (tempContent.empty())
    break;
   Output.push_back(tempContent);
  } while (0);
 }
 std::string Win::File::ReadAssignSize(const std::string& FilePathname, const size_t& assign_size, const int& OpenMode /*= std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary*/) {
  std::string result;
  std::fstream of(FilePathname, OpenMode);
  /// Buffering was added to avoid crashes on the X86 platform
  char* buffer = nullptr;
  do {
   if (!of.is_open())
    break;
   of.seekg(0, of.end);
   const size_t size = static_cast<size_t>(of.tellg());
   if (assign_size > size)
    break;
   buffer = reinterpret_cast<char*>(::malloc(assign_size));
   if (!buffer)
    break;
   of.seekg(0, of.beg);
   of.read(buffer, assign_size);
   result.append(buffer, assign_size);
  } while (0);
  if (of.is_open())
   of.close();
  SK_DELETE_PTR_C(buffer);
  return result;
 }
 std::streampos Win::File::ReadFileSizeA(const std::string& FilePathname, const int& OpenMode /*= std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary*/) {
  std::streampos result = 0;
  std::fstream of(FilePathname, OpenMode);
  do {
   if (!of.is_open())
    break;
   of.seekg(0, of.end);
   result = static_cast<size_t>(of.tellg());
   of.seekg(0, of.beg);
  } while (0);
  if (of.is_open())
   of.close();
  return result;
 }
 std::streampos Win::File::ReadFileSizeW(const std::wstring& FilePathname, const int& OpenMode /*= std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary*/) {
  return Win::File::ReadFileSizeA(IConv::WStringToMBytes(FilePathname), OpenMode);
 }
 std::string Win::File::Read(const std::string& FilePathname, const int& OpenMode /*= std::ios::_Nocreate | std::ios::_Noreplace | std::ios::binary*/) {
  std::string result;
  std::fstream of(FilePathname, OpenMode);
  /// Buffering was added to avoid crashes on the X86 platform
  char* buffer = nullptr;
  do {
   if (!of.is_open())
    break;
   of.seekg(0, of.end);
   size_t size = static_cast<size_t>(of.tellg());
   if (size <= 0)
    break;
   buffer = reinterpret_cast<char*>(::malloc(size));
   if (!buffer)
    break;
   of.seekg(0, of.beg);
   of.read(buffer, size);
   result.append(buffer, size);
  } while (0);
  if (of.is_open())
   of.close();
  SK_DELETE_PTR_C(buffer);
  return result;
 }
 bool Win::File::Empty(const std::string& FilePathname) {
  bool result = false;
  do {
   if (!Win::AccessA(FilePathname))
    break;
   std::ofstream of(FilePathname, std::ios_base::trunc | std::ios_base::out);
   if (!of.is_open())
    break;
   of.close();
   result = true;
  } while (0);
  return result;
 }

#if 0
 bool Win::File::WriteUTF8BOM(const std::wstring& file_pathname, const std::wstring& input_unicode_data) {
  //!@ http://zplutor.github.io/2016/07/03/convert-character-encoding-using-std-wstring-convert/
  /*
  使用std::wstring_convert进行字符编码转换
C++11新增的std::wstring_convert可以很方便地在std::string和std::wstring之间进行转换。例如，把一个std::wstring转换成以UTF-8编码的std::string：

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
std::string string = converter.to_bytes(L"这是一个宽字符串");
反过来，把一个以UTF-8编码的std::string转换成std::wstring：

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
std::wstring wide_string = converter.from_bytes("\xe5\xad\x97\xe7\xac\xa6\xe4\xb8\xb2”);  //字符串的内容为“字符串”
std::wstring_convert使用模板参数中指定的codecvt进行实际的转换工作，也就是说，std::string使用哪种字符编码由这个codecvt来决定。上面的例子用的是std::codecvt_utf8，即UTF-8编码。理论上，指定不同的codecvt，即可支持各种字符编码。但是，如何得到合适codecvt则是不小的问题。

来看一下std::wstring_convert其中一个构造函数：

wstring_convert(codecvt* pcvt = new codecvt);
在构造std::wstring_convert对象的时候需要传入一个codecvt对象的指针，如果没有传入，则默认使用new codecvt来创建。std::wstring_convert自行维护codecvt对象的生命周期，它的析构函数会调用delete操作符来删除该对象。这就限制了只能使用通过new操作符来创建的codecvt，而不能使用从std::locale中获取的codecvt。

在C++标准提供的codecvt中，能够直接用于std::wstring_convert的只有三个：std::codecvt_utf8，std::codecvt_utf16以及std::codecvt_utf8_utf16。可见，标准只支持UTF族的字符编码。为了获取其它字符编码的codecvt，需要使用std::codecvt_byname，这个类可以通过字符编码的名称来创建一个codecvt。这看起来挺不错，但遗憾的是，字符编码的名称并没有统一的标准，各个平台的支持情况都不一样。例如，在Windows下可以使用“chs”来创建简体中文编码的codecvt，在Mac OS X下则要使用“zh_cn.gb2312”；甚至在Mac OS X下，即使成功创建了这个codecvt，它也不能正常地转换。

下面以Windows为例，说明如何将std::codecvt_byname用于std::wstring_convert。由于历史原因，std::codecvt_byname的析构函数是protected的，std::wstring_convert不能对它调用delete，所以首先要自行定义一个类来继承std::codecvt_byname：

class chs_codecvt : public std::codecvt_byname<wchar_t, char, std::mbstate_t> {
public:
    chs_codecvt() : codecvt_byname("chs") { }
};
chs_codecvt的默认析构函数是public的，从而让std::wstring_convert可以删除它。为方便起见，在chs_codecvt的构造函数中，直接把“chs”传给了std::codecvt_byname。

接下来的用法跟本文开头的例子基本一致：

std::wstring_convert<chs_codecvt> converter;
std::string string = converter.to_bytes(L"你好");
std::wstring wide_string = converter.from_bytes("\xc4\xe3\xba\xc3”);  //字符串的内容为“你好”
综上所述，只有UTF编码的转换是完全符合C++标准并且真正能够跨平台的，使用这些国际化的字符编码能够减少很多不必要的麻烦。
  */
  bool result = false;
  do {
   std::ofstream ofs(file_pathname);
   if (!ofs.is_open())
    break;
   std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
   std::string utf8_data = conv.to_bytes(input_unicode_data);
   if (utf8_data.empty())
    break;
   ofs << 0xEF << 0xBB << 0xBF;
   ofs << utf8_data;
   ofs.close();
   result = true;
  } while (0);
  return result;
 }
#endif
 bool Win::File::WriteAddto(const std::string& FilePathname, const std::string& WriteData) {
  bool result = false;
  do {
   if (WriteData.empty())
    break;
   if (!Win::AccessA(FilePathname))
    Win::CreateDirectoryA(Win::GetPathByPathnameA(FilePathname));
   std::ofstream of(FilePathname, std::ios::binary | std::ios::out | std::ios::app);
   if (!of.is_open())
    break;
   of << WriteData;
   of.close();
   result = true;
  } while (0);
  return result;
 }
 //!@ 追加写入 std::to_string(i), std::ios::binary | std::ios::out | std::ios::app
 bool Win::File::Write(const std::string& FilePathname, const std::string& WriteData, const int& OpenMode /*= std::ios::binary | std::ios::out | std::ios::trunc*/) {
  bool result = false;
  do {
   if (WriteData.empty())
    break;
   if (!Win::AccessA(Win::GetPathByPathnameA(FilePathname)))
    Win::CreateDirectoryA(Win::GetPathByPathnameA(FilePathname));
   std::ofstream of(FilePathname, OpenMode);
   if (!of.is_open())
    break;
   of << WriteData;
   of.close();
   result = true;
  } while (0);
  return result;
 }
 std::string Win::File::Load(const std::string& FilePathname, const std::string& OpenMode /*= "rb"*/, const int& _ShFlag /*= _SH_DENYNO*/) {
  std::string result;
  FILE* pFile = nullptr;
  do {
   FILE* pFile = ::_fsopen(FilePathname.c_str(), OpenMode.c_str(), _ShFlag);
   if (!pFile)
    break;
   if (0 != ::fseek(pFile, 0, SEEK_END))
    break;
   long lSize = ::ftell(pFile);
   if (lSize <= 0)
    break;
   ::rewind(pFile);
   //::fseek(pFile, 0, SEEK_SET);
   result.resize(lSize);
   size_t nread = ::fread(&result[0], 1, lSize, pFile);
   if (result.size() >= 2) {
    //! UTF16BE
    if (unsigned char(result.at(0)) == 0xFE && unsigned char(result.at(1)) == 0xFF) {
     for (int i = 0; i < 2; ++i)
      result.erase(result.begin());
    }
    //! UTF16LE
    else if (unsigned char(result.at(0)) == 0xFF && unsigned char(result.at(1)) == 0xFE) {
     for (int i = 0; i < 2; ++i)
      result.erase(result.begin());
    }
    if (result.size() >= 3) {
     //! UTF8
     if (unsigned char(result.at(0)) == 0xEF && unsigned char(result.at(1)) == 0xBB && unsigned char(result.at(2)) == 0xBF) {
      for (int i = 0; i < 3; ++i)
       result.erase(result.begin());
     }
    }
   }
  } while (0);
  if (pFile) {
   ::fclose(pFile);
   pFile = nullptr;
  }
  return result;
 }

 bool Win::File::CopyFilePass(const std::wstring& toPath, const std::wstring& fromPathname) {
  //!@ 该方法暂无法使用
  return false;
  bool result = false;
  HRESULT comInitResult = S_FALSE;
  HRESULT hResult = S_FALSE;
  IFileOperation* fileOperation = nullptr;
  IShellItem* from = nullptr;
  IShellItem* to = nullptr;
  do {
   if (toPath.empty() || fromPathname.empty())
    break;
   if (!AccessW(fromPathname))
    break;
   std::wstring fromFilename = GetNameByPathnameW(fromPathname);
   std::wstring fromPath = GetPathByPathnameW(fromPathname);
   if (fromFilename.empty() || fromPath.empty())
    break;
   comInitResult = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
   if (!SUCCEEDED(comInitResult))
    break;
   hResult = ::CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileOperation));
   if (!SUCCEEDED(hResult))
    break;
   hResult = fileOperation->SetOperationFlags(
    FOF_NOCONFIRMATION |
    FOF_SILENT |
    FOFX_SHOWELEVATIONPROMPT |
    FOFX_NOCOPYHOOKS |
    FOFX_REQUIREELEVATION |
    FOF_NOERRORUI);
   if (!SUCCEEDED(hResult))
    break;
   hResult = ::SHCreateItemFromParsingName(fromPath.c_str(), NULL, IID_PPV_ARGS(&from));
   if (!SUCCEEDED(hResult))
    break;
   hResult = SHCreateItemFromParsingName(toPath.c_str(), NULL, IID_PPV_ARGS(&to));
   if (!SUCCEEDED(hResult))
    break;
   hResult = fileOperation->CopyItem(from, to, fromFilename.c_str(), NULL);
   if (!SUCCEEDED(hResult))
    break;
   hResult = fileOperation->PerformOperations();
   if (!SUCCEEDED(hResult))
    break;
   result = true;
  } while (0);
  if (to) {
   to->Release();
   to = nullptr;
  }
  if (from) {
   from->Release();
   from = nullptr;
  }
  if (fileOperation) {
   fileOperation->Release();
   fileOperation = nullptr;
  }
  if (SUCCEEDED(comInitResult))
   ::CoUninitialize();
  return result;
 }
 std::string Win::File::Attribute::GetVersion(const std::string& filePathname) {
  std::string result;
  do {
   if (!AccessA(filePathname))
    break;
   VS_FIXEDFILEINFO* pVsInfo;
   unsigned int iFileInfoSize = static_cast<unsigned int>(sizeof(VS_FIXEDFILEINFO));
   DWORD dwVerInfoSize = ::GetFileVersionInfoSizeA(filePathname.c_str(), nullptr);
   if (dwVerInfoSize <= 0)
    break;
   std::string tempBuffer;
   tempBuffer.resize(dwVerInfoSize, 0x00);
   if (!::GetFileVersionInfoA(filePathname.c_str(), 0, dwVerInfoSize, &tempBuffer[0]))
    break;
   if (!::VerQueryValueA(&tempBuffer[0], "\\", (void**)&pVsInfo, &iFileInfoSize))
    break;
   result = std::format("{}.{}.{}.{}",
    HIWORD(pVsInfo->dwFileVersionMS),
    LOWORD(pVsInfo->dwFileVersionMS),
    HIWORD(pVsInfo->dwFileVersionLS),
    LOWORD(pVsInfo->dwFileVersionLS)
   );
  } while (0);
  return result;
 }
 bool Win::File::Attribute::VerifySigned(const std::wstring& wPath) {
  bool result = false;
  do {
   if (wPath.empty())
    break;
   /* Building various data structures used as part of the query */
   LONG lStatus = -1;
   WINTRUST_FILE_INFO FileData;
   ::memset(&FileData, 0, sizeof(FileData));
   FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
   FileData.pcwszFilePath = static_cast<LPCWSTR>(&wPath[0]);
   FileData.hFile = NULL;
   FileData.pgKnownSubject = NULL;
   GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
   WINTRUST_DATA WinTrustData;
   ::memset(&WinTrustData, 0, sizeof(WinTrustData));
   WinTrustData.cbStruct = sizeof(WinTrustData);
   WinTrustData.pPolicyCallbackData = NULL;
   WinTrustData.pSIPClientData = NULL;
   WinTrustData.dwUIChoice = WTD_UI_NONE;
   WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
   WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
   WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
   WinTrustData.hWVTStateData = NULL;
   WinTrustData.pwszURLReference = NULL;
   WinTrustData.dwUIContext = 0;
   WinTrustData.pFile = &FileData;
   /* API call which identifies whether a file has been correctly signed */
   lStatus = ::WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
   /* This function returns 0 if the file is correctly signed */
   result = lStatus == 0;
  } while (0);
  return result;
 }

#if 0 //!@ 实例方法
 BOOL CheckCertificateIssuer(HANDLE hWVTStateData, const std::set<std::wstring>& stValidIssuers)
 {
  CRYPT_PROVIDER_DATA* pCryptProvData = WTHelperProvDataFromStateData(hWVTStateData);
  CRYPT_PROVIDER_SGNR* pSigner = WTHelperGetProvSignerFromChain(pCryptProvData, 0, FALSE, 0);
  CRYPT_PROVIDER_CERT* pCert = WTHelperGetProvCertFromChain(pSigner, 0);


  DWORD dwStrType;
  DWORD dwCount = 0;
  LPTSTR szSubjectRDN = nullptr;

  dwStrType = CERT_X500_NAME_STR;
  dwCount = ::CertGetNameStringW(
   pCert->pCert,
   CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
   0,
   NULL,
   NULL,
   0
  );

  szSubjectRDN = new wchar_t[dwCount];
  ::CertGetNameStringW(
   pCert->pCert,
   CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
   0,
   NULL,
   szSubjectRDN,
   dwCount
  );

  delete[] szSubjectRDN;
  szSubjectRDN = nullptr;
  std::wstring sIssuer;
  int nLength = CertGetNameStringW(pCert->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, NULL, NULL, 0);
  if (!nLength)
  {
   assert(FALSE && "Cannot get the length of the Issuer string");
   return FALSE;
  }
  sIssuer.resize(nLength, 0x00);
  if (!CertGetNameStringW(pCert->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, NULL, &sIssuer[0], nLength))
  {
   assert(FALSE && "Cannot get the Issuer string");
   return FALSE;
  }
  do {
   if (sIssuer.empty())
    break;
   if (*std::prev(sIssuer.end()) == 0)
    sIssuer.pop_back();
   else
    break;
  } while (1);
  if (stValidIssuers.find(sIssuer) == stValidIssuers.end())
  {
   assert(FALSE && "Certificate issuer is invalid");
   return FALSE;
  }
  return TRUE;
 }
 BOOL CheckCertificate(std::wstring filename)
 {
  std::set<std::wstring> stValidIssuers;
  stValidIssuers.insert(L"VeriSign Class 3 Code Signing 2010 CA");
  stValidIssuers.insert(L"Symantec Class 3 SHA256 Code Signing CA");
  stValidIssuers.insert(L"Sectigo Public Code Signing CA R36");

  bool UseStrongSigPolicy = false;

  DWORD Error = ERROR_SUCCESS;
  bool WintrustCalled = false;
  GUID GenericActionId = WINTRUST_ACTION_GENERIC_VERIFY_V2;
  WINTRUST_DATA WintrustData = {};
  WINTRUST_FILE_INFO FileInfo = {};
  WINTRUST_SIGNATURE_SETTINGS SignatureSettings = {};
  CERT_STRONG_SIGN_PARA StrongSigPolicy = {};

  // Setup data structures for calling WinVerifyTrust
  WintrustData.cbStruct = sizeof(WINTRUST_DATA);
  WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
  WintrustData.dwUIChoice = WTD_UI_NONE;
  WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
  WintrustData.dwUnionChoice = WTD_CHOICE_FILE;

  FileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO_);
  FileInfo.pcwszFilePath = filename.c_str();
  WintrustData.pFile = &FileInfo;

  //
  // First verify the primary signature (index 0) to determine how many secondary signatures
  // are present. We use WSS_VERIFY_SPECIFIC and dwIndex to do this, also setting
  // WSS_GET_SECONDARY_SIG_COUNT to have the number of secondary signatures returned.
  //
  SignatureSettings.cbStruct = sizeof(WINTRUST_SIGNATURE_SETTINGS);
  SignatureSettings.dwFlags = WSS_GET_SECONDARY_SIG_COUNT | WSS_VERIFY_SPECIFIC;
  SignatureSettings.dwIndex = 0;
  WintrustData.pSignatureSettings = &SignatureSettings;

  if (UseStrongSigPolicy != false)
  {
   StrongSigPolicy.cbSize = sizeof(CERT_STRONG_SIGN_PARA);
   StrongSigPolicy.dwInfoChoice = CERT_STRONG_SIGN_OID_INFO_CHOICE;
   StrongSigPolicy.pszOID = (LPSTR)szOID_CERT_STRONG_SIGN_OS_CURRENT;
   WintrustData.pSignatureSettings->pCryptoPolicy = &StrongSigPolicy;
  }
  BOOL bResult = E_NOT_SET;
  std::cout << "Verifying primary signature... " << std::endl;
  Error = WinVerifyTrust(NULL, &GenericActionId, &WintrustData);
  WintrustCalled = true;
  if (Error == ERROR_SUCCESS)
  {
   if (CheckCertificateIssuer(WintrustData.hWVTStateData, stValidIssuers))
   {
    if (bResult == E_NOT_SET)
     bResult = TRUE;
   }
   else
   {
    bResult = FALSE;
   }

   std::cout << "Success" << std::endl;

   std::cout << std::format("Found {} secondary signatures\n", WintrustData.pSignatureSettings->cSecondarySigs);

   // Now attempt to verify all secondary signatures that were found
   for (DWORD x = 1; x <= WintrustData.pSignatureSettings->cSecondarySigs; x++)
   {
    std::cout << std::format("Verify secondary signature at index {}... ", x) << std::endl;
    // Need to clear the previous state data from the last call to WinVerifyTrust
    WintrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    Error = WinVerifyTrust(NULL, &GenericActionId, &WintrustData);
    if (Error != ERROR_SUCCESS)
    {
     //No need to call WinVerifyTrust again
     WintrustCalled = false;
     //TRACE(L"%s", utils::error::getText(Error));
     assert(FALSE);
     break;
    }

    WintrustData.hWVTStateData = NULL;

    // Caller must reset dwStateAction as it may have been changed during the last call
    WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WintrustData.pSignatureSettings->dwIndex = x;
    Error = WinVerifyTrust(NULL, &GenericActionId, &WintrustData);
    if (Error != ERROR_SUCCESS)
    {
     //TRACE(L"%s", utils::error::getText(Error));
     assert(FALSE);
     break;
    }

    if (CheckCertificateIssuer(WintrustData.hWVTStateData, stValidIssuers))
    {
     if (bResult == E_NOT_SET)
      bResult = TRUE;
    }
    else
    {
     bResult = FALSE;
    }

    std::cout << "Success!\n" << std::endl;
   }
  }
  else
  {
   //TRACE(utils::error::getText(Error));
   assert(FALSE);
  }

  //
  // Caller must call WinVerifyTrust with WTD_STATEACTION_CLOSE to free memory
  // allocate by WinVerifyTrust
  //
  if (WintrustCalled != false)
  {
   WintrustData.dwStateAction = WTD_STATEACTION_CLOSE;
   WinVerifyTrust(NULL, &GenericActionId, &WintrustData);
  }

  return bResult;
 }
#endif
 bool Win::File::Attribute::GetSignatureA(const std::string& FileBlobOrPathname, std::string& signature, const bool& blob /*= false*/) {
  bool result = false;
  signature.clear();
  do {
   std::wstring signature_w;
   if (!Win::File::Attribute::GetSignatureW(FileBlobOrPathname, signature_w, blob) || signature_w.empty())
    break;
   signature = IConv::WStringToMBytes(signature_w);
   result = !signature.empty();
  } while (0);
  return result;
 }
 bool Win::File::Attribute::GetSignatureW(const std::string& FileBlobOrPathname, std::wstring& signature, const bool& blob /*= false*/) {
  return GetSignatureW(IConv::MBytesToWString(FileBlobOrPathname), signature, blob);
 }
 bool Win::File::Attribute::GetSignatureW(const std::wstring& FileBlobOrPathname, std::wstring& signature, const bool& blob /*= false*/) {
  bool result = false;
  signature.clear();
  do {
   if (FileBlobOrPathname.empty())
    break;
   if (!blob)
    if (!AccessW(FileBlobOrPathname))
     break;
   PCMSG_SIGNER_INFO pCMsgSignerInfo = nullptr;
   HCERTSTORE hCertStore = nullptr;
   HCRYPTMSG hMsg = nullptr;
   PCCERT_CONTEXT pCertContext = nullptr;
   DWORD dwMsgAndCertEncodingType = 0;
   DWORD dwContentType = 0;
   DWORD dwFormatType = 0;
   DWORD dwSignerInfo = 0;
   CERT_INFO CertInfo = { 0 };
   CERT_BLOB CertBlob = { 0 };
   try {
    do {
     const void* pvObject = nullptr;
     if (blob) {
      CertBlob.cbData = static_cast<DWORD>(FileBlobOrPathname.size());
      CertBlob.pbData = (unsigned char*)FileBlobOrPathname.data();
      pvObject = &CertBlob;
     }
     else {
      pvObject = FileBlobOrPathname.data();
     }
     if (FALSE == ::CryptQueryObject(
      blob ? CERT_QUERY_OBJECT_BLOB : CERT_QUERY_OBJECT_FILE,
      pvObject,
      CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
      CERT_QUERY_FORMAT_FLAG_BINARY,
      0,
      &dwMsgAndCertEncodingType,
      &dwContentType,
      &dwFormatType,
      &hCertStore,
      &hMsg,
      nullptr)) {
      //CRYPT_E_NO_MATCH
      //auto error = ::GetLastError();
      break;
     }
     if (0 == ::CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, nullptr, &dwSignerInfo))
      break;
     pCMsgSignerInfo = (PCMSG_SIGNER_INFO)::LocalAlloc(LPTR, dwSignerInfo);
     if (!pCMsgSignerInfo)
      break;
     if (0 == ::CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, pCMsgSignerInfo, &dwSignerInfo))
      break;
     CertInfo.Issuer = pCMsgSignerInfo->Issuer;
     CertInfo.SerialNumber = pCMsgSignerInfo->SerialNumber;
     pCertContext = ::CertFindCertificateInStore(
      hCertStore,
      PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,
      0,
      CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, nullptr);
     if (!pCertContext)
      break;
     DWORD dwSignSize = ::CertGetNameStringW(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, NULL, 0);
     if (dwSignSize <= 0)
      break;
     signature.resize(dwSignSize, 0x00);
     void(::CertGetNameStringW(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE,
      0, NULL, &signature[0], dwSignSize));
     result = true;
    } while (0);
   }
   catch (...) {
    break;
   }

   if (pCMsgSignerInfo) {
    ::LocalFree((HLOCAL)pCMsgSignerInfo);
    pCMsgSignerInfo = nullptr;
   }
   if (pCertContext) {
    ::CertFreeCertificateContext(pCertContext);
    pCertContext = nullptr;
   }
   if (hCertStore) {
    ::CertCloseStore(hCertStore, 0);
    hCertStore = nullptr;
   }
   if (hMsg) {
    ::CryptMsgClose(hMsg);
    hMsg = nullptr;
   }

  } while (0);
  return result;
 }
 bool Win::File::Attribute::GetFileObjSignA(const std::string& FilePathname, std::string& outSignText) {
  bool result = false;
#if 1 //!@ 旧的方式备份
  PCMSG_SIGNER_INFO pSignerInfo = nullptr;
  HCERTSTORE hCertStore = nullptr;
  HCRYPTMSG hMsg = nullptr;
  PCCERT_CONTEXT pCertContext = nullptr;
  wchar_t* pSignBuffer = nullptr;
  do {
   if (!shared::Win::AccessA(FilePathname))
    break;
   try {
    do {
     DWORD dwObjectType = CERT_QUERY_OBJECT_FILE;
     DWORD dwExpectedContentTypeFlags = CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED;
     DWORD dwExpectedFormatTypeFlags = CERT_QUERY_FORMAT_FLAG_BINARY;
     DWORD dwFlags = 0;
     DWORD dwMsgAndCertEncodingType = 0;
     DWORD dwContentType = 0;
     DWORD dwFormatType = 0;
     CERT_INFO CertInfo = { 0 };
     if (0 == ::CryptQueryObject(
      dwObjectType,
      IConv::MBytesToWString(FilePathname).c_str(),
      dwExpectedContentTypeFlags,
      dwExpectedFormatTypeFlags,
      dwFlags,
      &dwMsgAndCertEncodingType,
      &dwContentType,
      &dwFormatType,
      &hCertStore,
      &hMsg,
      nullptr))
      break;
     DWORD dwSignerInfo = 0;
     DWORD dwParamType = CMSG_SIGNER_INFO_PARAM;
     DWORD dwIndex = 0;
     if (0 == ::CryptMsgGetParam(
      hMsg,
      dwParamType,
      dwIndex,
      nullptr,
      &dwSignerInfo))
      break;
     pSignerInfo = (PCMSG_SIGNER_INFO)::LocalAlloc(LPTR, dwSignerInfo);
     if (!pSignerInfo)
      break;
     if (0 == ::CryptMsgGetParam(
      hMsg,
      dwParamType,
      dwIndex,
      pSignerInfo,
      &dwSignerInfo))
      break;
     CertInfo.Issuer = pSignerInfo->Issuer;
     CertInfo.SerialNumber = pSignerInfo->SerialNumber;
     DWORD dwCertEncodingType = PKCS_7_ASN_ENCODING | X509_ASN_ENCODING;
     DWORD dwFindFlags = 0;
     DWORD dwFindType = CERT_FIND_SUBJECT_CERT;
     pCertContext = ::CertFindCertificateInStore(
      hCertStore,
      dwCertEncodingType,
      dwFindFlags,
      dwFindType,
      (PVOID)&CertInfo,
      nullptr
     );
     if (!pCertContext)
      break;
     DWORD dwSignSize = ::CertGetNameStringW(
      pCertContext,
      CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
      0, NULL, NULL, 0);
     if (dwSignSize <= 0)
      break;
     pSignBuffer = new wchar_t[dwSignSize];
     DWORD dwResultSize = ::CertGetNameStringW(
      pCertContext,
      CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
      0, NULL, pSignBuffer, dwSignSize);
     if (dwResultSize > 0) {
      std::wstring wSign(pSignBuffer, dwResultSize);
      outSignText = IConv::WStringToMBytes(wSign);
     }
    } while (0);
   }
   catch (...) {
    break;
   }
   result = !outSignText.empty();
  } while (0);

  if (pSignerInfo) {
   LocalFree((HLOCAL)pSignerInfo);
   pSignerInfo = nullptr;
  }
  if (pCertContext) {
   ::CertFreeCertificateContext(pCertContext);
   pCertContext = nullptr;
  }
  if (hCertStore) {
   ::CertCloseStore(hCertStore, 0);
   hCertStore = nullptr;
  }
  if (hMsg) {
   ::CryptMsgClose(hMsg);
   hMsg = nullptr;
  }
  SK_DELETE_PTR_BUFFER(pSignBuffer);
#endif

#if 0//!@ 方式 2022/5/2 7:50
  DWORD Error = ERROR_SUCCESS;
  GUID GenericActionId = WINTRUST_ACTION_GENERIC_VERIFY_V2;
  WINTRUST_DATA WintrustData = { 0 };
  WINTRUST_FILE_INFO FileInfo = { 0 };
  WINTRUST_SIGNATURE_SETTINGS SignatureSettings = { 0 };
  CERT_STRONG_SIGN_PARA StrongSigPolicy = { 0 };
  // Setup data structures for calling WinVerifyTrust
  WintrustData.cbStruct = sizeof(WINTRUST_DATA);
  WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
  WintrustData.dwUIChoice = WTD_UI_NONE;
  WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
  WintrustData.dwUnionChoice = WTD_CHOICE_FILE;
  FileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO_);
  std::wstring filepathname = IConv::MBytesToWString(FilePathname);
  FileInfo.pcwszFilePath = filepathname.c_str();
  WintrustData.pFile = &FileInfo;
  // First verify the primary signature (index 0) to determine how many secondary signatures
  // are present. We use WSS_VERIFY_SPECIFIC and dwIndex to do this, also setting
  // WSS_GET_SECONDARY_SIG_COUNT to have the number of secondary signatures returned.
  SignatureSettings.cbStruct = sizeof(WINTRUST_SIGNATURE_SETTINGS);
  SignatureSettings.dwFlags = WSS_GET_SECONDARY_SIG_COUNT | WSS_VERIFY_SPECIFIC;
  SignatureSettings.dwIndex = 0;
  WintrustData.pSignatureSettings = &SignatureSettings;
  if (0) {
   StrongSigPolicy.cbSize = sizeof(CERT_STRONG_SIGN_PARA);
   StrongSigPolicy.dwInfoChoice = CERT_STRONG_SIGN_OID_INFO_CHOICE;
   StrongSigPolicy.pszOID = (LPSTR)szOID_CERT_STRONG_SIGN_OS_CURRENT;
   WintrustData.pSignatureSettings->pCryptoPolicy = &StrongSigPolicy;
  }

  CRYPT_PROVIDER_DATA* pCryptProviderData = nullptr;
  CRYPT_PROVIDER_SGNR* pCryptProviderSgnr = nullptr;
  CRYPT_PROVIDER_CERT* pCryptProviderCert = nullptr;

  do {
   if (ERROR_SUCCESS != ::WinVerifyTrust(NULL, &GenericActionId, &WintrustData))
    break;
   if (!WintrustData.hWVTStateData)
    break;
   pCryptProviderData = ::WTHelperProvDataFromStateData(WintrustData.hWVTStateData);
   if (!pCryptProviderData)
    break;
   pCryptProviderSgnr = ::WTHelperGetProvSignerFromChain(pCryptProviderData, 0, FALSE, 0);
   if (!pCryptProviderSgnr)
    break;
   pCryptProviderCert = ::WTHelperGetProvCertFromChain(pCryptProviderSgnr, 0);
   if (!pCryptProviderCert)
    break;
   DWORD dwSignSize = ::CertGetNameStringW(pCryptProviderCert->pCert,
    CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
    0, NULL, NULL, 0);
   if (dwSignSize <= 0)
    break;
   wchar_t* pSignBuffer = new wchar_t[dwSignSize];
   DWORD dwResultSize = ::CertGetNameStringW(pCryptProviderCert->pCert,
    CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
    0, NULL, pSignBuffer, dwSignSize);
   if (dwResultSize > 0) {
    std::wstring wSign(pSignBuffer, dwResultSize);
    outSignText = IConv::WStringToMBytes(wSign);
   }
   delete[] pSignBuffer;
   pSignBuffer = nullptr;
   result = !outSignText.empty();
  } while (0);
#endif
  return result;
 }
 bool Win::File::Attribute::GetFileObjSignW(const std::wstring& FilePathname, std::wstring& outSignText) {
  bool result = false;
#if 1 //!@ 旧的方式备份
  PCMSG_SIGNER_INFO pSignerInfo = nullptr;
  HCERTSTORE hCertStore = nullptr;
  HCRYPTMSG hMsg = nullptr;
  PCCERT_CONTEXT pCertContext = nullptr;
  wchar_t* pSignBuffer = nullptr;
  do {
   if (!shared::Win::AccessW(FilePathname))
    break;
   try {
    do {
     DWORD dwObjectType = CERT_QUERY_OBJECT_FILE;
     DWORD dwExpectedContentTypeFlags = CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED;
     DWORD dwExpectedFormatTypeFlags = CERT_QUERY_FORMAT_FLAG_BINARY;
     DWORD dwFlags = 0;
     DWORD dwMsgAndCertEncodingType = 0;
     DWORD dwContentType = 0;
     DWORD dwFormatType = 0;
     CERT_INFO CertInfo = { 0 };
     if (0 == ::CryptQueryObject(
      dwObjectType,
      FilePathname.c_str(),
      dwExpectedContentTypeFlags,
      dwExpectedFormatTypeFlags,
      dwFlags,
      &dwMsgAndCertEncodingType,
      &dwContentType,
      &dwFormatType,
      &hCertStore,
      &hMsg,
      nullptr))
      break;
     DWORD dwSignerInfo = 0;
     DWORD dwParamType = CMSG_SIGNER_INFO_PARAM;
     DWORD dwIndex = 0;
     if (0 == ::CryptMsgGetParam(
      hMsg,
      dwParamType,
      dwIndex,
      nullptr,
      &dwSignerInfo))
      break;
     pSignerInfo = (PCMSG_SIGNER_INFO)::LocalAlloc(LPTR, dwSignerInfo);
     if (!pSignerInfo)
      break;
     if (0 == ::CryptMsgGetParam(
      hMsg,
      dwParamType,
      dwIndex,
      pSignerInfo,
      &dwSignerInfo))
      break;
     CertInfo.Issuer = pSignerInfo->Issuer;
     CertInfo.SerialNumber = pSignerInfo->SerialNumber;
     DWORD dwCertEncodingType = PKCS_7_ASN_ENCODING | X509_ASN_ENCODING;
     DWORD dwFindFlags = 0;
     DWORD dwFindType = CERT_FIND_SUBJECT_CERT;


     std::vector<PCCERT_CONTEXT> context_s;
     PCCERT_CONTEXT pos = nullptr;
     do {
      auto current = ::CertEnumCertificatesInStore(hCertStore, pos);
      if (!current)
       break;
      pos = current;
      auto sk = 0;
     } while (1);



     pCertContext = ::CertFindCertificateInStore(
      hCertStore,
      dwCertEncodingType,
      dwFindFlags,
      dwFindType,
      (PVOID)&CertInfo,
      nullptr
     );
     if (!pCertContext)
      break;
     DWORD dwSignSize = ::CertGetNameStringW(
      pCertContext,
      CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
      0, NULL, NULL, 0);
     if (dwSignSize <= 0)
      break;
     pSignBuffer = new wchar_t[dwSignSize];
     DWORD dwResultSize = ::CertGetNameStringW(
      pCertContext,
      CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
      0, NULL, pSignBuffer, dwSignSize);
     if (dwResultSize > 0) {
      std::wstring wSign(pSignBuffer, dwResultSize);
      outSignText = wSign;
     }
    } while (0);
   }
   catch (...) {
    break;
   }
   result = !outSignText.empty();
  } while (0);

  if (pSignerInfo) {
   LocalFree((HLOCAL)pSignerInfo);
   pSignerInfo = nullptr;
  }
  if (pCertContext) {
   ::CertFreeCertificateContext(pCertContext);
   pCertContext = nullptr;
  }
  if (hCertStore) {
   ::CertCloseStore(hCertStore, 0);
   hCertStore = nullptr;
  }
  if (hMsg) {
   ::CryptMsgClose(hMsg);
   hMsg = nullptr;
  }
  SK_DELETE_PTR_BUFFER(pSignBuffer);




#endif

#if 0//!@ 方式 2022/5/2 7:50
  DWORD Error = ERROR_SUCCESS;
  GUID GenericActionId = WINTRUST_ACTION_GENERIC_VERIFY_V2;
  WINTRUST_DATA WintrustData = { 0 };
  WINTRUST_FILE_INFO FileInfo = { 0 };
  WINTRUST_SIGNATURE_SETTINGS SignatureSettings = { 0 };
  CERT_STRONG_SIGN_PARA StrongSigPolicy = { 0 };
  // Setup data structures for calling WinVerifyTrust
  WintrustData.cbStruct = sizeof(WINTRUST_DATA);
  WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
  WintrustData.dwUIChoice = WTD_UI_NONE;
  WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
  WintrustData.dwUnionChoice = WTD_CHOICE_FILE;
  FileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO_);
  std::wstring filepathname = IConv::MBytesToWString(FilePathname);
  FileInfo.pcwszFilePath = filepathname.c_str();
  WintrustData.pFile = &FileInfo;
  // First verify the primary signature (index 0) to determine how many secondary signatures
  // are present. We use WSS_VERIFY_SPECIFIC and dwIndex to do this, also setting
  // WSS_GET_SECONDARY_SIG_COUNT to have the number of secondary signatures returned.
  SignatureSettings.cbStruct = sizeof(WINTRUST_SIGNATURE_SETTINGS);
  SignatureSettings.dwFlags = WSS_GET_SECONDARY_SIG_COUNT | WSS_VERIFY_SPECIFIC;
  SignatureSettings.dwIndex = 0;
  WintrustData.pSignatureSettings = &SignatureSettings;
  if (0) {
   StrongSigPolicy.cbSize = sizeof(CERT_STRONG_SIGN_PARA);
   StrongSigPolicy.dwInfoChoice = CERT_STRONG_SIGN_OID_INFO_CHOICE;
   StrongSigPolicy.pszOID = (LPSTR)szOID_CERT_STRONG_SIGN_OS_CURRENT;
   WintrustData.pSignatureSettings->pCryptoPolicy = &StrongSigPolicy;
  }

  CRYPT_PROVIDER_DATA* pCryptProviderData = nullptr;
  CRYPT_PROVIDER_SGNR* pCryptProviderSgnr = nullptr;
  CRYPT_PROVIDER_CERT* pCryptProviderCert = nullptr;

  do {
   if (ERROR_SUCCESS != ::WinVerifyTrust(NULL, &GenericActionId, &WintrustData))
    break;
   if (!WintrustData.hWVTStateData)
    break;
   pCryptProviderData = ::WTHelperProvDataFromStateData(WintrustData.hWVTStateData);
   if (!pCryptProviderData)
    break;
   pCryptProviderSgnr = ::WTHelperGetProvSignerFromChain(pCryptProviderData, 0, FALSE, 0);
   if (!pCryptProviderSgnr)
    break;
   pCryptProviderCert = ::WTHelperGetProvCertFromChain(pCryptProviderSgnr, 0);
   if (!pCryptProviderCert)
    break;
   DWORD dwSignSize = ::CertGetNameStringW(pCryptProviderCert->pCert,
    CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
    0, NULL, NULL, 0);
   if (dwSignSize <= 0)
    break;
   wchar_t* pSignBuffer = new wchar_t[dwSignSize];
   DWORD dwResultSize = ::CertGetNameStringW(pCryptProviderCert->pCert,
    CERT_NAME_SIMPLE_DISPLAY_TYPE/*CERT_NAME_RDN_TYPE*/,
    0, NULL, pSignBuffer, dwSignSize);
   if (dwResultSize > 0) {
    std::wstring wSign(pSignBuffer, dwResultSize);
    outSignText = IConv::WStringToMBytes(wSign);
   }
   delete[] pSignBuffer;
   pSignBuffer = nullptr;
   result = !outSignText.empty();
  } while (0);
#endif
  return result;
 }
 bool Win::File::Attribute::GetVersionInfoA(_In_ const std::string& FilePathname, _Out_ File::FileVersionInfoA& outFileVersionInfo) {
  bool result = false;
  outFileVersionInfo = File::FileVersionInfoA();
  char* pVersionData = nullptr;
  do {
   if (!Win::AccessA(FilePathname))
    break;
   DWORD dwHandle = 0;
   DWORD dwFileVersionInfoSize = ::GetFileVersionInfoSizeA(FilePathname.c_str(), &dwHandle);
   if (dwFileVersionInfoSize <= 0)
    break;
   pVersionData = new char[dwFileVersionInfoSize];
   // 检索信息
   if (FALSE == ::GetFileVersionInfoA(FilePathname.c_str(), dwHandle, dwFileVersionInfoSize, pVersionData))
    break;
   UINT nQuerySize = 0;
   DWORD* pTransTable = nullptr;
   // 设置语言
   if (FALSE == ::VerQueryValueA(pVersionData, R"(\VarFileInfo\Translation)", (void**)&pTransTable, &nQuerySize))
    break;
   DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

   LPVOID lpBuffer = nullptr;
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(FileDescription)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.FileDescription.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(FileVersion)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.FileVersion.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(InternalName)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.InternalName.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(CompanyName)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.CompanyName.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(LegalCopyright)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.LegalCopyright.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(OriginalFilename)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.OriginalFilename.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(ProductName)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.ProductName.append((char*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueA(pVersionData,
    std::format(R"(\StringFileInfo\{:08x}\{})", dwLangCharset, R"(ProductVersion)").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.ProductVersion.append((char*)lpBuffer, nQuerySize);
   }
   //!@ fix
   //! 
   //! 
   while (!outFileVersionInfo.FileDescription.empty() && *std::prev(outFileVersionInfo.FileDescription.end()) == 0)
    outFileVersionInfo.FileDescription.pop_back();
   while (!outFileVersionInfo.FileVersion.empty() && *std::prev(outFileVersionInfo.FileVersion.end()) == 0)
    outFileVersionInfo.FileVersion.pop_back();
   while (!outFileVersionInfo.InternalName.empty() && *std::prev(outFileVersionInfo.InternalName.end()) == 0)
    outFileVersionInfo.InternalName.pop_back();
   while (!outFileVersionInfo.CompanyName.empty() && *std::prev(outFileVersionInfo.CompanyName.end()) == 0)
    outFileVersionInfo.CompanyName.pop_back();
   while (!outFileVersionInfo.LegalCopyright.empty() && *std::prev(outFileVersionInfo.LegalCopyright.end()) == 0)
    outFileVersionInfo.LegalCopyright.pop_back();
   while (!outFileVersionInfo.OriginalFilename.empty() && *std::prev(outFileVersionInfo.OriginalFilename.end()) == 0)
    outFileVersionInfo.OriginalFilename.pop_back();
   while (!outFileVersionInfo.ProductName.empty() && *std::prev(outFileVersionInfo.ProductName.end()) == 0)
    outFileVersionInfo.ProductName.pop_back();
   while (!outFileVersionInfo.ProductVersion.empty() && *std::prev(outFileVersionInfo.ProductVersion.end()) == 0)
    outFileVersionInfo.ProductVersion.pop_back();
   result = true;
  } while (0);
  SK_DELETE_PTR_BUFFER(pVersionData);
  return result;
 }
 bool Win::File::Attribute::GetVersionInfoW(_In_ const std::wstring& FilePathname, _Out_ File::FileVersionInfoW& outFileVersionInfo) {
  bool result = false;
  outFileVersionInfo = File::FileVersionInfoW();
  std::uint8_t* pVersionData = nullptr;
  do {
   if (!Win::AccessW(FilePathname))
    break;
   DWORD dwHandle = 0;
   DWORD dwFileVersionInfoSize = ::GetFileVersionInfoSizeW(FilePathname.c_str(), &dwHandle);
   if (dwFileVersionInfoSize <= 0)
    break;
   pVersionData = new std::uint8_t[dwFileVersionInfoSize];
   // 检索信息
   if (FALSE == ::GetFileVersionInfoW(FilePathname.c_str(), dwHandle, dwFileVersionInfoSize, pVersionData))
    break;
   UINT nQuerySize = 0;
   DWORD* pTransTable = nullptr;
   // 设置语言
   if (FALSE == ::VerQueryValueW(pVersionData, LR"(\VarFileInfo\Translation)", (void**)&pTransTable, &nQuerySize))
    break;
   DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

   LPVOID lpBuffer = nullptr;
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"FileDescription").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.FileDescription.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"FileVersion").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.FileVersion.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"InternalName").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.InternalName.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"CompanyName").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.CompanyName.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"LegalCopyright").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.LegalCopyright.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"OriginalFilename").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.OriginalFilename.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"ProductName").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.ProductName.append((wchar_t*)lpBuffer, nQuerySize);
   }
   if (TRUE == ::VerQueryValueW(pVersionData,
    std::format(LR"(\StringFileInfo\{:08x}\{})", dwLangCharset, L"ProductVersion").c_str(),
    &lpBuffer,
    &nQuerySize)) {
    outFileVersionInfo.ProductVersion.append((wchar_t*)lpBuffer, nQuerySize);
   }

   //!@ fix
   while (!outFileVersionInfo.FileDescription.empty() && *std::prev(outFileVersionInfo.FileDescription.end()) == 0)
    outFileVersionInfo.FileDescription.pop_back();
   while (!outFileVersionInfo.FileVersion.empty() && *std::prev(outFileVersionInfo.FileVersion.end()) == 0)
    outFileVersionInfo.FileVersion.pop_back();
   while (!outFileVersionInfo.InternalName.empty() && *std::prev(outFileVersionInfo.InternalName.end()) == 0)
    outFileVersionInfo.InternalName.pop_back();
   while (!outFileVersionInfo.CompanyName.empty() && *std::prev(outFileVersionInfo.CompanyName.end()) == 0)
    outFileVersionInfo.CompanyName.pop_back();
   while (!outFileVersionInfo.LegalCopyright.empty() && *std::prev(outFileVersionInfo.LegalCopyright.end()) == 0)
    outFileVersionInfo.LegalCopyright.pop_back();
   while (!outFileVersionInfo.OriginalFilename.empty() && *std::prev(outFileVersionInfo.OriginalFilename.end()) == 0)
    outFileVersionInfo.OriginalFilename.pop_back();
   while (!outFileVersionInfo.ProductName.empty() && *std::prev(outFileVersionInfo.ProductName.end()) == 0)
    outFileVersionInfo.ProductName.pop_back();
   while (!outFileVersionInfo.ProductVersion.empty() && *std::prev(outFileVersionInfo.ProductVersion.end()) == 0)
    outFileVersionInfo.ProductVersion.pop_back();
   result = true;
  } while (0);
  SK_DELETE_PTR_BUFFER(pVersionData);
  return result;
 }
#if 0//!@ 未完成
 bool Win::File::Attribute::QueryFileInfo(const std::string& Filepathname, __inout std::map<std::string, std::string>& parameters) {
  bool result = false;
  do {
   if (parameters.empty())
    parameters = {
     {"FileDescription",""},
{"FileVersion",""},
{"InternalName",""},
{"CompanyName",""},
{"LegalCopyright",""},
{"OriginalFilename",""},
{"ProductName",""},
{"ProductVersion",""}, };


   char* m_lpVersionData = nullptr;
   do {
    if (Filepathname.empty())
     break;
    DWORD dwHandle = 0;
    DWORD dwDataSize = ::GetFileVersionInfoSizeA(Filepathname.c_str(), &dwHandle);
    if (dwDataSize <= 0)
     break;
    m_lpVersionData = new char[dwDataSize];// 分配缓冲区
    if (!m_lpVersionData)
     break;
    ::memset(m_lpVersionData, 0x00, dwDataSize);
    dwHandle = 0;
    if (FALSE == ::GetFileVersionInfoA(Filepathname.c_str(), dwHandle, dwDataSize, (void*)m_lpVersionData))
     break;
    UINT nQuerySize = 0;
    DWORD* pTransTable = nullptr;
    if (!::VerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation", (void**)&pTransTable, &nQuerySize))
     break;
    if (m_lpVersionData == NULL)
     break;
    DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
    char formatstr[512] = { 0 };
    ::sprintf_s(formatstr, 512, "\\StringFileInfo\\%08x\\%ws", dwLangCharset, ValueName.c_str());
    LPVOID lpData = nullptr;
    if (!::VerQueryValueA(m_lpVersionData, formatstr, &lpData, &nQuerySize) || !lpData || nQuerySize <= 0)
     break;
    RetStr.append((wchar_t*)lpData, nQuerySize);
    result = !RetStr.empty();
   } while (0);
   SK_DELETE_PTR_BUFFER(m_lpVersionData);
  } while (0);
  return result;
 }
#endif
 bool Win::File::Attribute::QueryValueW(const std::wstring& ValueName, const std::wstring& szModuleName, std::wstring& RetStr) {
  bool result = false;
  RetStr.clear();
  wchar_t* m_lpVersionData = nullptr;
  do {
   if (!ValueName.size() || !szModuleName.size())
    break;
   DWORD dwHandle = 0;
   DWORD dwDataSize = ::GetFileVersionInfoSizeW(szModuleName.c_str(), &dwHandle);
   if (dwDataSize == 0)
    break;
   m_lpVersionData = new (std::nothrow) wchar_t[dwDataSize + 1];// 分配缓冲区
   if (!m_lpVersionData)
    break;
   dwHandle = 0;
   if (FALSE == ::GetFileVersionInfoW(szModuleName.c_str(), dwHandle, dwDataSize + 1, (void*)m_lpVersionData))
    break;
   UINT nQuerySize = 0;
   DWORD* pTransTable = nullptr;
   if (!::VerQueryValueW(m_lpVersionData, L"\\VarFileInfo\\Translation", (void**)&pTransTable, &nQuerySize))
    break;
   if (m_lpVersionData == NULL)
    break;
   DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
   wchar_t formatstr[512] = { 0 };
   ::swprintf_s(formatstr, 512, L"\\StringFileInfo\\%08x\\%ws", dwLangCharset, ValueName.c_str());
   LPVOID lpData = nullptr;
   if (!::VerQueryValueW(m_lpVersionData, formatstr, &lpData, &nQuerySize) || !lpData || nQuerySize <= 0)
    break;
   RetStr.append((wchar_t*)lpData, nQuerySize);
   result = !RetStr.empty();
  } while (0);
  SK_DELETE_PTR_BUFFER(m_lpVersionData);
  return result;
 }
 bool Win::File::Attribute::QueryValueA(const std::string& ValueName, const std::string& szModuleName, std::string& RetStr) {
  bool result = false;
  RetStr.clear();
  BYTE* m_lpVersionData = nullptr;
  do {
   if (!ValueName.size() || !szModuleName.size())
    break;
   DWORD dwHandle = 0;
   // 判断系统能否检索到指定文件的版本信息
   DWORD dwDataSize = ::GetFileVersionInfoSizeA((LPCSTR)szModuleName.c_str(), &dwHandle);
   if (dwDataSize == 0)
    break;
   m_lpVersionData = new (std::nothrow) BYTE[dwDataSize + 1];// 分配缓冲区
   if (NULL == m_lpVersionData)
    break;
   dwHandle = 0;
   if (FALSE == ::GetFileVersionInfoA((LPCSTR)szModuleName.c_str(), dwHandle, dwDataSize + 1, (void*)m_lpVersionData))
    break;
   UINT nQuerySize = 0;
   DWORD* pTransTable = nullptr;
   // 设置语言
   if (FALSE == ::VerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation", (void**)&pTransTable, &nQuerySize))
    break;
   if (m_lpVersionData == NULL)
    break;
   std::wstring wValueName = IConv::MBytesToWString(ValueName);
   if (wValueName.empty())
    break;
   DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
   wchar_t formatstr[512] = { 0 };
   ::swprintf_s(formatstr, 512, L"\\StringFileInfo\\%08x\\%ws", dwLangCharset, wValueName.c_str());
   LPVOID lpData = nullptr;
   // 调用此函数查询前需要先依次调用函数GetFileVersionInfoSize和GetFileVersionInfo
   if (!::VerQueryValueW(m_lpVersionData, formatstr, &lpData, &nQuerySize) || !lpData || nQuerySize <= 0)
    break;
   RetStr = IConv::WStringToMBytes(std::wstring((wchar_t*)lpData, nQuerySize));
   result = true;
  } while (0);
  SK_DELETE_PTR_BUFFER(m_lpVersionData);
  return result;
 }



 //获取文件说明 --- 以下方法效率低
 bool Win::File::Attribute::GetFileDescription(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("FileDescription", szModuleName, RetStr);
 };
 //获取文件版本	
 bool Win::File::Attribute::GetFileVersion(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("FileVersion", szModuleName, RetStr);
 };
 //获取内部名称
 bool Win::File::Attribute::GetInternalName(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("InternalName", szModuleName, RetStr);
 };
 //获取公司名称
 bool Win::File::Attribute::GetCompanyName(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("CompanyName", szModuleName, RetStr);
 };
 //获取版权
 bool Win::File::Attribute::GetLegalCopyright(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("LegalCopyright", szModuleName, RetStr);
 };
 //获取原始文件名
 bool Win::File::Attribute::GetOriginalFilename(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("OriginalFilename", szModuleName, RetStr);
 };
 bool Win::File::Attribute::GetOriginalFilenameW(const std::wstring& szModuleName, std::wstring& result) {
  return QueryValueW(L"OriginalFilename", szModuleName, result);
 }
 //获取产品名称
 bool Win::File::Attribute::GetProductName(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("ProductName", szModuleName, RetStr);
 };
 bool Win::File::Attribute::GetProductNameW(const std::wstring& szModuleName, std::wstring& RetStr) {
  return QueryValueW(L"ProductName", szModuleName, RetStr);
 }
 //获取产品版本
 bool Win::File::Attribute::GetProductVersion(const std::string& szModuleName, std::string& RetStr) {
  return QueryValueA("ProductVersion", szModuleName, RetStr);
 };




}///namespace shared