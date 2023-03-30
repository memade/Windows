#include "stdafx.h"

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif

 physical::IPhysical* pPhysical = nullptr;
 do {
  std::string library_pathname;
#ifdef _WIN64
#if _DEBUG
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\physical.dll)", "x64", "Debug");
#else
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\physical.dll)", "x64", "Release");
#endif
#else
#if _DEBUG
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\physical.dll)", "Win32", "Debug");
#else
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\physical.dll)", "Win32", "Release");
#endif
#endif
  pPhysical = physical::IPhysical::CreateInterface(library_pathname.c_str());
 } while (0);

 if (!pPhysical)
  return -1;


 //auto pCore = pPhysical->CreateCore();
 //auto pCoreIdentify = pCore->IdentifyGet();
 //auto pCoreConfig = pCore->ConfigGet();

 /*for (int i = 0; i < 1000000; ++i) {
  auto pCore = pPhysical->CreateCore();


  auto sk = 0;
 }*/
 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
    physical::IPhysical::DestoryInterface(pPhysical);
    exit = true;
   }
   else if (input == "test") {


    auto  sk = 0;
   }



  });

 return 0;
}
