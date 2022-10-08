﻿#include "stdafx.h"

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif

 pchacker::IPCHacker* pPCHackerObj = \
  pchacker::IPCHacker::CreateInterface(\
   (shared::Win::GetModulePathA() + "pchacker.dll").c_str());

 libcurlpp::IHttpApi* pHttpObj = \
  reinterpret_cast<libcurlpp::IHttpApi*>(pPCHackerObj->GetLibcurlppHandle());


 auto pReqObj = pHttpObj->CreateRequest();
 pReqObj->RequestType(libcurlpp::EnRequestType::REQUEST_TYPE_GET);
 pReqObj->RequestUrl(R"(https://cn.bing.com/)");
 pReqObj->FinishCb(
  [&](const libcurlpp::IResponse* resObj) {
  
  
   auto sk = 0;
  });

 pHttpObj->PerformM({ pReqObj });



 pPCHackerObj->UICreate(pchacker::UIType::WXUI_MAIN, true);


 std::thread maint(
  [&]() {
   do {
    std::string _input;
    char c;
    while (std::cin >> std::noskipws >> c) { if ('\n' == c) break; _input.push_back(c); }
    std::cin >> std::skipws;
    shared::IConv::ToLowerA(_input);
    std::vector<std::string> cmds;
    shared::Win::File::ParseA(_input, ' ', cmds);
    if (cmds.empty())
     continue;
    if (cmds[0] == "q") {
     pchacker::IPCHacker::DestoryInterface(pPCHackerObj);
     break;
    }
    else if (cmds[0] == "test") {
    }
    else {
    }
   } while (1);
  });
 maint.join();

 return 0;
}