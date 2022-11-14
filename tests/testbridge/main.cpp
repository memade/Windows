#include "stdafx.h"
#define ENABLE_TEST_CDR_PLUGIN 1

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif

#if ENABLE_TEST_CDR_PLUGIN
 do {
  auto hCdrPlugin = ::LoadLibraryA(R"(C:\Program Files\Corel\CorelDRAW Graphics Suite 2020\Draw\Plugins64\cdrplugin.cpg)");
  //auto hCdrPlugin = ::LoadLibraryA(R"(D:\__Github__\Windows\bin\x64\Release\cdrplugin.cpg)");
  if (!hCdrPlugin)
   break;



  auto sksks = 0;
 } while (0);
#endif

#if _DEBUG
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);
 tf_api_object_init api_object_init = nullptr;
 tf_api_object_uninit api_object_uninit = nullptr;
 HMODULE hLaunch = nullptr;
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
 shared::Win::Process::CreateA(R"(D:\__Github__\Windows\bin\bridge\x64\Release\launch.exe)", "", [](const auto&,const auto&) {}, true, true, 0);
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


