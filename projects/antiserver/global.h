#if !defined(INC_H___7456879D_D389_435E_9007_CECCE59BBD0F__HEAD__)
#define INC_H___7456879D_D389_435E_9007_CECCE59BBD0F__HEAD__

namespace local {

 class Global final {
 public:
  Global();
  ~Global();
 private:
  void Init();
  void UnInit();
 public:
  static Core* CoreGet();
  static pchacker::libuvpp::ILibuv* LibuvGet();
  static pchacker::IPCHacker* PCHackerGet();
  static uiframework::IUIFrameWork* UIFrameWorkGet();
 private:
  Core* m_pCore = nullptr;
  std::atomic_bool m_Ready = false;
  pchacker::libuvpp::ILibuv* m_pLibuv = nullptr;
  HMODULE m_hPCHacker = nullptr;
  HMODULE m_hUIFrameWork = nullptr;
  uiframework::IUIFrameWork* m_pUIFrameWork = nullptr;
  pchacker::IPCHacker* m_pPCHacker = nullptr;
 };

 extern Global* GlobalGet();
}///namespace local

/// /*新生®（上海）**/
/// /*2022_10_31T22:13:09.8219432Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___7456879D_D389_435E_9007_CECCE59BBD0F__HEAD__
