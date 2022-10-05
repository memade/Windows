#include "stdafx.h"
#include <Sddl.h>

namespace shared {

 static const unsigned kSHA1Length = 20;

 const unsigned char kCrcTable[256] = {
  0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
  0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
  0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
  0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
  0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
  0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
  0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
  0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
  0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
  0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
  0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
  0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
  0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
  0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
  0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
  0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
  0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
  0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
  0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
  0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
  0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
  0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
  0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
  0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
  0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
  0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
  0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
  0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
  0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
  0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
  0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
  0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
 };

 Win::Kaimi::Kaimi() {
  GenerateMachineID();
  GenerateUserRawID();
 }

 Win::Kaimi::~Kaimi() {

 }
 const std::string& Win::Kaimi::device_id() const { return m_device_id; }
 const std::string& Win::Kaimi::machine_id() const { return m_machine_id; }
 const std::string& Win::Kaimi::machine_full_id() const { return m_machine_full_id; }
 bool Win::Kaimi::GetSystemVolumeSerialNumber(int* number) {
  if (!number)
   return false;

  *number = 0;

  // Find the system root path (e.g: C:\).
  wchar_t system_path[MAX_PATH + 1];
  if (!GetSystemDirectoryW(system_path, MAX_PATH))
   return false;

  wchar_t* first_slash = wcspbrk(system_path, L"\\/");
  if (first_slash != NULL)
   *(first_slash + 1) = 0;

  DWORD number_local = 0;
  if (!GetVolumeInformationW(system_path, NULL, 0, &number_local, NULL, NULL,
   NULL, 0))
   return false;

  *number = number_local;
  return true;
 }

 bool Win::Kaimi::GetComputerSid(const wchar_t* account_name, SID* sid, DWORD sid_size) {
  static const DWORD kStartDomainLength = 128;  // reasonable to start with

  std::wstring domain_buffer(kStartDomainLength, 0);
  DWORD domain_size = kStartDomainLength;
  DWORD sid_dword_size = sid_size;
  SID_NAME_USE sid_name_use;

  BOOL success = ::LookupAccountNameW(NULL, account_name, sid,
   &sid_dword_size, &domain_buffer[0],
   &domain_size, &sid_name_use);
  if (!success && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
   // We could have gotten the insufficient buffer error because
   // one or both of sid and szDomain was too small. Check for that
   // here.
   if (sid_dword_size > sid_size)
    return false;

   if (domain_size > kStartDomainLength)
    domain_buffer.resize(domain_size);

   success = ::LookupAccountNameW(NULL, account_name, sid, &sid_dword_size,
    &domain_buffer[0], &domain_size,
    &sid_name_use);
  }

  return success != FALSE;
 }

 std::wstring Win::Kaimi::ConvertSidToString(SID* sid) {
  std::wstring sid_string;
#if _WIN32_WINNT >= 0x500
  wchar_t* sid_buffer = NULL;
  if (ConvertSidToStringSidW(sid, &sid_buffer)) {
   sid_string = sid_buffer;
   LocalFree(sid_buffer);
  }
#else
  SID_IDENTIFIER_AUTHORITY* sia = ::GetSidIdentifierAuthority(sid);

  if (sia->Value[0] || sia->Value[1]) {
   base::SStringPrintf(
    &sid_string, L"S-%d-0x%02hx%02hx%02hx%02hx%02hx%02hx",
    SID_REVISION, (USHORT)sia->Value[0], (USHORT)sia->Value[1],
    (USHORT)sia->Value[2], (USHORT)sia->Value[3], (USHORT)sia->Value[4],
    (USHORT)sia->Value[5]);
  }
  else {
   ULONG authority = 0;
   for (int i = 2; i < 6; ++i) {
    authority <<= 8;
    authority |= sia->Value[i];
   }
   base::SStringPrintf(&sid_string, L"S-%d-%lu", SID_REVISION, authority);
  }

  int sub_auth_count = *::GetSidSubAuthorityCount(sid);
  for (int i = 0; i < sub_auth_count; ++i)
   base::StringAppendF(&sid_string, L"-%lu", *::GetSidSubAuthority(sid, i));
#endif

  return sid_string;
 }


