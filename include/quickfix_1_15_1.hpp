#if !defined(AFX_SKSTU_1609899219_H__E15B9_88CAAFEC_164720480A70857_1280__HEAD__)
#define AFX_SKSTU_1609899219_H__E15B9_88CAAFEC_164720480A70857_1280__HEAD__

#include <dllinterface.h>
//!@ Please specify project attach include file [....\quickfix1.15.1\src\]!
//! 
//! 
#include <quickfix1.15.1\src\config_windows.h>
#include <quickfix1.15.1\src\C++\SessionSettings.h>
#include <quickfix1.15.1\src\C++\Message.h>
#include <quickfix1.15.1\src\C++\Session.h>
//!@ FIX.4.4
#include <quickfix1.15.1\src\C++\fix44\MarketDataRequest.h>
#include <quickfix1.15.1\src\C++\fix44\MarketDataRequestReject.h>
#include <quickfix1.15.1\src\C++\fix44\NewOrderSingle.h>
#include <quickfix1.15.1\src\C++\fix44\OrderCancelRequest.h>
#include <quickfix1.15.1\src\C++\fix44\TradeCaptureReport.h>
#include <quickfix1.15.1\src\C++\fix44\ExecutionReport.h>
#include <quickfix1.15.1\src\C++\fix44\MarketDataSnapshotFullRefresh.h>
#include <quickfix1.15.1\src\C++\fix44\MarketDataIncrementalRefresh.h>
#include <quickfix1.15.1\src\C++\fix44\Logon.h>
#include <quickfix1.15.1\src\C++\fix44\Logout.h>
#include <quickfix1.15.1\src\C++\fix44\Reject.h>
#include <quickfix1.15.1\src\C++\fix44\ExecutionReport.h>
#include <quickfix1.15.1\src\C++\fix44\OrderStatusRequest.h>
#include <quickfix1.15.1\src\C++\fix44\BusinessMessageReject.h>
#include <quickfix1.15.1\src\C++\fix44\SourceRawData.h>
#include <quickfix1.15.1\src\C++\MessageCracker.h>
#include <quickfix1.15.1\src\C++\fix44\PositionReport.h>
#include <quickfix1.15.1\src\C++\fix44\RequestForPositions.h>
#include <quickfix1.15.1\src\C++\fix44\RequestForPositionsAck.h>
#include <quickfix1.15.1\src\C++\fix44\TradeCaptureReport.h>
#include <quickfix1.15.1\src\C++\fix44\TradeCaptureReportAck.h>
#include <quickfix1.15.1\src\C++\fix44\TradeCaptureReportRequest.h>
#include <quickfix1.15.1\src\C++\fix44\TradeCaptureReportRequestAck.h>

namespace quickfix_1_15_1 {

 class IProtocolSpi : public FIX::Application, public FIX::MessageCracker {
 public:
  virtual void onCreate(const FIX::SessionID&) override { return; }
  virtual void onCreate(const FIX::SessionID&, void**) override { return; }
  virtual void onLogon(const FIX::SessionID&) override { return; }
  virtual void onLogout(const FIX::SessionID&) override { return; }
  virtual void toAdmin(FIX::Message&, const FIX::SessionID&) override { return; }
  virtual void toApp(FIX::Message&, const FIX::SessionID&) override { return; }
  virtual void fromAdmin(const FIX::Message&, const FIX::SessionID&) override { return; }
  virtual void fromApp(const FIX::Message&, const FIX::SessionID&) override { return; }
  virtual void onMessage(const FIX44::SourceRawData&, const FIX::SessionID&) override { return; }
 protected:
  virtual inline bool Start(const std::string& protocol_module);
  virtual inline void Stop();
 protected:
  class IProtocol* m_Protocol = nullptr;
  std::atomic_bool m_IsOpen = false;
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 };

 class IConfig {
 public:
  virtual void Release() const = 0;
  virtual void ProtocolSpi(IProtocolSpi*) = 0;
  virtual const bool& EnableSSLProtocol() const = 0;
  virtual IProtocolSpi* ProtocolSpi() const = 0;
  //!@ FIX protocol configuration from the INI file.
  virtual bool operator<<(const std::string&) = 0;
 };

