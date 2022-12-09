#include "stdafx.h"
#include <D:\34896\asdg.h>

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif
 HMODULE hModule = nullptr;
 do {
  using tf_VerifierOpenLayerProperties = void(__stdcall*)(void);
  hModule = ::LoadLibraryW(LR"(D:\__Github__\Windows\bin\bridge\x64\Debug\vrfcore.dll)");
  if (!hModule)
   break;
  tf_VerifierOpenLayerProperties VerifierOpenLayerProperties = (tf_VerifierOpenLayerProperties)::GetProcAddress(hModule, "VerifierOpenLayerProperties");
  if (!VerifierOpenLayerProperties)
   break;
  VerifierOpenLayerProperties();
 } while (0);


 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
    SK_FREE_LIBRARY(hModule);
    exit = true;
   }
   else if (input == "test") {


    auto  sk = 0;
   }



  });

 return 0;
}
