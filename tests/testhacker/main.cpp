#include "stdafx.h"
#include <log.h>
int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif
 
 hacker::IHacker* pHacker = nullptr;
 do {
  std::string library_pathname;
#ifdef _WIN64
#if _DEBUG
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\hacker.dll)", "x64", "Debug");
#else
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\hacker.dll)", "x64", "Release");
#endif
#else
#if _DEBUG
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\hacker.dll)", "Win32", "Debug");
#else
  library_pathname = std::format(R"(D:\__Github__\Windows\bin\{}\{}\hacker.dll)", "Win32", "Release");
#endif
#endif
  pHacker = hacker::IHacker::CreateInterface("hacker.dll"/*library_pathname.c_str()*/);
 } while (0);

 if (!pHacker)
  return -1;

 pHacker->TerminalProcess(4688);

 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
    hacker::IHacker::DestoryInterface(pHacker);
    exit = true;
   }
   else if (input == "test") {
    pHacker->TerminalProcess(9564);
    auto  sk = 0;
   }



  });

 return 0;
}
