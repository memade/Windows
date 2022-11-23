#include "stdafx.h"
#define ENABLE_TEST_CDR_PLUGIN 0
#define ENABLE_TEST_WIN32_MDI 0

#if !ENABLE_TEST_WIN32_MDI
int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif
 auto sss = shared::Win::UuidA();


 auto xxx = 0;
#if ENABLE_TEST_CDR_PLUGIN
 do {
  auto hCdrPlugin = ::LoadLibraryA(R"(C:\Program Files\Corel\CorelDRAW Graphics Suite 2020\Draw\Plugins64\cdrplugin.cpg)");
  //auto hCdrPlugin = ::LoadLibraryA(R"(D:\__Github__\Windows\bin\x64\Release\cdrplugin.cpg)");
  if (!hCdrPlugin)
   break;



  auto sksks = 0;
 } while (0);
#endif

 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);
 tf_api_object_init api_object_init = nullptr;
 tf_api_object_uninit api_object_uninit = nullptr;
 HMODULE hLaunch = nullptr;
#if _DEBUG
 do {
  std::string library_pathname;
#ifdef _WIN64
#if _DEBUG
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\bridge\{}\{}\launch.dll)", "x64", "Debug");
#else
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\bridge\{}\{}\launch.dll)", "x64", "Release");
#endif
#else
#if _DEBUG
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\bridge\{}\{}\launch.dll)", "Win32", "Debug");
#else
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\bridge\{}\{}\launch.dll)", "Win32", "Release");
#endif
#endif
  hLaunch = ::LoadLibraryA(library_pathname.c_str());
  if (!hLaunch)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hLaunch, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hLaunch, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;
  api_object_init(nullptr, 0);

 } while (0);
#else
 //do {
 // auto library_pathname = std::format(R"(D:\__Github__\Windows\bin\bridge\{}\{}\launch.dll)", "x64", "Release");
 // hLaunch = ::LoadLibraryA(library_pathname.c_str());
 // if (!hLaunch)
 //  break;
 // api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hLaunch, "api_object_init"));
 // api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hLaunch, "api_object_uninit"));
 // if (!api_object_init || !api_object_uninit)
 //  break;
 // auto pe = shared::Win::File::Read(R"(D:\__Github__\Windows\bin\x64\Release\uiframework.dll)");
 // tfRoutePak routePak;
 // shared::Win::Resource::MadeRoute({
 //  {0,"1234"},{1, pe} }, routePak);
 // api_object_init(routePak.data(), unsigned long(routePak.size()));
 //} while (0);

 shared::Win::Process::CreateA(R"(D:\__Github__\Windows\bin\bridge\x64\Release\launch.exe)", "", [](const auto&, const auto&) {}, true, true, 0);
#endif




 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
#if _DEBUG
    if (api_object_uninit)
     api_object_uninit();
    SK_FREE_LIBRARY(hLaunch);
#endif
    exit = true;
   }
   else if (input == "test") {


    auto  sk = 0;
   }



  });

 return 0;
}

#else
HWND g_hMDIClient = NULL;
HINSTANCE g_hInstance = NULL;
HWND g_hMainWindow = NULL;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
 switch (msg) {
 case WM_GETMINMAXINFO: {
  MINMAXINFO* minMax = (MINMAXINFO*)lParam;
  minMax->ptMinTrackSize.x = 220;
  minMax->ptMinTrackSize.y = 110;
  return 0;
 }break;
 case WM_SIZE: {
  RECT rcClient;
  GetClientRect(hWnd, &rcClient);
  SetWindowPos(g_hMDIClient, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
  return 0;
 }break;
 case WM_CREATE: {
  CLIENTCREATESTRUCT ccs = { 0 };
  ccs.idFirstChild = 50000;
  HWND hMDIClient = CreateWindowEx(0, TEXT("MDICLIENT"), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL |
   WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL,
   g_hInstance, (LPVOID)&ccs);
  g_hMDIClient = hMDIClient;
  MDICREATESTRUCT mcs;
  mcs.szTitle = L"Untitled";
  mcs.szClass = L"Win32 MDI Example Application - Child";
  mcs.hOwner = g_hInstance;
  mcs.x = mcs.cx = CW_USEDEFAULT;
  mcs.y = mcs.cy = CW_USEDEFAULT;
  mcs.style = MDIS_ALLCHILDSTYLES;
  HWND hWndChild = (HWND)::SendMessageW(hMDIClient, WM_MDICREATE, 0, (LPARAM)&mcs);
  return 0;
 }break;
 case WM_DESTROY: {
  PostQuitMessage(0);
  return 0;
 }break;
 default:
  break;
 }
 return ::DefFrameProcW(hWnd, g_hMDIClient, msg, wParam, lParam);
}

