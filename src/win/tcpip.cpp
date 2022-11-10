#include "stdafx.h"

namespace shared {
 const size_t MAX_IPADDR_STRING = ::strlen("255.255.255.255\0");


 //本机网络连接类型(成功)  
#define  NET_TYPE_RAS_DIAL_UP_CONNECT_NET           0x01    //上网类型：采用RAS拨号连接上网    0x01  
#define  NET_TYPE_LAN_CONNECT_NET                   0x02    //上网类型：采用网卡通过局域网上网 0x02  
#define  NET_TYPE_PROXY_CONNECT_NET                 0x04    //上网类型：使用代理服务器上网     0x04  
#define  NET_TYPE_RAS_INSTALL                       0x10    //RAS安装                        0x10  
//本机网络连接(失败)  
#define  NET_TYPE_NO_CONNECT_NET                    0x41    //未连接到网络  
#define  NET_STATE_VALID_CONNECT_NOCONNECT_NET      0x40    //可以联网，但当前不可用    0x40  
#define  NET_STATE_MODEM_BUSY                       0x08    //调制解调器 繁忙           0x08  
#define  NET_STATE_SYSTEM_OFFLINE_MODE              0x20    //系统脱机模式              0x20  

#if !defined(WINLIB_DISABLE_WININET)
  //!@ 检测网络状态
 static DWORD NetGetConnectedState()
 {
  DWORD dwOnline;
  if (!::InternetGetConnectedState(&dwOnline, 0))
  {
   if (dwOnline & INTERNET_CONNECTION_CONFIGURED)
   {
    return NET_STATE_VALID_CONNECT_NOCONNECT_NET;
   }
   if (dwOnline & INTERNET_CONNECTION_MODEM_BUSY)
   {
    return NET_STATE_MODEM_BUSY;
   }
   if (dwOnline & INTERNET_CONNECTION_OFFLINE)
   {
    return NET_STATE_SYSTEM_OFFLINE_MODE;
   }
   return NET_TYPE_NO_CONNECT_NET;
  }
  if (dwOnline & INTERNET_CONNECTION_MODEM)      //上网类型：采用RAS拨号连接上网
  {
   return  NET_TYPE_RAS_DIAL_UP_CONNECT_NET;
  }
  else if (dwOnline & INTERNET_CONNECTION_LAN)  //上网类型：采用网卡通过局域网上网
  {
   return NET_TYPE_LAN_CONNECT_NET;
  }
  else if (dwOnline & INTERNET_CONNECTION_PROXY) //在线：代理
  {
   return NET_TYPE_PROXY_CONNECT_NET;
  }
  else if (dwOnline & INTERNET_CONNECTION_MODEM_BUSY) //MODEM被其他非INTERNET连接占用
  {
   return NET_TYPE_RAS_INSTALL;
  }
  return 0;
 }

 bool Win::Tcpip::NetConnectIs() {
  switch (NetGetConnectedState()) {
  case NET_TYPE_RAS_DIAL_UP_CONNECT_NET:
  case NET_TYPE_LAN_CONNECT_NET:
  case NET_TYPE_PROXY_CONNECT_NET:
  case NET_TYPE_RAS_INSTALL:
   return true;
  }
  return false;
 }
#endif




 std::string Win::Tcpip::ipv4_ltoa(const unsigned long& ip_integer) {
  std::string result;
  do {
   struct in_addr inaddr = { 0 };
   inaddr.S_un.S_addr = ip_integer;
   result = ::inet_ntoa(inaddr);
  } while (0);
  return result;
 }
 unsigned long Win::Tcpip::ipv4_atol(const std::string& ip_str) {
  unsigned long result = 0;
  do {
   if (ip_str.empty())
    break;
   struct sockaddr_in antelope;
   if (1 != ::inet_pton(AF_INET, ip_str.c_str(), &(antelope.sin_addr)))
    break;
   result = antelope.sin_addr.S_un.S_addr;
  } while (0);
  return result;
 }