 bool Win::Kaimi::GetRawMachineId(std::wstring& sid_string, int* volume_id) {
  // Calculate the Windows SID.
  wchar_t computer_name[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
  DWORD size = ARRAYSIZE(computer_name);
  if (GetComputerNameW(computer_name, &size)) {
   char sid_buffer[SECURITY_MAX_SID_SIZE];
   SID* sid = reinterpret_cast<SID*>(sid_buffer);
   if (GetComputerSid(computer_name, sid, SECURITY_MAX_SID_SIZE)) {
    sid_string = ConvertSidToString(sid);
   }
  }
  // Get the system drive volume serial number.
  *volume_id = 0;
  if (!GetSystemVolumeSerialNumber(volume_id)) {
   *volume_id = 0;
  }
  return true;
 }

 std::string Win::Kaimi::SHA1HashString(const std::string& str) {
  HCRYPTPROV provider;
  if (!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL,
   CRYPT_VERIFYCONTEXT)) {
   return std::string(kSHA1Length, '\0');
  }

  {
   HCRYPTHASH hash;
   if (!CryptCreateHash(provider, CALG_SHA1, 0, 0, &hash)) {
    return std::string(kSHA1Length, '\0');
   }

   if (!CryptHashData(hash, reinterpret_cast<CONST BYTE*>(str.data()),
    static_cast<DWORD>(str.length()), 0)) {
    return std::string(kSHA1Length, '\0');
   }

   DWORD hash_len = 0;
   DWORD buffer_size = sizeof hash_len;
   if (!CryptGetHashParam(hash, HP_HASHSIZE,
    reinterpret_cast<unsigned char*>(&hash_len),
    &buffer_size, 0)) {
    return std::string(kSHA1Length, '\0');
   }

   std::string result;

   result.reserve(hash_len + 1);
   result.resize(hash_len);
   if (!CryptGetHashParam(hash, HP_HASHVAL,
    // We need the + 1 here not because the call will write a trailing \0,
    // but so that result.length() is correctly set to |hash_len|.
    //reinterpret_cast<BYTE*>(WriteInto(&result, hash_len + 1)), &hash_len,
    reinterpret_cast<BYTE*>(&result[0]), &hash_len,
    0)) {

    return std::string(kSHA1Length, '\0');
   }

   if (hash_len != kSHA1Length) {
    return std::string(kSHA1Length, '\0');
   }

   return result;
  }
 }

 bool Win::Kaimi::Crc8Generate(const unsigned char* data, int length,
  unsigned char* check_sum) {
  if (!check_sum)
   return false;

  *check_sum = 0;
  if (!data)
   return false;

  // The inital and final constants are as used in the ATM HEC.
  static const unsigned char kInitial = 0x00;
  static const unsigned char kFinal = 0x55;
  unsigned char crc = kInitial;
  for (int i = 0; i < length; ++i) {
   crc = kCrcTable[(data[i] ^ crc) & 0xFFU];
  }

  *check_sum = crc ^ kFinal;
  return true;
 }

 bool Win::Kaimi::BytesToString(const unsigned char* data, int data_len, std::string* string) {
  if (!string)
   return false;

  string->clear();
  if (data_len < 1 || !data)
   return false;

  static const char kHex[] = "0123456789ABCDEF";

  // Fix the buffer size to begin with to avoid repeated re-allocation.
  string->resize(data_len * 2);
  int index = data_len;
  while (index--) {
   string->at(2 * index) = kHex[data[index] >> 4];  // high digit
   string->at(2 * index + 1) = kHex[data[index] & 0x0F];  // low digit
  }

  return true;
 }

