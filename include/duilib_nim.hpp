#if !defined(__40F5D87C_0C1E_41A2_82E0_D4742FCD732C__)
#define __40F5D87C_0C1E_41A2_82E0_D4742FCD732C__

#include "base/base.h"
#include "duilib/UIlib.h"
#include "ui_components/ui_components.h"

namespace duilib_nim {

 static const unsigned int WM_NOTIFYICONDATA_MESSAGE = WM_USER + 10001;
 static const unsigned int WM_NOTIFYICONDATA_POPMENU_EXIT = WM_USER + 10002;

 class IApp : public nbase::FrameworkThread {
 protected:
  IApp() : nbase::FrameworkThread("MainApp") { 	}
  virtual ~IApp() { }
 protected:
  virtual void Init() = 0;
  virtual void Cleanup() = 0;
 };

 class IFrame : public ui::WindowImplBase {
 protected:
  IFrame(const int& logo_res_id = 0, const bool& is_tray = false)
   : m_IsSupportTray(is_tray)
   , m_LogoResID(logo_res_id) {

  }
  virtual ~IFrame() {

  }
 public:
  std::wstring GetWindowClassName() const override { return m_MainIdentify; }
 protected:
  virtual void Release() const = 0;
 protected:
  void OnFinalMessage(HWND hWnd) { Release(); }
  std::wstring GetSkinFolder() override { return m_MainIdentify; }
  std::wstring GetSkinFile() override { return m_MainIdentify + LR"(.xml)"; }
 protected:
  const int m_LogoResID;
  const bool m_IsSupportTray;
  const std::wstring m_MainIdentify = LR"(Main)";
  HMENU m_hTrayPopMenu = nullptr;
  NOTIFYICONDATAW m_NOTIFYICONDATA = { 0 };
  virtual void CreateTray(_Inout_ PNOTIFYICONDATAW pNotifyIconData) {
   pNotifyIconData->cbSize = sizeof(*pNotifyIconData);
   pNotifyIconData->hWnd = m_hWnd;
   pNotifyIconData->uID = m_LogoResID;
   pNotifyIconData->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
   pNotifyIconData->uCallbackMessage = WM_NOTIFYICONDATA_MESSAGE;
   pNotifyIconData->hIcon = ::LoadIconW(
    ::GetModuleHandleW(nullptr), MAKEINTRESOURCE(m_LogoResID));
   ::swprintf_s(pNotifyIconData->szTip, L"%s", m_MainIdentify.c_str());
   ::Shell_NotifyIconW(NIM_ADD, pNotifyIconData);
  }
  virtual void DestoryTray(_Inout_ PNOTIFYICONDATAW pNotifyIconData) {
   ::Shell_NotifyIconW(NIM_DELETE, pNotifyIconData);
  }
 protected:
  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override {
   switch (uMsg) {
   case WM_CREATE: {


    do {//!@ Tray
     if (!m_IsSupportTray)
      break;
     m_hTrayPopMenu = ::CreatePopupMenu();
     ::AppendMenuW(m_hTrayPopMenu, MF_STRING, WM_NOTIFYICONDATA_POPMENU_EXIT, TEXT("退出"));
     CreateTray(&m_NOTIFYICONDATA);
    } while (0);
   }break;
   case WM_DESTROY: {


    do {//!@ Tray
     if (!m_IsSupportTray)
      break;

     DestoryTray(&m_NOTIFYICONDATA);
     ::DestroyMenu(m_hTrayPopMenu);
    } while (0);
   }break;
   case WM_NOTIFYICONDATA_MESSAGE: {
    switch (lParam) {
    case WM_LBUTTONDBLCLK: {
     if (wParam != m_LogoResID)
      break;
     if (::IsWindowVisible(m_hWnd)) {
      ShowWindow(false);
     }
     else {
      ShowWindow(true);
      /*::SetForegroundWindow(m_hWnd);
      ::BringWindowToTop(m_hWnd);*/
      ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
     }
    }break;
    case WM_RBUTTONDOWN: {
     if (wParam != m_LogoResID)
      break;
     POINT pt = { 0 };
     ::GetCursorPos(&pt);
     ::TrackPopupMenu(m_hTrayPopMenu, TPM_RIGHTBUTTON, pt.x, pt.y, NULL, m_hWnd, NULL);
    }break;
    default:
     break;
    }
   }break;
   case WM_COMMAND: {
    switch (wParam) {
    case WM_NOTIFYICONDATA_POPMENU_EXIT: {
     ::PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
     return TRUE;
    }break;
    default:
     break;
    }
   }break;
   default:
    break;
   }
   return __super::HandleMessage(uMsg, wParam, lParam);
  }


 };



}///namespace duilib_nim




#if 0
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"winspool.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")
#pragma comment(lib,"uuid.lib")
#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"advapi32.lib")
#endif
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"Msimg32.lib")
#if _DEBUG
#pragma comment(lib,R"(base\Debug\base.lib)")
#pragma comment(lib,R"(duilib\Debug\duilib.lib)")
#pragma comment(lib,R"(ui_components\Debug\ui_components.lib)")
#else
#pragma comment(lib,R"(base\Release\base.lib)")
#pragma comment(lib,R"(duilib\Release\duilib.lib)")
#pragma comment(lib,R"(ui_components\Release\ui_components.lib)")
#endif

/// /*_ Memade®（新生™） _**/
/// /*_ Sat, 18 Feb 2023 09:44:02 GMT _**/
/// /*_____ https://www.skstu.com/ _____ **/
#endif///__40F5D87C_0C1E_41A2_82E0_D4742FCD732C__