 bool Win::Tcpip::ipv4_inetntop(const void* sockaddr_or_sockaddr_in, std::string& ip, unsigned short& port) {
  bool result = false;
  ip.clear();
  port = 0;
  do {
   struct sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(const_cast<void*>(sockaddr_or_sockaddr_in));
   if (!addr_in)
    break;
   ip.resize(MAX_IPADDR_STRING, 0x00);
   result = ::inet_ntop(AF_INET, &addr_in->sin_addr, &ip[0], ip.size()) != nullptr;
   if (result) {
    for (auto it = ip.begin(); it != ip.end();) {
     if (*it == 0)
      it = ip.erase(it);
     else
      ++it;
    }
    port = addr_in->sin_port;
   }
  } while (0);
  return result;
 }
 bool Win::Tcpip::ipv4_inetpton(const std::string& ip, const unsigned short& port, void* sockaddr_or_sockaddr_in) {
  bool result = false;
  do {
   if (!sockaddr_or_sockaddr_in)
    break;
   if (ip.empty() || port <= 0)
    break;
   if (ip.size() > MAX_IPADDR_STRING)
    break;
   struct sockaddr_in* sockin = reinterpret_cast<struct sockaddr_in*>(sockaddr_or_sockaddr_in);
   result = ::inet_pton(AF_INET, ip.c_str(), &sockin->sin_addr) > 0;
   if (!result)
    break;
   sockin->sin_port = port;
   sockin->sin_family = AF_INET;
  } while (0);
  return result;
 }
 std::uint64_t Win::Tcpip::GetLocalMachineUseNetWorkCardMACEx() {
  std::uint64_t result = 0;
  do {
   std::set<std::string> macs;
   Tcpip::GetLocalMachineAllNetWorkCardMAC(macs);
   if (macs.empty())
    break;
   for (const auto& mac : macs) {
    result = ::strtoull(mac.c_str(), nullptr, 16);
    if (result != 0)
     break;
   }
  } while (0);
  return result;
 }
 std::uint64_t Win::Tcpip::GetLocalMachineUseNetWorkCardMAC() {
  std::uint64_t result = 0;
  PIP_ADAPTER_INFO pAdapterInfo = nullptr;
  PIP_ADAPTER_INFO pAdapter = nullptr;
  DWORD dwRetVal = 0;
  UINT i = 0;
  /* variables used to print DHCP time info */
  struct tm newtime = { 0 };
  char buffer[32] = { 0 };
  errno_t error = 0;
  do {
   ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
   pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
   if (pAdapterInfo == NULL)
    break;
   // Make an initial call to GetAdaptersInfo to get
   // the necessary size into the ulOutBufLen variable
   if (::GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
    ::free(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    if (!pAdapterInfo)
     break;
   }

   if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR)
    break;
   pAdapter = pAdapterInfo;
   while (pAdapter) {
    if (pAdapter->DhcpEnabled) {
     if (pAdapter->LeaseObtained > 10) {
      for (i = 0; i < __min(MAX_ADAPTER_ADDRESS_LENGTH, pAdapter->AddressLength); i++) {
       result <<= 8;
       result |= pAdapter->Address[i];
      }
      if (result)
       break;//!@ Take only one network card address
     }
    }
    pAdapter = pAdapter->Next;
   }
  } while (0);

  SK_DELETE_PTR_C(pAdapterInfo);
  return result;
 }
 bool Win::Tcpip::GetLocalMachineUseNetWorkCardMAC(std::string& mac, const bool& lower /*= false*/) {
  PIP_ADAPTER_INFO pAdapterInfo = nullptr;
  PIP_ADAPTER_INFO pAdapter = nullptr;
  DWORD dwRetVal = 0;
  UINT i = 0;
  /* variables used to print DHCP time info */
  struct tm newtime = { 0 };
  char buffer[32] = { 0 };
  errno_t error = 0;
  mac.clear();
  bool result = false;
  do {
   ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
   pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
   if (pAdapterInfo == NULL)
    break;
   // Make an initial call to GetAdaptersInfo to get
   // the necessary size into the ulOutBufLen variable
   if (::GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
    ::free(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    if (!pAdapterInfo)
     break;
   }

   if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR)
    break;
   pAdapter = pAdapterInfo;
   while (pAdapter) {
    if (pAdapter->DhcpEnabled) {
     if (pAdapter->LeaseObtained > 10) {
      for (i = 0; i < __min(MAX_ADAPTER_ADDRESS_LENGTH, pAdapter->AddressLength); i++) {
       mac.append(1, pAdapter->Address[i]);
#if 0
       if (i == (pAdapter->AddressLength - 1))
        printf("%.2X\n", (int)pAdapter->Address[i]);
       else
        printf("%.2X-", (int)pAdapter->Address[i]);
#endif
      }
     }
    }
    pAdapter = pAdapter->Next;
   }
   result = !mac.empty();
  } while (0);

  if (pAdapterInfo) {
   ::free(pAdapterInfo);
   pAdapterInfo = nullptr;
  }
  mac = Win::BinToHex(mac);
  mac = IConv::ToUpperA(mac);
  if (lower) {
   mac = IConv::ToLowerA(mac);
  }
  return result;
 }

#if 0
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

 // Link with Iphlpapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* Note: could also use malloc() and free() */