 bool Win::Kaimi::GetMachineIdImpl(const std::wstring& sid_string, int volume_id, std::string* machine_id) {
  machine_id->clear();

  // The ID should be the SID hash + the Hard Drive SNo. + checksum byte.
  static const int kSizeWithoutChecksum = kSHA1Length + sizeof(int);
  std::basic_string<unsigned char> id_binary(kSizeWithoutChecksum + 1, 0);

  if (!sid_string.empty()) {
   // In order to be compatible with the old version of RLZ, the hash of the
   // SID must be done with all the original bytes from the unicode string.
   // However, the chromebase SHA1 hash function takes only an std::string as
   // input, so the unicode string needs to be converted to std::string
   // "as is".
   size_t byte_count = sid_string.size() * sizeof(wchar_t);
   const char* buffer = reinterpret_cast<const char*>(sid_string.c_str());
   std::string sid_string_buffer(buffer, byte_count);

   // Note that digest can have embedded nulls.
   std::string digest(SHA1HashString(sid_string_buffer));
   if (digest.size() != kSHA1Length)
    return false;
   std::copy(digest.begin(), digest.end(), id_binary.begin());
  }

  // Convert from int to binary (makes big-endian).
  for (int i = 0; i < sizeof(int); i++) {
   int shift_bits = 8 * (sizeof(int) - i - 1);
   id_binary[kSHA1Length + i] = static_cast<unsigned char>(
    (volume_id >> shift_bits) & 0xFF);
  }

  // Append the checksum byte.
  if (!sid_string.empty() || (0 != volume_id))
   Crc8Generate(id_binary.c_str(),
    kSizeWithoutChecksum,
    &id_binary[kSizeWithoutChecksum]);

  return BytesToString(
   id_binary.c_str(), kSizeWithoutChecksum + 1, machine_id);
 }

 bool Win::Kaimi::GetMachineId(std::string* machine_id) {
  if (!machine_id)
   return false;

  static std::string calculated_id;
  static bool calculated = false;
  if (calculated) {
   *machine_id = calculated_id;
   return true;
  }

  std::wstring sid_string;
  int volume_id = 0;
  if (!GetRawMachineId(sid_string, &volume_id))
   return false;

  m_device_id = IConv::WStringToMBytes(sid_string);

  if (!GetMachineIdImpl(sid_string, volume_id, machine_id))
   return false;

  calculated = true;
  calculated_id = *machine_id;

  do {//!@ full machine id
   std::string full_user_sid = Win::GetCurrentUserSid();
   if (full_user_sid.empty() || volume_id<=0)
    break;
   if (!GetMachineIdImpl(shared::IConv::MBytesToWString(full_user_sid), volume_id, &m_machine_full_id))
    break;
  } while (0);


  return true;
 }

 bool Win::Kaimi::GenerateMachineID() {
  if (!GetMachineId(&m_machine_id))
   return false;
  return !m_machine_id.empty();
 }
 bool Win::Kaimi::GenerateUserRawID() {
  wchar_t user_name[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
  DWORD size = ARRAYSIZE(user_name);
  do {
   if (!GetUserNameW(user_name, &size))
    break;
   char sid_buffer[SECURITY_MAX_SID_SIZE];
   SID* sid = reinterpret_cast<SID*>(sid_buffer);
   if (!GetComputerSid(user_name, sid, SECURITY_MAX_SID_SIZE))
    break;
   m_userraw_id = IConv::WStringToMBytes(ConvertSidToString(sid));
  } while (0);
#if 0
  // Get the system drive volume serial number.
  int volume_id = 0;
  if (!GetSystemVolumeSerialNumber(&volume_id)) {
   volume_id = 0;
  }
#endif
  return !m_userraw_id.empty();
 }

 const std::string& Win::Kaimi::GetUserRawID() const {
  return m_userraw_id;
 }

 std::string Win::Kaimi::GetCurrentUserIdSha1Crc8(const std::string& current_user_sid) {
  std::string result;
  do {
   if (current_user_sid.empty())
    break;



   auto sk = 0;
  } while (0);
  return result;
 }
}///namespace shared