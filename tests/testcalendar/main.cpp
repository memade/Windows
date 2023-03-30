#include "stdafx.h"

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif

 auto pUIFrame = uiframework::IUIFrameWork::CreateInterface(R"(D:\__Github__\Windows\bin\x64\Debug\uiframework.dll)");
 do {
  if (!pUIFrame)
   break;
  auto pConfig = pUIFrame->CreateWindowConfig();
  pConfig->UIType(uiframework::EnUIType::WxWidgets);
  auto pUIMain = pUIFrame->CreateUIMain(pConfig);

  auto ssss = 0;
 } while (0);


 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
#if _DEBUG

#endif
    uiframework::IUIFrameWork::DestoryInterface(pUIFrame);
    exit = true;
   }
   else if (input == "test") {


    auto  sk = 0;
   }

  });

 return 0;
}
