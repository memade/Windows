#include "stdafx.h"
#if !defined(WINLIB_DISABLE_WINDOWS)
#if !defined(WINLIB_DISABLE_GDIPLUS)
using namespace Gdiplus;
#endif

namespace shared {
 static Win::tagWindowConfig GlobalWindowConfig;
 extern Win::tagWindowConfig* GlobalWindowConfigGet() { return &GlobalWindowConfig; }

 Win::tagWindowConfig::tagWindowConfig() {
  ::memset(this, 0x00, sizeof(*this));
 }
 void Win::tagWindowConfig::operator=(const Win::tagWindowConfig& obj) {
  ::memcpy(this, &obj, sizeof(*this));
 }

 Win::tagMDICreateRoute::tagMDICreateRoute() {
  ::memset(this, 0x00, sizeof(*this));
  ClientCreateStruct.idFirstChild = 50000;
 }
 void Win::tagMDICreateRoute::operator=(const Win::tagMDICreateRoute& obj) {
  ::memcpy(this, &obj, sizeof(*this));
 }


 bool Win::Window::SetLogo(const HWND& hWnd, const HICON& hIcon) {
  bool result = false;
  do {
   if (!hWnd || !hIcon)
    break;
   ::SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
   ::SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
   result = true;
  } while (0);
  return result;
 }
 bool Win::Window::SetLogo2(const HWND& hWnd, const HICON& hIcon) {
  bool result = false;
  do {
   if (!hWnd || !hIcon)
    break;
   ::SetClassLongPtrW(hWnd, GCLP_HICONSM, (LONG_PTR)hIcon);
   result = true;
  } while (0);
  return result;
 }
#if !defined(WINLIB_DISABLE_GDIPLUS)
 ULONG_PTR Win::Window::GdiplusStartup() {
  ULONG_PTR gdiplusToken = 0;
  Gdiplus::GdiplusStartupInput gdiplusStartupInput = { 0 };
  do {
   if (Gdiplus::Status::Ok != Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL))
    break;
  } while (0);
  return gdiplusToken;
 }
 void Win::Window::GdiplusShutdown(_In_ const ULONG_PTR& gdiplusToken) {
  if (gdiplusToken <= 0)
   return;
  Gdiplus::GdiplusShutdown(gdiplusToken);
 }
#endif

#if 0//!@ WndProc
 [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT {
  PAINTSTRUCT ps;
  HDC hdc;
  wchar_t greeting[] = L"(Hello, Windows desktop!)";
  switch (message) {
  case WM_CREATE: {
   HICON hIcon = shared::Win::CreateIcon(\
    shared::Encryption::WemadeDecode(std::string((char*)&logo_ico_res[0], sizeof(logo_ico_res))));
   ::SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
   ::SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
  }break;
  case WM_PAINT: {
   hdc = BeginPaint(hWnd, &ps);
   // Here your application is laid out.
   // For this introduction, we just print out "Hello, Windows desktop!"
   // in the top left corner.
   ::TextOutW(hdc,
    5, 5,
    greeting, static_cast<int>(::wcslen(greeting)));
   // End application specific layout section.
   EndPaint(hWnd, &ps);
  }break;
  case WM_DESTROY: {
   PostQuitMessage(0);
  }break;
  default: {
   return ::DefWindowProcW(hWnd, message, wParam, lParam);
  }break;
  }
  return 0;
 });