LRESULT CALLBACK MDIChildWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
 switch (msg) {
 case WM_MDIACTIVATE: {
  return 0;
 }break;
 case WM_CREATE: {
  return 0;
 }break;
 case WM_DESTROY: {
  return 0;
 }break;
 default:
  break;
 }
 return ::DefMDIChildProcW(hWnd, msg, wParam, lParam);
}

BOOL RegisterMainWindowClass() {
 WNDCLASSEXW wc = { 0 };
 wc.cbSize = sizeof(wc);
 wc.style = 0;
 wc.lpfnWndProc = &MainWndProc;
 wc.cbClsExtra = 0;
 wc.cbWndExtra = 0;
 wc.hInstance = g_hInstance;
 wc.hIcon = NULL/*(HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE |
  LR_DEFAULTCOLOR | LR_SHARED)*/;
 wc.hCursor = NULL/*(HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED)*/;
 wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
 wc.lpszMenuName = NULL/*MAKEINTRESOURCE(IDR_MAINMENU)*/;
 wc.lpszClassName = TEXT("Win32 MDI Example Application");
 wc.hIconSm = NULL/*(HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR)*/;
 return ::RegisterClassExW(&wc) ? TRUE : FALSE;
}
BOOL RegisterMDIChildWindowClass() {
 WNDCLASSEXW wc = { 0 };
 wc.cbSize = sizeof(wc);
 wc.style = 0;
 wc.lpfnWndProc = &MDIChildWndProc;
 wc.cbClsExtra = 0;
 wc.cbWndExtra = 0;
 wc.hInstance = g_hInstance;
 wc.hIcon = NULL/* (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE |
  LR_DEFAULTCOLOR | LR_SHARED)*/;
 wc.hCursor = NULL/*(HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED)*/;
 wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
 wc.lpszMenuName = NULL/*MAKEINTRESOURCE(IDR_MAINMENU)*/;
 wc.lpszClassName = TEXT("Win32 MDI Example Application - Child");/*MDIChildWndClass*/;
 wc.hIconSm = NULL/*(HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR)*/;
 return ::RegisterClassExW(&wc) ? TRUE : FALSE;
}