 int __cdecl main(int argc, char** argv)
 {

  /* Declare and initialize variables */

  DWORD dwSize = 0;
  DWORD dwRetVal = 0;

  unsigned int i = 0;

  // Set the flags to pass to GetAdaptersAddresses
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

  // default to unspecified address family (both)
  ULONG family = AF_UNSPEC;

  LPVOID lpMsgBuf = NULL;

  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  ULONG outBufLen = 0;
  ULONG Iterations = 0;

  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
  PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
  PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
  PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
  IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
  IP_ADAPTER_PREFIX* pPrefix = NULL;

  if (argc != 2) {
   printf(" Usage: getadapteraddresses family\n");
   printf("        getadapteraddresses 4 (for IPv4)\n");
   printf("        getadapteraddresses 6 (for IPv6)\n");
   printf("        getadapteraddresses A (for both IPv4 and IPv6)\n");
   exit(1);
  }

  if (atoi(argv[1]) == 4)
   family = AF_INET;
  else if (atoi(argv[1]) == 6)
   family = AF_INET6;

  printf("Calling GetAdaptersAddresses function with family = ");
  if (family == AF_INET)
   printf("AF_INET\n");
  if (family == AF_INET6)
   printf("AF_INET6\n");
  if (family == AF_UNSPEC)
   printf("AF_UNSPEC\n\n");

  // Allocate a 15 KB buffer to start with.
  outBufLen = WORKING_BUFFER_SIZE;

  do {

   pAddresses = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
   if (pAddresses == NULL) {
    printf
    ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
    exit(1);
   }

   dwRetVal =
    GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

   if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
    FREE(pAddresses);
    pAddresses = NULL;
   }
   else {
    break;
   }

   Iterations++;

  } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

  if (dwRetVal == NO_ERROR) {
   // If successful, output some information from the data we received
   pCurrAddresses = pAddresses;
   while (pCurrAddresses) {
    printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n",
     pCurrAddresses->Length);
    printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex);
    printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

    pUnicast = pCurrAddresses->FirstUnicastAddress;
    if (pUnicast != NULL) {
     for (i = 0; pUnicast != NULL; i++)
      pUnicast = pUnicast->Next;
     printf("\tNumber of Unicast Addresses: %d\n", i);
    }
    else
     printf("\tNo Unicast Addresses\n");

    pAnycast = pCurrAddresses->FirstAnycastAddress;
    if (pAnycast) {
     for (i = 0; pAnycast != NULL; i++)
      pAnycast = pAnycast->Next;
     printf("\tNumber of Anycast Addresses: %d\n", i);
    }
    else
     printf("\tNo Anycast Addresses\n");

    pMulticast = pCurrAddresses->FirstMulticastAddress;
    if (pMulticast) {
     for (i = 0; pMulticast != NULL; i++)
      pMulticast = pMulticast->Next;
     printf("\tNumber of Multicast Addresses: %d\n", i);
    }
    else
     printf("\tNo Multicast Addresses\n");

    pDnServer = pCurrAddresses->FirstDnsServerAddress;
    if (pDnServer) {
     for (i = 0; pDnServer != NULL; i++)
      pDnServer = pDnServer->Next;
     printf("\tNumber of DNS Server Addresses: %d\n", i);
    }
    else
     printf("\tNo DNS Server Addresses\n");

    printf("\tDNS Suffix: %wS\n", pCurrAddresses->DnsSuffix);
    printf("\tDescription: %wS\n", pCurrAddresses->Description);
    printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);

    if (pCurrAddresses->PhysicalAddressLength != 0) {
     printf("\tPhysical address: ");
     for (i = 0; i < (int)pCurrAddresses->PhysicalAddressLength;
      i++) {
      if (i == (pCurrAddresses->PhysicalAddressLength - 1))
       printf("%.2X\n",
        (int)pCurrAddresses->PhysicalAddress[i]);
      else
       printf("%.2X-",
        (int)pCurrAddresses->PhysicalAddress[i]);
     }
    }
    printf("\tFlags: %ld\n", pCurrAddresses->Flags);
    printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
    printf("\tIfType: %ld\n", pCurrAddresses->IfType);
    printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
    printf("\tIpv6IfIndex (IPv6 interface): %u\n",
     pCurrAddresses->Ipv6IfIndex);
    printf("\tZoneIndices (hex): ");
    for (i = 0; i < 16; i++)
     printf("%lx ", pCurrAddresses->ZoneIndices[i]);
    printf("\n");

    printf("\tTransmit link speed: %I64u\n", pCurrAddresses->TransmitLinkSpeed);
    printf("\tReceive link speed: %I64u\n", pCurrAddresses->ReceiveLinkSpeed);

    pPrefix = pCurrAddresses->FirstPrefix;
    if (pPrefix) {
     for (i = 0; pPrefix != NULL; i++)
      pPrefix = pPrefix->Next;
     printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
    }
    else
     printf("\tNumber of IP Adapter Prefix entries: 0\n");

    printf("\n");

    pCurrAddresses = pCurrAddresses->Next;
   }
  }
  else {
   printf("Call to GetAdaptersAddresses failed with error: %d\n",
    dwRetVal);
   if (dwRetVal == ERROR_NO_DATA)
    printf("\tNo addresses were found for the requested parameters\n");
   else {

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
     FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
     NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
     // Default language
     (LPTSTR)&lpMsgBuf, 0, NULL)) {
     printf("\tError: %s", lpMsgBuf);
     LocalFree(lpMsgBuf);
     if (pAddresses)
      FREE(pAddresses);
     exit(1);
    }
   }
  }

  if (pAddresses) {
   FREE(pAddresses);
  }

  return 0;
 }