#endif
 bool Win::Window::Win32CreateWindow(
  _In_ const HINSTANCE& hInstance,
  _In_ const WNDPROC& wndProc,
  _In_ const std::wstring& wndTitle,
  _In_ const std::wstring& wndClass /*= L""*/,
  _In_ const POINT& position /*= { 0 }*/,//! x,y
  _In_ const SIZE& size /*= { 0 }*/,//! cx,cy
  _In_ const DWORD& hbrBackground /*= RGB(255,255,255)*/,
  _In_ const bool& show /*= true*/,
  _In_ const HWND& hParent /*= NULL*/) {
  bool result = false;
  do {
   if (!hInstance || !wndProc)
    break;
   WNDCLASSEXW wcex;
   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = wndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = ::LoadIconW(wcex.hInstance, IDI_APPLICATION);
   wcex.hCursor = ::LoadCursorW(NULL, IDC_ARROW);
   wcex.hbrBackground = ::CreateSolidBrush(hbrBackground);
   /*wcex.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH);*/
   wcex.lpszMenuName = NULL;
   wcex.lpszClassName = wndClass.empty() ? Win::UuidW().c_str() : wndClass.c_str();
   wcex.hIconSm = ::LoadIconW(wcex.hInstance, IDI_APPLICATION);
   ATOM register_class = ::RegisterClassExW(&wcex);
   if (!register_class)
    break;
   DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
   DWORD dwStyle = WS_OVERLAPPEDWINDOW;
   HWND hWnd = ::CreateWindowExW(
    dwExStyle,
    wcex.lpszClassName,
    wndTitle.c_str(),
    dwStyle,
    position.x,
    position.y,
    size.cx,
    size.cy,
    hParent,
    NULL,
    hInstance,
    NULL
   );
   if (!hWnd)
    break;

   INITCOMMONCONTROLSEX InitCtrls;
   InitCtrls.dwSize = sizeof(InitCtrls);
   InitCtrls.dwICC = ICC_WIN95_CLASSES | ICC_LINK_CLASS;
   ::InitCommonControlsEx(&InitCtrls);

   //CreateSolidBrush(RGB(255,0,0));
   //HBRUSH brush = ::CreateSolidBrush(hbrBackground);
   //::SetClassLongW(hWnd, /*GCL_HBRBACKGROUND*/-10, reinterpret_cast<LONG>(brush));
   //::DeleteObject(brush);
   ::ShowWindow(hWnd, show ? SW_SHOWNORMAL : SW_HIDE);
   ::UpdateWindow(hWnd);
   MSG msg;
#if 1
   while (::GetMessageW(&msg, NULL, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);
   }
#else
   do {
    if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
     if (msg.message == WM_QUIT)
      break;
     ::TranslateMessage(&msg);
     ::DispatchMessageW(&msg);
    }
    else {
     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
   } while (1);
#endif
   result = true;
  } while (0);
  return result;
 }

 bool Win::Window::Win32MDICreateWindow(
  const HINSTANCE& hInstance,
  const WNDPROC& mainWndProcCb,
  const WNDPROC& childWndProcCb,
  const std::function<void()>& OnIdleCb,
  const std::function<void(const HWND&, const HWND&)>& createWindowFinishCb /*= nullptr*/,
  const std::function<void(WNDCLASSEXW&)>& registerMainClassCb /*= nullptr*/,
  const std::function<void(WNDCLASSEXW&)>& registerChildClassCb /*= nullptr*/,
  const std::function<void(tagMDICreateRoute&)>& createMainWindowCb /*= nullptr*/,
  const std::function<void(int&)>& ShowWindowCb /*= nullptr*/
 ) {
  bool result = false;
  do {
   INITCOMMONCONTROLSEX icc = { 0 };
   icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icc.dwICC = ICC_WIN95_CLASSES;
   if (FALSE == ::InitCommonControlsEx(&icc))
    break;
   WNDCLASSEXW wndClassMain = { 0 };
   wndClassMain.cbSize = sizeof(WNDCLASSEXW);
   wndClassMain.lpfnWndProc = mainWndProcCb;
   wndClassMain.hInstance = hInstance;
   wndClassMain.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wndClassMain.lpszClassName = LR"(Win32 MDI Example Application - Main)";

   WNDCLASSEXW wndClassChild = { 0 };
   wndClassChild.cbSize = sizeof(WNDCLASSEXW);
   wndClassChild.lpfnWndProc = childWndProcCb;
   wndClassChild.hInstance = hInstance;
   wndClassChild.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wndClassChild.lpszClassName = LR"(Win32 MDI Example Application - Child)";

   if (registerMainClassCb)
    registerMainClassCb(wndClassMain);
   if (registerChildClassCb)
    registerChildClassCb(wndClassChild);
   if (::RegisterClassExW(&wndClassMain) == 0 || ::RegisterClassExW(&wndClassChild) == 0)
    break;
   MDICreateRoute mdi_create_route;
   mdi_create_route.CreateStructW.hInstance = hInstance;
   mdi_create_route.CreateStructW.dwExStyle = WS_EX_CLIENTEDGE;
   mdi_create_route.CreateStructW.style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
   mdi_create_route.CreateStructW.x = CW_USEDEFAULT;
   mdi_create_route.CreateStructW.y = CW_USEDEFAULT;
   mdi_create_route.CreateStructW.cx = CW_USEDEFAULT;
   mdi_create_route.CreateStructW.cy = CW_USEDEFAULT;
   mdi_create_route.CreateStructW.hwndParent = NULL;
   mdi_create_route.CreateStructW.lpCreateParams = NULL;
   mdi_create_route.CreateStructW.lpszClass = wndClassMain.lpszClassName;
   mdi_create_route.CreateStructW.lpszName = LR"(Win32 MDI Example Application)";
   mdi_create_route.CreateStructW.hMenu = NULL;

   mdi_create_route.CreateStructWClient.hInstance = hInstance;
   mdi_create_route.CreateStructWClient.dwExStyle = 0;
   mdi_create_route.CreateStructWClient.lpszClass = L"MDICLIENT";
   mdi_create_route.CreateStructWClient.style = WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE;
   mdi_create_route.CreateStructWClient.x = CW_USEDEFAULT;
   mdi_create_route.CreateStructWClient.y = CW_USEDEFAULT;
   mdi_create_route.CreateStructWClient.cx = CW_USEDEFAULT;
   mdi_create_route.CreateStructWClient.cy = CW_USEDEFAULT;

   mdi_create_route.ClientCreateStruct.idFirstChild = 50000;

   if (createMainWindowCb)
    createMainWindowCb(mdi_create_route);
   HWND hWnd = ::CreateWindowExW(
    mdi_create_route.CreateStructW.dwExStyle,
    mdi_create_route.CreateStructW.lpszClass,
    mdi_create_route.CreateStructW.lpszName,
    mdi_create_route.CreateStructW.style,
    mdi_create_route.CreateStructW.x,
    mdi_create_route.CreateStructW.y,
    mdi_create_route.CreateStructW.cx,
    mdi_create_route.CreateStructW.cy,
    mdi_create_route.CreateStructW.hwndParent,
    mdi_create_route.CreateStructW.hMenu,
    mdi_create_route.CreateStructW.hInstance,
    &mdi_create_route);
   if (!hWnd)
    break;
   HWND hWndClient = ::GetWindow(hWnd, GW_CHILD);
   if (!hWndClient)
    break;
   int ShowWindowFlag = SW_SHOWNORMAL;
   if (ShowWindowCb)
    ShowWindowCb(ShowWindowFlag);
   ::ShowWindow(hWnd, ShowWindowFlag);
   ::UpdateWindow(hWnd);
   if (createWindowFinishCb)
    createWindowFinishCb(hWnd, hWndClient);
   MSG msg = { 0 };
#if 0
   while (::GetMessageW(&msg, NULL, 0, 0) > 0) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);
   }
