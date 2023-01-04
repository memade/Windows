#if !defined(INC_H___BDAB9E1B_8B18_4737_9262_CE217922F8BD__HEAD__)
#define INC_H___BDAB9E1B_8B18_4737_9262_CE217922F8BD__HEAD__

#include <dllinterface.h>

namespace uiframework {
 using TypeIdentify = unsigned long long;
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

 enum class EnUIChildType : unsigned long {
  Win32 = 0x1,
  WxWidgets = Win32 << 1,
  DuiLib = WxWidgets << 1,
  DearImGui = DuiLib << 1,
  Chromium = DearImGui << 1,
 };

 enum class EnUIType : unsigned long long {
  Win32SDKSDI = 0x10000000,//!@ Win32 SDK SDI
  Win32SDKMDI = 0x20000000,//!@ Win32 SDK MDI
  WxWidgets = 0x30000000,
  DuiLib = 0x40000000,
  DearImGui = 0x50000000,
  Chromium = 0x60000000,
 };

 class IWindowConfig {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual void WindowClassName(const std::wstring&) = 0;
  virtual const std::wstring& WindowClassName() const = 0;
  virtual void MainWindowTitleText(const std::wstring&) = 0;
  virtual const std::wstring& MainWindowTitleText() const = 0;
  virtual const HINSTANCE& Hinstance() const = 0;
  virtual void Hinstance(const HINSTANCE&) = 0;
  virtual void WindowDefaultSize(const SIZE&) = 0;
  virtual void WindowDefaultPosition(const POINT&) = 0;
  virtual void WindowDefaultBackgroundColor(const DWORD&, const double& Transparency = 1.0) = 0;
  virtual void WndProcPtr(void*) = 0;
  virtual void* WndProcPtr() const = 0;
  virtual void UIType(const EnUIType&) = 0;
  virtual const EnUIType& UIType() const = 0;
  virtual void EnableDpiAwareness(const bool&) = 0;
  virtual bool EnableDpiAwareness() const = 0;
  virtual void Visible(const bool&) = 0;
  virtual bool Visible() const = 0;
  virtual const HWND& Parent() const = 0;
  virtual void Parent(const HWND&) = 0;
  virtual void Release() const = 0;
 };

 class IWindowChildConfig {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual const HINSTANCE& Hinstance() const = 0;
  virtual void Hinstance(const HINSTANCE&) = 0;
  virtual void WindowClassName(const std::wstring&) = 0;
  virtual const std::wstring& WindowClassName() const = 0;
  virtual void WindowTitleText(const std::wstring&) = 0;
  virtual const std::wstring& WindowTitleText() const = 0;
  virtual void WindowDefaultSize(const SIZE&) = 0;
  virtual void WindowDefaultPosition(const POINT&) = 0;
  virtual void WindowDefaultBackgroundColor(const DWORD&, const double& Transparency = 1.0) = 0;
  virtual void UIType(const EnUIChildType&) = 0;
  virtual const EnUIChildType& UIType() const = 0;
  virtual void Visible(const bool&) = 0;
  virtual bool Visible() const = 0;
  virtual const HWND& ParentClientHwnd() const = 0;
  virtual void ParentClientHwnd(const HWND&) = 0;
  virtual const HWND& ParentHwnd() const = 0;
  virtual void ParentHwnd(const HWND&) = 0;
  virtual const DWORD& WindowStyle() const = 0;
  virtual void WindowStyle(const DWORD&) = 0;
  virtual void WndProcPtr(void*) = 0;
  virtual void* WndProcPtr() const = 0;
  virtual void Release() const = 0;
 };

 class IUIChild {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual void Show(const bool&) = 0;
  virtual bool Show() const = 0;
  virtual const EnUIChildType& UIType() const = 0;
  virtual const HWND& GetHwnd() const = 0;
  virtual bool Create() = 0;
  virtual void Destory() = 0;
  virtual void Release() const = 0;
  virtual IWindowChildConfig* Config() const = 0;
 };

 class IUIMain {
 public:
  virtual IWindowConfig* Config() const = 0;
  //Create main window.
  virtual void Create() = 0;
  //Destory main window and all childs.
  virtual void Destory() = 0;
  //Get main window handle.
  virtual const HWND& MainWnd() const = 0;
  virtual void Release() const = 0;

  virtual void Center() const = 0;
  //Window show control.
  virtual void Show(const bool&) = 0;
  virtual bool Show() const = 0;
  //Window child control
  virtual IWindowChildConfig* CreateChildConfig() = 0;
  virtual IUIChild* CreateChild(IWindowChildConfig*) = 0;
  virtual void MdiCascade() const = 0;
  virtual void MdiConarrange() const = 0;
  virtual void MdiTile() const = 0;
 };

 class IUIFrameWork : public shared::InterfaceDll<IUIFrameWork> {
 public:
  virtual IWindowConfig* CreateWindowConfig() = 0;
  virtual IUIMain* CreateUIMain(IWindowConfig*) = 0;
  virtual void DestoryUIMain(IUIMain*&) = 0;
  virtual void DestoryUIMain(const TypeIdentify&) = 0;
  virtual IUIMain* SearchUIMain(const TypeIdentify&) = 0;
 };

}///namespace uiframework



/// /*新生®（上海）**/
/// /*2022_11_02T10:28:38.2129169Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___BDAB9E1B_8B18_4737_9262_CE217922F8BD__HEAD__