 class IProtocol : public shared::InterfaceDll<IProtocol> {
 public:
  virtual IConfig* ConfigGet() const = 0;
  virtual bool Start() = 0;
  virtual void Stop() = 0;
  virtual bool Ready() const = 0;
  virtual void Release() const = 0;
  virtual std::uint64_t Account() const = 0;
  virtual FIX::SEQNUM ExpectedTargetNumGet() const = 0;
  virtual FIX::IContext* FindContext(const FIX::SessionID&) const = 0;
  virtual FIX::IContext* FindContext(const unsigned __int64&) const = 0;
  virtual void IterateContext(const std::function<void(FIX::IContext*)>&) const = 0;
  virtual std::size_t SendToAllSessions(const FIX::Message&) const = 0;
  virtual std::size_t SendToAllContexts(const FIX::Message&) const = 0;
  virtual bool Send(const FIX::Message&) const = 0;
  virtual bool Send(const FIX::SessionID&, const FIX::Message&) const = 0;
 public:
#if 0
  virtual bridge::trade::IOrder* CreateBridgeOrder() const = 0;
  virtual bridge::IMarketData* CreateBridgeMarketData() const = 0;
  virtual bridge::IInstrument* CreateBridgeInstrument() const = 0;
  virtual bridge::IProduct* CreateBridgeLPProduct() const = 0;
  virtual bridge::IProduct* CreateBridgeSystemProduct() const = 0;
  virtual bridge::IProduct* CreateBridgeMTGatewayProduct() const = 0;
  virtual bridge::IProduct* CreateBridgeMTFeederProduct() const = 0;
#endif
 };

