#include "stdafx.h"
using namespace local;

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 ::_CrtSetBreakAlloc(234);
#endif
 auto rfc1123 = shared::Win::Time::RFC1123();


 GlobalGet()->CoreGet()->Open();

 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {
   if (input == "q") {
    GlobalGet()->CoreGet()->Close();
    exit = true;
   }
   else if (input == "test") {

   }
  });
 return 0;
}