#else
   do {
    while (::PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE)) {
     ::TranslateMessage(&msg);
     ::DispatchMessageW(&msg);
     if (msg.message == WM_QUIT) {
      //!@ OnQuit()
      break;
     }
    }

    //!@ OnIdle()
    if (OnIdleCb)
     OnIdleCb();




    std::this_thread::sleep_for(std::chrono::microseconds(10));
   } while (1);
#endif
   result = true;
  } while (0);
  return result;
 }
 HWND Win::Window::Win32MDICreateChildWindow(
  const HWND& hClient,
  const MDICREATESTRUCTW& mdiCreateStructW) {
  HWND result = NULL;
  do {
   if (mdiCreateStructW.szClass[0] == 0)
    break;
   if (!mdiCreateStructW.hOwner)
    break;
   result = (HWND)::SendMessageW(hClient, WM_MDICREATE, 0, (LPARAM)&mdiCreateStructW);
   if (!result)
    break;
  } while (0);
  return result;
 }
 HWND Win::Window::Win32CreateEdit(
  _In_ const HWND& parent,
  _In_ const std::wstring& edit_text,
  _In_ 	const DWORD& dwStyle /*= WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL*/,
  _In_ const int& x /*= 0*/,
  _In_ const int& y /*= 0*/,
  _In_ const int& width /*= 100*/,
  _In_ const int& height /*= 100*/) {
  HWND result = nullptr;
  result = ::CreateWindowExW(
   0,
   L"EDIT",   // predefined class 
   NULL,         // no window title 
   dwStyle,
   x,
   y,
   width,
   height, // set size in WM_SIZE message 
   parent, // parent window 
   (HMENU)0/*ID_EDITCHILD*/,   // edit control ID 
   (HINSTANCE)::GetWindowLongPtrW(parent, GWLP_HINSTANCE),
   NULL);        // pointer not needed 
  if (result)
   // Add text to the window. 
   ::SendMessageW(result, WM_SETTEXT, 0, (LPARAM)edit_text.c_str());
  return result;
 }
 HWND Win::Window::Win32CreateButton(
  _In_ const HWND& parent,
  _In_ const std::wstring& button_text,
  _In_ const DWORD& dwStyle /*= WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON*/,
  _In_ const int& x /*= 0*/,
  _In_ const int& y /*= 0*/,
  _In_ const int& width /*= 100*/,
  _In_ const int& height /*= 100*/) {

  return ::CreateWindowW(
   L"BUTTON",  // Predefined class; Unicode assumed 
   button_text.c_str(),      // Button text 
   dwStyle,  // Styles 
   x,         // x position 
   y,         // y position 
   width,        // Button width
   height,        // Button height
   parent,     // Parent window
   NULL,       // No menu.
   (HINSTANCE)::GetWindowLongPtrW(parent, GWLP_HINSTANCE),
   NULL);      // Pointer not needed.
 }

 bool Win::Window::Win32TextOut(
  _In_ const HDC& hdc,
  _In_ const std::wstring& text,
  _In_ const DWORD& rgb,
  _In_ const int& x /*= 0*/,
  _In_ const int& y /*= 0*/,
  _In_ const bool& transparent /*= true*/
 ) {
  bool result = false;
  do {
   if (!hdc)
    break;
   ::SetTextColor(hdc, rgb);
   if (transparent)
    ::SetBkMode(hdc, TRANSPARENT);
   // Here your application is laid out.
   // For this introduction, we just print out "Hello, Windows desktop!"
   // in the top left corner.
   if (FALSE == ::TextOutW(hdc, x, y, text.c_str(), static_cast<int>(text.size())))
    break;
   // End application specific layout section.
   result = true;
  } while (0);
  return result;
 }
 void Win::Window::Win32CenterWindow(
  _In_ const HWND& hwnd,
  _In_ const bool& calcTaskbarSize /*= true*/) {
  do {
   if (!hwnd)
    break;
   RECT rtWindow = { 0 };
   ::GetWindowRect(hwnd, &rtWindow);
   const int width = rtWindow.right - rtWindow.left;
   const int height = rtWindow.bottom - rtWindow.top;
   if (width <= 0 && height <= 0)
    break;

   SIZE sizeSystem = { 0 };
   sizeSystem.cx = ::GetSystemMetrics(SM_CXSCREEN);
   sizeSystem.cy = ::GetSystemMetrics(SM_CYSCREEN);

   ::SetWindowPos(hwnd, HWND_TOP, (sizeSystem.cx - width) / 2, (sizeSystem.cy - height) / 2, width, height, SWP_SHOWWINDOW);

   if (!calcTaskbarSize)
    break;
   APPBARDATA ad = { 0 };
   ad.cbSize = sizeof(APPBARDATA);
   ::SHAppBarMessage(ABM_GETTASKBARPOS, &ad);
   switch (ad.uEdge) {
   case ABE_LEFT: {
   }break;
   case ABE_TOP: {
   }break;
   case ABE_RIGHT: {
   }break;
   case ABE_BOTTOM: {
    int to_top = (sizeSystem.cy - height - (ad.rc.bottom - ad.rc.top)) / 2;
    ::SetWindowPos(hwnd, HWND_TOP, (sizeSystem.cx - width) / 2, to_top, width, height, SWP_SHOWWINDOW);
   }break;
   default:
    break;
   }

  } while (0);
 }
 bool Win::Window::Win32SizeWindow(_In_ const HWND& hwnd, _In_ const SIZE& size) {
  bool result = false;
  do {
   if (!hwnd)
    break;
   result = ::SetWindowPos(hwnd, HWND_TOP, 0, 0, size.cx, size.cy, SWP_SHOWWINDOW) == TRUE;
  } while (0);
  return result;
 }
 SIZE Win::Window::Win32GetSystemMetrics(_In_ const bool& calcTaskbarSize /*= true*/) {
  SIZE result = { 0 };
  result.cx = ::GetSystemMetrics(SM_CXSCREEN);
  result.cy = ::GetSystemMetrics(SM_CYSCREEN);
  do {
   if (!calcTaskbarSize)
    break;
   APPBARDATA ad = { 0 };
   ad.cbSize = sizeof(APPBARDATA);
   ::SHAppBarMessage(ABM_GETTASKBARPOS, &ad);
   switch (ad.uEdge) {
   case ABE_LEFT: {
    result.cx -= (ad.rc.right - ad.rc.left);
   }break;
   case ABE_TOP: {
    result.cy -= (ad.rc.top - ad.rc.bottom);
   }break;
   case ABE_RIGHT: {
    result.cx -= (ad.rc.right - ad.rc.left);
   }break;
   case ABE_BOTTOM: {
    result.cy -= (ad.rc.bottom - ad.rc.top);
   }break;
   default:
    break;
   }
  } while (0);
  return result;
 }

 bool Win::Window::Win32CreateFontIndirect(
  _In_ const HDC& hdc,
  _Out_ HGDIOBJ& PrevSelectObject,
  _In_ const LONG& lfHeight /*= 0*/,
  _In_ const LONG& lfWeight /*= 0*/,
  _In_ const std::wstring& font_name /*= LR"(Segoe UI)"*/,
  _In_ const bool& lfItalic /*= false*/) {
  bool result = false;
  PrevSelectObject = nullptr;
  do {
   if (!hdc)
    break;
   TEXTMETRICW lptm = { 0 };
   if (FALSE == ::GetTextMetricsW(hdc, &lptm))
    break;
   LOGFONTW logfont = { 0 };
   auto pSystemFontObj = ::GetStockObject(SYSTEM_FONT);
   ::GetObjectW(pSystemFontObj, sizeof(LOGFONTW), &logfont);
   ::DeleteObject(pSystemFontObj);
   pSystemFontObj = nullptr;
   logfont.lfItalic = lfItalic ? TRUE : FALSE;
   logfont.lfCharSet = lptm.tmCharSet;
   logfont.lfHeight = lfHeight == 0 ? lptm.tmHeight : lfHeight;
   logfont.lfWeight = lfWeight == 0 ? lptm.tmWeight : lfWeight;
   //logfont.lfPitchAndFamily = 2;
   ::swprintf_s(logfont.lfFaceName, L"%s", font_name.c_str());
   HFONT hFont = ::CreateFontIndirectW(&logfont);
   if (!hFont)
    break;
   PrevSelectObject = ::SelectObject(hdc, hFont);
   ::DeleteObject(hFont);
   hFont = nullptr;

   result = true;
  } while (0);
  return result;
 }
 bool Win::Window::Win32GdiCreateImage(
  _In_ const HDC& hdc,
  _In_ const std::string& imageBuffer,
  _In_ const std::function<void(const unsigned int& bmpWidth, const unsigned int& bmpHeight, int& x, int& y)>& create_cb
 ) {
  bool result = false;
#if !defined(WINLIB_DISABLE_GDIPLUS)
  IStream* pStream = nullptr;
  Gdiplus::Image* pImage = nullptr;
  do {
   if (!hdc || imageBuffer.empty() || !create_cb)
    break;
   pStream = ::SHCreateMemStream(\
    reinterpret_cast<const unsigned char*>(imageBuffer.data()), \
    static_cast<unsigned int>(imageBuffer.size()));
   if (!pStream)
    break;
   pImage = Gdiplus::Image::FromStream(pStream);
   if (!pImage)
    break;
   // 计算绘制位置
  /*	Gdiplus::Graphics graphics(hWnd);*/
   Gdiplus::Graphics graphics(hdc);
   int x = 0;
   int y = 0;
   create_cb(pImage->GetWidth(), pImage->GetHeight(), x, y);
   // 绘制图片
   Gdiplus::Status status = graphics.DrawImage(pImage, x, y, \
    static_cast<int>(pImage->GetWidth()), static_cast<int>(pImage->GetHeight()));
   result = status == Gdiplus::Status::Ok;
  } while (0);
  SK_DELETE_PTR(pImage);
  if (pStream) {
   pStream->Release();
   pStream = nullptr;
  }
#endif
  return result;
 }
 bool Win::Window::Win32CreateDIBitmap(
  _In_ const HDC& hdc,
  _In_ const std::string& bmpBuffer,
  _In_ const std::function<void(const unsigned int& bmpWidth, const unsigned int& bmpHeight, int& x, int& y)>& create_cb) {
  bool result = false;
  BITMAP bitmap = { 0 };
  HBITMAP		hBitmap = nullptr;
  HDC hdcMem = nullptr;
  BITMAPFILEHEADER* pbmfh = nullptr;

  do {
   if (!hdc || bmpBuffer.empty() || !create_cb)
    break;
   pbmfh = (BITMAPFILEHEADER*)&bmpBuffer[0];
   hBitmap = ::CreateDIBitmap(
    hdc,
    (BITMAPINFOHEADER*)(pbmfh + 1),
    CBM_INIT,
    (BYTE*)pbmfh + pbmfh->bfOffBits,
    (BITMAPINFO*)(pbmfh + 1),
    DIB_RGB_COLORS);
   if (!hBitmap)
    break;
   ::GetObjectW(hBitmap, sizeof(BITMAP), &bitmap);
   hdcMem = ::CreateCompatibleDC(hdc);
   if (!hdcMem)
    break;
   auto pervObj = ::SelectObject(hdcMem, hBitmap);
   int x{ 0 }, y{ 0 };
   create_cb(bitmap.bmWidth, bitmap.bmHeight, x, y);
   if (FALSE == ::BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY))
    break;
   result = true;
  } while (0);
  if (hdcMem) {
   ::DeleteDC(hdcMem);
   hdcMem = nullptr;
  }
  if (hBitmap) {
   DeleteObject(hBitmap);
   hBitmap = nullptr;
  }
  return result;
 }
 HWND Win::Window::Win32CreateHyperlink(
  _In_ const HWND& hParent,
  _In_ const HINSTANCE& hInst,
  _In_ const std::wstring& url,
  _In_ const std::wstring& text,
  _In_ const int& x /*= 0*/,
  _In_ const int& y /*= 0*/,
  _In_ const int& width /*= 100*/,
  _In_ const int& height /*= 100*/)
 {
  HWND result = nullptr;
  do {
   if (url.empty() || text.empty())
    break;
   //https://www.microsoft.com
   std::wstring href = std::vformat(LR"(<a href="{}">{}</a>)", std::make_wformat_args(url, text));
   result = ::CreateWindowExW(
    0,
    WC_LINK,
    href.c_str(),
    WS_VISIBLE | WS_CHILD | WS_TABSTOP,
    x, y, width, height,
    hParent,
    NULL,
    hInst,
    NULL);
  } while (0);
  return result;
 }
}///namespace shared

#endif