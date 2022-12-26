#if !defined(AFX_SKSTU_1609899219_H__E15B9_88CAAFEC_164720480A70857_1280__HEAD__)
#define AFX_SKSTU_1609899219_H__E15B9_88CAAFEC_164720480A70857_1280__HEAD__

#include <dllinterface.h>
//!@ Please specify project attach include file [....\quickfix\src\]!
//! 
//! 
#include <quickfix\src\config_windows.h>
#include <quickfix\src\C++\SessionSettings.h>
#include <quickfix\src\C++\Message.h>
#include <quickfix\src\C++\Session.h>
//!@ FIX.4.4
#include <quickfix\src\C++\fix44\MarketDataRequest.h>
#include <quickfix\src\C++\fix44\MarketDataRequestReject.h>
#include <quickfix\src\C++\fix44\NewOrderSingle.h>
#include <quickfix\src\C++\fix44\OrderCancelRequest.h>
#include <quickfix\src\C++\fix44\TradeCaptureReport.h>
#include <quickfix\src\C++\fix44\ExecutionReport.h>
#include <quickfix\src\C++\fix44\MarketDataSnapshotFullRefresh.h>
#include <quickfix\src\C++\fix44\MarketDataIncrementalRefresh.h>
#include <quickfix\src\C++\fix44\Logon.h>
#include <quickfix\src\C++\fix44\Logout.h>
#include <quickfix\src\C++\fix44\Reject.h>
#include <quickfix\src\C++\fix44\ExecutionReport.h>
#include <quickfix\src\C++\fix44\OrderStatusRequest.h>
#include <quickfix\src\C++\fix44\BusinessMessageReject.h>
//#include <quickfix\src\C++\fix44\SourceRawData.h>
#include <quickfix\src\C++\MessageCracker.h>
#include <quickfix\src\C++\fix44\PositionReport.h>
#include <quickfix\src\C++\fix44\RequestForPositions.h>
#include <quickfix\src\C++\fix44\RequestForPositionsAck.h>
#include <quickfix\src\C++\fix44\TradeCaptureReport.h>
#include <quickfix\src\C++\fix44\TradeCaptureReportAck.h>
#include <quickfix\src\C++\fix44\TradeCaptureReportRequest.h>
#include <quickfix\src\C++\fix44\TradeCaptureReportRequestAck.h>

namespace quickfix {
 using TypeIdentify = unsigned long long;

 class IConfig {
 public:
  virtual void Release() const = 0;
  virtual const std::string& SessionSettingsCache() const = 0;
  virtual bool operator<<(const std::string&) = 0;
 };

 class IQuickfix : public shared::InterfaceDll<IQuickfix> {
 public:
  virtual IConfig* ConfigGet() const = 0;
  virtual bool Start(void*/*FIX::Application*/) = 0;
  virtual void Stop() = 0;
  virtual bool Ready() const = 0;
  virtual void Release() const = 0;

#if 0
  virtual std::uint64_t Account() const { return 0; }
  virtual FIX::SEQNUM ExpectedTargetNumGet() const { return 0; }
  virtual const FIX::SessionSettings& SessionSettingsGet() const = 0;
  virtual ISession* FindContext(const FIX::SessionID&) const { return nullptr; }
  virtual ISession* FindContext(const unsigned __int64&) const { return nullptr; }
  virtual void IterateContext(const std::function<void(ISession*)>&) const {}
  virtual std::size_t SendToAllSessions(const FIX::Message&) const { return 0; }
  virtual std::size_t SendToAllContexts(const FIX::Message&) const { return 0; }
  virtual bool Send(const FIX::Message&) const { return false; }
  virtual bool Send(const FIX::SessionID&, const FIX::Message&) const { return false; }
#endif
 protected:
  IQuickfix() {}
  ~IQuickfix() {}
 };


}///namespace quickfix

#pragma comment(lib,"quickfix.lib")
/// /*新生联创（上海）**/
/// /*2021年01月6日 10:13:42|001**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_1609899219_H__E15B9_88CAAFEC_164720480A70857_1280__TAIL__