#endif
 std::string Win::Tcpip::GenerateTerminalIdentifybyMAC() {
  std::string result;
  std::string AdapterName;
  const int max_tries = 3;
  const int max_working_buffer_size = 15000;
  ULONG outBufLen = 0;
  DWORD dwSize = 0;
  DWORD dwRetVal = 0;
  ULONG Iterations = 0;
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
  do {
   bool alloc_addresses_result = true;
   outBufLen = max_working_buffer_size;
   do {
    pAddresses = (IP_ADAPTER_ADDRESSES*)SK_HEAP_ALLOC(outBufLen);
    if (pAddresses == NULL) {
     alloc_addresses_result = false;
     break;
    }
    dwRetVal = ::GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
     SK_HEAP_FREE(pAddresses);
    }
    else
     break;
    Iterations++;
   } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < max_tries));
   if (!alloc_addresses_result)
    break;
   if (dwRetVal != NO_ERROR)
    break;
   pCurrAddresses = pAddresses;
   while (pCurrAddresses) {
    if (pCurrAddresses->AdapterName)
     AdapterName.append(pCurrAddresses->AdapterName);

    if (pCurrAddresses->PhysicalAddressLength != 0)
     for (int i = 0; i < (int)pCurrAddresses->PhysicalAddressLength; i++)
      result.append(std::format("{:2X}", (int)pCurrAddresses->PhysicalAddress[i]));
    pCurrAddresses = pCurrAddresses->Next;
   }
  } while (0);
  SK_HEAP_FREE(pAddresses);

  if (result.empty())
   result = IConv::ToUpperA(AdapterName);

  for (auto it = result.begin(); it != result.end();) {
   if ((*it >= '0' && *it <= '9') || (*it >= 'A' && *it <= 'F')) {
    ++it;
    continue;
   }
   it = result.erase(it);
  }
  return result;
 }

 std::string Win::Tcpip::GetLocalMachineAllNetWorkCardMAC(std::set<std::string>& macs, const char& space /*= 0*/, const bool& lower /*= false*/) {
  std::string resultMacStr;
  IP_ADAPTER_INFO* oldptr = nullptr;
  IP_ADAPTER_INFO* pIPAdapterInfo = new IP_ADAPTER_INFO;
  ULONG nIPAdapterInfo = static_cast<ULONG>(sizeof(IP_ADAPTER_INFO));
  ULONG result = ::GetAdaptersInfo(pIPAdapterInfo, &nIPAdapterInfo);
  if (ERROR_BUFFER_OVERFLOW == result) {
   SK_DELETE_PTR(pIPAdapterInfo);
   pIPAdapterInfo = (PIP_ADAPTER_INFO)new std::uint8_t[nIPAdapterInfo];
   result = ::GetAdaptersInfo(pIPAdapterInfo, &nIPAdapterInfo);
  }
  oldptr = pIPAdapterInfo;
  if (ERROR_SUCCESS == result) {
   do {
    if (!pIPAdapterInfo)
     break;
    std::string mac;
    for (decltype(pIPAdapterInfo->AddressLength) i = 0; i < __min(MAX_ADAPTER_ADDRESS_LENGTH, pIPAdapterInfo->AddressLength); ++i) {
     char szTemp[5] = { 0 };
     if (lower)
      sprintf_s(szTemp, _countof(szTemp), "%02x", pIPAdapterInfo->Address[i]);
     else
      sprintf_s(szTemp, _countof(szTemp), "%02X", pIPAdapterInfo->Address[i]);
     mac.append(szTemp);
     if (space)
      mac.append(sizeof(char), space);
    }
    if (!mac.empty()) {
     if (space)
      mac.pop_back();
     macs.insert(mac);
    }
    pIPAdapterInfo = pIPAdapterInfo->Next;
   } while (1);
  }
  SK_DELETE_PTR(oldptr);
  for (const auto& mac : macs)
   resultMacStr.append(mac).append(",");
  if (!resultMacStr.empty())
   resultMacStr.pop_back();
  return resultMacStr;
 }
}///namespace shared

