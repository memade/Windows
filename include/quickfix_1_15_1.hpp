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

 class IFixApi
 {
 public:
  virtual int Start() = 0;
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
 };

 class IFixSpi : public FIX::MessageCracker
 {
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
  virtual UINT_PTR ProtocolSetup() { return 0; }
 public:
  virtual void onCreate(const FIX::SessionID&) {}
  virtual void onCreate(const FIX::SessionID&, void**) {}
  virtual void onLogon(const FIX::SessionID&) {}
  virtual void onLogout(const FIX::SessionID&) {}
  virtual void fromAdmin(const FIX::Message&, const FIX::SessionID&) {}
  virtual void fromApp(const FIX::Message&, const FIX::SessionID&) {}
 private:
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
 __inline std::size_t IFixSpi::SendToAllContexts(const FIX::Message& message) const
 {
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
 __inline FIX::SEQNUM IFixSpi::ExpectedTargetNumGet() const
 {
  if (m_FixApi)
   return m_FixApi->ExpectedTargetNumGet();
  return 0;
 }
 __inline bool IFixSpi::Start()
 {
  if ((m_FixApi = reinterpret_cast<IFixApi*>(ProtocolSetup())) != 0)
   return 0 == m_FixApi->Start();
  return false;
 }
 __inline void IFixSpi::Stop()
 {
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