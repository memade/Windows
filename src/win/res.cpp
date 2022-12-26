#include "stdafx.h"

namespace shared {

 static bool IsZipCompress(const std::string& buffer) {
  bool result = false;
  do {
   if (buffer.empty())
    break;
   if (buffer.size() < sizeof(__int64))
    break;
   __int64 head = 0;
   ::memcpy(&head, buffer.data(), sizeof(__int64));
   result = (head << (8 * 4)) >> (8 * 4) == 0x04034b50;///*0x0000001404034b50*/
  } while (0);
  return result;
 }
#ifdef WINLIB_DISABLE_ZIPPP_H
 bool Win::Resource::UnRespak(_In_ const std::string& respak_buffer, _Out_ std::string& out_buffer, \
  const std::function<bool(const std::string&, const unsigned long& origin_size, std::string&)>& unzip_cb) {
  bool result = false;
  out_buffer.clear();
  do {
   if (respak_buffer.empty())
    break;
   if (unzip_cb) {
    unsigned long origin_size = 0;
    ::memcpy(&origin_size, respak_buffer.data() + (respak_buffer.size() - sizeof(unsigned long)), sizeof(origin_size));
    std::string unzip;
    if (origin_size <= 0 || !unzip_cb(std::string(respak_buffer.data(), respak_buffer.size() - sizeof(unsigned long)), origin_size, unzip))
     break;
    out_buffer = unzip;
   }
   else
    out_buffer = respak_buffer;
   result = !out_buffer.empty();
  } while (0);
  return result;
 }
#else
 bool Win::Resource::UnRespak(_In_ const std::string& respak_buffer, _Out_ std::string& out_buffer) {
  bool result = false;
  out_buffer.clear();
  do {
   unsigned long origin_size = 0;
   ::memcpy(&origin_size, respak_buffer.data() + (respak_buffer.size() - sizeof(unsigned long)), sizeof(origin_size));
   if (origin_size <= 0)
    break;
   if (shared::Zip::zipUnCompress(std::string(respak_buffer.data(), respak_buffer.size() - sizeof(unsigned long)), origin_size, out_buffer) != Z_OK)
    break;
   result = true;
  } while (0);
  return result;
 }
#endif
 bool Win::Resource::Load(_In_ const HINSTANCE& hInstance, _In_ const DWORD& CustomRes, _In_ LPCSTR pCustomResName, std::string& outRes) {
  bool result = false;
  outRes.clear();
  HGLOBAL hGlobal = nullptr;
  do {
   HRSRC hRsrc = ::FindResourceA(hInstance, MAKEINTRESOURCEA(CustomRes), pCustomResName);
   if (!hRsrc)
    break;
   size_t nSize = ::SizeofResource(hInstance, hRsrc);
   if (nSize <= 0)
    break;
   hGlobal = ::LoadResource(hInstance, hRsrc);
   if (!hGlobal)
    break;
   void* pBuffer = ::LockResource(hGlobal);
   if (!pBuffer)
    break;
   outRes.append((char*)pBuffer, nSize);
   result = true;
  } while (0);
  if (hGlobal) {
   ::GlobalUnlock(hGlobal);
   hGlobal = nullptr;
  }
  return result;
 }
 std::string Win::Resource::Load(_In_ const HINSTANCE& hInstance, _In_ const DWORD& CustomRes, _In_ LPCSTR pCustomResName) {
  std::string result;
  HGLOBAL hGlobal = nullptr;
  do {
   HRSRC hRsrc = ::FindResourceA(hInstance, MAKEINTRESOURCEA(CustomRes), pCustomResName);
   if (!hRsrc)
    break;
   size_t nSize = ::SizeofResource(hInstance, hRsrc);
   if (nSize <= 0)
    break;
   hGlobal = ::LoadResource(hInstance, hRsrc);
   if (!hGlobal)
    break;
   void* pBuffer = ::LockResource(hGlobal);
   if (!pBuffer)
    break;
   result.append((char*)pBuffer, nSize);
  } while (0);
  if (hGlobal) {
   ::GlobalUnlock(hGlobal);
   hGlobal = nullptr;
  }
  return result;
 }

 bool Win::Resource::UnMadeRoute(const tfRoutePak& route_data, tfRouteRes& outpak) {
  bool result = false;
  outpak.clear();
  do {
   if (route_data.empty())
    break;
   const char* begin = route_data.data();
   auto pHead = (shared::nt::PROUTEDATAHEAD)(begin);
   if (!pHead->Verify())
    break;
   unsigned long info_pos = sizeof(shared::nt::ROUTEDATAHEAD);
   for (decltype(pHead->data_count) i = 0; i < pHead->data_count; ++i) {
    auto pInfo = (shared::nt::ROUTEDATAINFO*)(begin + info_pos);
    info_pos += sizeof(shared::nt::ROUTEDATAINFO);
    outpak.emplace(pInfo->data_identify, std::string((char*)(begin + info_pos), pInfo->data_size));
    info_pos += pInfo->data_size;
   }
   result = !outpak.empty();
  } while (0);
  return result;
 }
 bool Win::Resource::MadeRoute(const tfRouteRes& ress, tfRoutePak& outres) {
  bool result = false;
  outres.clear();
  do {
   if (ress.empty())
    break;
   shared::nt::ROUTEDATAHEAD head;
   head.data_count = static_cast<decltype(head.data_count)>(ress.size());
   outres.append((char*)&head, sizeof(head));

   std::for_each(ress.begin(), ress.end(),
    [&](const auto& node) {
     shared::nt::ROUTEDATAINFO dataInfo;
   dataInfo.data_identify = node.first;
   dataInfo.data_size = static_cast<decltype(dataInfo.data_size)>(node.second.size());
   dataInfo.data_postion = static_cast<decltype(dataInfo.data_postion)>(outres.size());
   outres.append((char*)&dataInfo, sizeof(dataInfo));
   if (dataInfo.data_size > 0)
    outres.append(node.second);
   ((shared::nt::ROUTEDATAHEAD*)outres.data())->data_size += dataInfo.data_size;
    });
   result = !outres.empty();
  } while (0);
  return result;
 }

}///namespace shared