#if 0
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
 _In_opt_ HINSTANCE hPrevInstance,
 _In_ LPWSTR lpCmdLine,
 _In_ int nShowCmd) {
#else

#if 0
static bool Win32CreateWindowMDI(
 const HINSTANCE & hInstance,
 const WNDPROC & mainWndProcCb,
 const WNDPROC & childWndProcCb,
 const std::function<void(const HWND&,const HWND&)>& createWindowFinishCb = nullptr,
 const std::function<void(WNDCLASSEXW&)>&registerMainClassCb = nullptr,
 const std::function<void(WNDCLASSEXW&)>&registerChildClassCb = nullptr,
 const std::function<void(CREATESTRUCTW&)>&CreateWindowCb = nullptr,
 const std::function<void(int&)>&ShowWindowCb = nullptr
) {
 bool result = false;
 do {
  INITCOMMONCONTROLSEX icc = { 0 };
  icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icc.dwICC = ICC_WIN95_CLASSES;
  if (FALSE == InitCommonControlsEx(&icc))
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
  CREATESTRUCTW create_struct_w = { 0 };
  create_struct_w.hInstance = hInstance;
  create_struct_w.dwExStyle = WS_EX_CLIENTEDGE;
  create_struct_w.style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
  create_struct_w.x = CW_USEDEFAULT;
  create_struct_w.y = CW_USEDEFAULT;
  create_struct_w.cx = CW_USEDEFAULT;
  create_struct_w.cy = CW_USEDEFAULT;
  create_struct_w.hwndParent = NULL;
  create_struct_w.lpCreateParams = NULL;
  create_struct_w.lpszClass = LR"(Win32 MDI Example Application - Main)";
  create_struct_w.lpszName = LR"(Win32 MDI Example Application)";
  create_struct_w.hMenu = NULL;
  if (CreateWindowCb)
   CreateWindowCb(create_struct_w);
  HWND hWnd = ::CreateWindowExW(
   create_struct_w.dwExStyle,
   create_struct_w.lpszClass,
   create_struct_w.lpszName,
   create_struct_w.style,
   create_struct_w.x,
   create_struct_w.y,
   create_struct_w.cx,
   create_struct_w.cy,
   create_struct_w.hwndParent,
   create_struct_w.hMenu,
   create_struct_w.hInstance,
   create_struct_w.lpCreateParams);
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
  while (::GetMessageW(&msg, NULL, 0, 0) > 0) {
   ::TranslateMessage(&msg);
   ::DispatchMessageW(&msg);
  }
  result = true;
 } while (0);
 return result;
}
#endif
int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif
#endif
 
 shared::Win::Window::Win32MDICreateWindow(
  ::GetModuleHandleW(NULL),
  [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT {
   static HWND hwndClient = NULL;
   switch (message) {
   case WM_GETMINMAXINFO: {
    MINMAXINFO* minMax = (MINMAXINFO*)lParam;
    minMax->ptMinTrackSize.x = 220;
    minMax->ptMinTrackSize.y = 110;
    return 0;
   }break;
   case WM_SIZE: {
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    SetWindowPos(hwndClient, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
    return 0;
   }break;
   case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
   }break;
   case WM_CREATE: {
    LPCREATESTRUCTW pCreateStructW = reinterpret_cast<LPCREATESTRUCTW>(lParam);
    if (!pCreateStructW)
     break;
    auto pCreateRoute = reinterpret_cast<shared::Win::LPMDICreateRoute>(pCreateStructW->lpCreateParams);
    if (!pCreateRoute)
     break;
    hwndClient = ::CreateWindowExW(
     pCreateRoute->CreateStructWClient.dwExStyle,
     pCreateRoute->CreateStructWClient.lpszClass,
     pCreateRoute->CreateStructWClient.lpszName,
     pCreateRoute->CreateStructWClient.style,
     pCreateRoute->CreateStructWClient.x,
     pCreateRoute->CreateStructWClient.y,
     pCreateRoute->CreateStructWClient.cx,
     pCreateRoute->CreateStructWClient.cy,
     hwnd,
     pCreateRoute->CreateStructWClient.hMenu,
     pCreateRoute->CreateStructWClient.hInstance,
     (LPVOID)&pCreateRoute->ClientCreateStruct);
#if 1
    CLIENTCREATESTRUCT ccs = { 0 };
    ccs.idFirstChild = 50000;
    HWND hMDIClient = ::CreateWindowExW(0, TEXT("MDICLIENT"), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL |
     WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL,
     pCreateStructW->hInstance, (LPVOID)&ccs);
    //g_hMDIClient = hMDIClient;
    MDICREATESTRUCT mcs;
    mcs.szTitle = L"Untitled";
    mcs.szClass = L"Win32 MDI Example Application - Child";
    mcs.hOwner = pCreateStructW->hInstance;
    mcs.x = 0;
    mcs.cx = 100;
    mcs.y = 0;
    mcs.cy = 100;
    mcs.style = MDIS_ALLCHILDSTYLES;
    HWND hWndChild = (HWND)::SendMessageW(hMDIClient, WM_MDICREATE, 0, (LPARAM)&mcs);
#endif
    return 0;
   }break;
   default:
    break;
   }
   return ::DefFrameProcW(hwnd, hwndClient, message, wParam, lParam);
  },
  [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT {
   switch (message) {
   case WM_MDIACTIVATE: {
    //return 0;
   }break;
   case WM_CREATE: {
    return 1;
   }break;
   case WM_DESTROY: {
    return 0;
   }break;
   default:
    break;
   }
   return ::DefMDIChildProcW(hwnd, message, wParam, lParam);
  }, []() {}
 );

 return 0;
}


#endif