 inline bool IProtocolSpi::Start(const std::string& protocol_module) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (m_IsOpen.load())
    break;
   m_Protocol = IProtocol::CreateInterface(protocol_module);
   if (!m_Protocol)
    break;
   if (!m_Protocol->Start())
    break;
   m_IsOpen.store(true);
  } while (0);
  return m_IsOpen.load();
 }
 inline void IProtocolSpi::Stop() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_IsOpen.load())
    break;
   if (!m_Protocol)
    break;
   m_Protocol->Stop();
   IProtocol::DestoryInterface(m_Protocol);
   m_IsOpen.store(false);
  } while (0);
 }
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 class IFixApi {
  using api_protocol_init_initiator = void* (__stdcall*)(const void*, unsigned long, void*);
  using api_protocol_init_acceptor = void* (__stdcall*)(const void*, unsigned long, void*);
 public:
  virtual bool Start() = 0;
  virtual void Stop() = 0;
  virtual bool Ready() const = 0;
  virtual void Release() const = 0;
  virtual std::uint64_t Account() const { return 0; }
  virtual FIX::SEQNUM ExpectedTargetNumGet() const { return 0; }
  virtual const FIX::SessionSettings& SessionSettingsGet() const = 0;
  virtual FIX::IContext* FindContext(const FIX::SessionID&) const { return nullptr; }
  virtual FIX::IContext* FindContext(const unsigned __int64&) const { return nullptr; }
  virtual void IterateContext(const std::function<void(FIX::IContext*)>&) const {}
  virtual std::size_t SendToAllSessions(const FIX::Message&) const { return 0; }
  virtual std::size_t SendToAllContexts(const FIX::Message&) const { return 0; }
  virtual bool Send(const FIX::Message&) const { return false; }
  virtual bool Send(const FIX::SessionID&, const FIX::Message&) const { return false; }
 private:
  HMODULE m_hModule = nullptr;
  api_protocol_init_initiator m_api_protocol_init_initiator = nullptr;
  api_protocol_init_acceptor m_api_protocol_init_acceptor = nullptr;
 public:
  static IFixApi* CreateInterface(const std::string& module_pathname, const std::string& protocol_config, void* spi, const bool& acceptor = true) {
   IFixApi* result = nullptr;
   do {
    if (protocol_config.empty())
     break;
    auto hModule = ::LoadLibraryA(module_pathname.c_str());
    if (!hModule)
     break;
    auto p_api_protocol_init_initiator = \
     reinterpret_cast<api_protocol_init_initiator>(::GetProcAddress(hModule, "api_protocol_init_initiator"));
    if (!p_api_protocol_init_initiator)
     break;
    auto p_api_protocol_init_acceptor = \
     reinterpret_cast<api_protocol_init_acceptor>(::GetProcAddress(hModule, "api_protocol_init_acceptor"));
    if (!p_api_protocol_init_acceptor)
     break;
    acceptor ? \
     result = reinterpret_cast<IFixApi*>(p_api_protocol_init_acceptor(protocol_config.data(), static_cast<unsigned long>(protocol_config.size()), spi)) :
     result = reinterpret_cast<IFixApi*>(p_api_protocol_init_initiator(protocol_config.data(), static_cast<unsigned long>(protocol_config.size()), spi));

    if (!result)
     break;
    result->m_hModule = hModule;
    result->m_api_protocol_init_acceptor = p_api_protocol_init_acceptor;
    result->m_api_protocol_init_initiator = p_api_protocol_init_initiator;
   } while (0);
   return result;

  }
  static void DestoryInterface(IFixApi*& api) {
   do {
    if (!api)
     break;
    if (!api->m_hModule)
     break;
    api->Release();
    ::FreeLibrary(api->m_hModule);
    api->m_hModule = nullptr;
    api->m_api_protocol_init_acceptor = nullptr;
    api->m_api_protocol_init_initiator = nullptr;
   } while (0);
  }
 };

 class IFixSpi : public FIX::MessageCracker {
 protected:
  __inline std::uint64_t Account() const;
  __inline FIX::SEQNUM ExpectedTargetNumGet() const;
 public:
  __inline bool Start();
  __inline void Stop();
  __inline bool Ready() const;
  __inline bool Send(const FIX::Message&) const;
  __inline bool Send(const FIX::SessionID&, const FIX::Message&);
  __inline std::size_t SendToAllSessions(const FIX::Message&) const;
  __inline std::size_t SendToAllContexts(const FIX::Message&) const;
  __inline FIX::IContext* FindContext(const FIX::SessionID&) const;
  __inline FIX::IContext* FindContext(const unsigned __int64&) const;
  __inline void IterateContext(const std::function<void(FIX::IContext*)>&) const;
 protected:
  virtual bool ProtocolSetup() { return false; }
 public:
  virtual void onCreate(const FIX::SessionID&) {}
  virtual void onCreate(const FIX::SessionID&, void**) {}
  virtual void onLogon(const FIX::SessionID&) {}
  virtual void onLogout(const FIX::SessionID&) {}
  virtual void fromAdmin(const FIX::Message&, const FIX::SessionID&) {}
  virtual void fromApp(const FIX::Message&, const FIX::SessionID&) {}
 protected:
  IFixApi* m_FixApi = nullptr;
 };

 __inline bool IFixSpi::Send(const FIX::Message& message) const
 {
  if (m_FixApi)
   return m_FixApi->Send(message);
  return false;
 }
 __inline bool IFixSpi::Send(const FIX::SessionID& sessionID, const FIX::Message& message)
 {
  if (m_FixApi)
   return m_FixApi->Send(sessionID, message);
  return false;
 }
 __inline std::size_t IFixSpi::SendToAllSessions(const FIX::Message& message) const
 {
  if (m_FixApi)
   return m_FixApi->SendToAllSessions(message);
  return false;
 }
 __inline std::size_t IFixSpi::SendToAllContexts(const FIX::Message& message) const {
  if (m_FixApi)
   return m_FixApi->SendToAllContexts(message);
  return false;
 }
 __inline std::uint64_t IFixSpi::Account() const
 {
  if (m_FixApi)
   return m_FixApi->Account();
  return 0;
 }
 __inline FIX::SEQNUM IFixSpi::ExpectedTargetNumGet() const {
  if (m_FixApi)
   return m_FixApi->ExpectedTargetNumGet();
  return 0;
 }
 __inline bool IFixSpi::Start() {
  bool result = false;
  do {
   if (!ProtocolSetup())
    break;
   if (!m_FixApi)
    break;
   result = m_FixApi->Start();
  } while (0);
  return result;
 }
 __inline void IFixSpi::Stop() {
  if (m_FixApi)
  {
   m_FixApi->Stop();
   m_FixApi->Release();
   m_FixApi = nullptr;
  }
 }
 __inline bool IFixSpi::Ready() const
 {
  if (m_FixApi)
   return m_FixApi->Ready();
  return false;
 }
 __inline FIX::IContext* IFixSpi::FindContext(const FIX::SessionID& sessionID) const
 {
  if (m_FixApi)
   return m_FixApi->FindContext(sessionID);
  return nullptr;
 }
 __inline FIX::IContext* IFixSpi::FindContext(const unsigned __int64& contextID) const
 {
  if (m_FixApi)
   return m_FixApi->FindContext(contextID);
  return nullptr;
 }
 __inline void IFixSpi::IterateContext(const std::function<void(FIX::IContext*)>& iterate_cb) const
 {
  if (m_FixApi)
   m_FixApi->IterateContext(iterate_cb);
 }

}///namespace quickfix_1_15_1


#pragma comment(lib,"quickfix1.15.1.lib")
/// /*新生联创（上海）**/
/// /*2021年01月6日 10:13:42|001**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_1609899219_H__E15B9_88CAAFEC_164720480A70857_1280__TAIL__