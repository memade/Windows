#include "stdafx.h"

#define TEST_WIN7_CREATE_FILE 0
#define TEST_LIBCURLPP 0
#define TEST_PCHACKER 0
#define TEST_PELOAD 0
#define TEST_LIBUVPP 1

pchacker::IPCHacker* gpPCHackerObj = nullptr;

#if TEST_LIBUVPP


#define ENABLE_MMLOAD 0

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif

 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);
 tf_api_object_init api_object_init = nullptr;
 tf_api_object_uninit api_object_uninit = nullptr;
#if !ENABLE_MMLOAD
 HMODULE hPCHacker = nullptr;
 do {
#ifdef _DEBUG
  hPCHacker = ::LoadLibraryA(R"(D:\__Github__\Windows\bin\Win32\Debug\pchacker.dll)");
#else
  hPCHacker = ::LoadLibraryA(R"(D:\__Github__\Windows\bin\Win32\Release\pchacker.dll)");
#endif
  if (!hPCHacker)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hPCHacker, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hPCHacker, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;

  gpPCHackerObj = reinterpret_cast<decltype(gpPCHackerObj)>(api_object_init(nullptr, 0));
  if (!gpPCHackerObj)
   break;


  auto pConfig = gpPCHackerObj->ConfigureGet();
  pConfig->EnableLibuvppServer(true);
  gpPCHackerObj->Start(pConfig);
  auto sk = 0;
 } while (0);
#else
 void* hPELoad = nullptr;
 do {
  std::string pe_buffer;
#ifdef _DEBUG
  pe_buffer = shared::Win::File::Read(R"(D:\__Github__\Windows\bin\Win32\Debug\libuvpp.dll)");
  if (pe_buffer.empty())
   break;
#else
  pe_buffer = shared::Win::File::Read(R"(D:\__Github__\Windows\bin\Win32\Release\libuvpp.dll)");
  if (pe_buffer.empty())
   break;
#endif
  hPELoad = shared::Win::PE::MemoryLoadLibrary(pe_buffer.data(), pe_buffer.size());
  if (!hPELoad)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(shared::Win::PE::MemoryGetProcAddress(hPELoad, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(shared::Win::PE::MemoryGetProcAddress(hPELoad, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;
  libuvpp::ILibuv* pLibuv = reinterpret_cast<libuvpp::ILibuv*>(api_object_init(nullptr, 0));
  auto pServer = pLibuv->CreateServer();
  if (!pServer->Start(libuvpp::EnSocketType::TCP, libuvpp::EnIPV::IPV4, "127.0.0.1:13762"))
   break;


 } while (0);

#endif


 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q")
    exit = true;
   else if (input == "test") {


    auto  sk = 0;
   }



  });

 return 0;
}
#endif

#if TEST_PELOAD
#define MMLOAD 0
#define TESTREQUEST 1
static void __stdcall pchacker_taskstatus_cb(pchacker::ITaskNode* pTask) {
 do {
  if (!pTask)
   break;

  auto pResult = pTask->Result();

  const auto task_status = pTask->Status();
  switch (task_status) {
  case pchacker::EnActionType::DownBeworking: {
   std::cout << std::format("speed({}),total({}),{:.3f}%",
    shared::Win::RealtimeSpeed(static_cast<long long>(pResult->down_speed_s())),
    shared::Win::Time::TimePeriodUnMade(pResult->down_time_s()),
    pResult->down_percentage()) << std::endl;
  }break;
  case pchacker::EnActionType::DownRemove: {

   auto sk = 0;
  }break;
  case pchacker::EnActionType::DownFailed: {
   if (!pTask->FinalResult())
    std::cout << "Download final result failed." << std::endl;

   pTask->Action(pchacker::EnActionType::DownStop);
  }break;
  case pchacker::EnActionType::DownFinished: {

   pTask->Install();
   //pTask->Action(pchacker::EnActionType::InstallStart);
   //pTask->Action(pchacker::EnActionType::DownRemove);
  }break;

  case pchacker::EnActionType::InstallStart: {

   //pTask->Install();
   auto sk = 0;
   //pTask->Action(pchacker::EnActionType::InstallStart);
   //pTask->Action(pchacker::EnActionType::DownRemove);
  }break;
  case pchacker::EnActionType::InstallBeworking: {

   std::cout << std::format("install progress(total({}),current({}),progress({}))", pResult->extract_total(), pResult->extract_current(), pResult->extract_percentage()) << std::endl;
   auto sk = 0;
  }break;
  default:
   break;
  }


 } while (0);
}


int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif

 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);
 tf_api_object_init api_object_init = nullptr;
 tf_api_object_uninit api_object_uninit = nullptr;
 const std::string CurrentPath = shared::Win::GetModulePathA();
 void* hPELoad = nullptr;
#if MMLOAD
 do {
  std::string peLoadBuffer = shared::Win::File::Read((CurrentPath + "peload.dll").c_str());
  if (peLoadBuffer.empty())
   break;
  hPELoad = shared::Win::PE::MemoryLoadLibrary(peLoadBuffer.data(), peLoadBuffer.size());
  if (!hPELoad)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(shared::Win::PE::MemoryGetProcAddress(hPELoad, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(shared::Win::PE::MemoryGetProcAddress(hPELoad, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;
  std::string mircrosoft_runtime_pe = shared::Win::GetModulePathA() + "msvcr100.dll";
  gpPCHackerObj = reinterpret_cast<decltype(gpPCHackerObj)>(api_object_init(mircrosoft_runtime_pe.data(), static_cast<unsigned long>(mircrosoft_runtime_pe.size())));
  if (!gpPCHackerObj)
   break;
 } while (0);
#else
 do {
  HMODULE hPCHacker = ::LoadLibraryA((CurrentPath + "pchacker.dll").c_str());
  if (!hPCHacker)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hPCHacker, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hPCHacker, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;
  gpPCHackerObj = reinterpret_cast<decltype(gpPCHackerObj)>(api_object_init(nullptr, 0));
  if (!gpPCHackerObj)
   break;
 } while (0);
#endif

 auto pConfig = gpPCHackerObj->ConfigureGet();
 pConfig->EnableLibuvppServer(true);
 pConfig->EnableLibcurlpp(true);
 pConfig->ProjectCurrentPath(shared::Win::GetModulePathA());
 pConfig->DownPreparedResourcePath(shared::Win::GetModulePathA() + R"(\Games\prepareds\)");
 pConfig->DownResourceCachePath(shared::Win::GetModulePathA() + R"(\Games\caches\)");
 pConfig->FinishInstalledPath(shared::Win::GetModulePathA() + R"(\Games\finishs\)");

 gpPCHackerObj->Start(pConfig);
 gpPCHackerObj->RegisterTaskResultStatusCallback(pchacker_taskstatus_cb);

 static std::vector<int> taskids = { 4027 };
 for (const auto& id : taskids) {
  pchacker::ITaskNode* task = nullptr;
  switch (id) {
  case 4027: {
#if TESTREQUEST
   task = gpPCHackerObj->TaskCreate(id);
   if (!task)
    break;
   auto cache_pathname = gpPCHackerObj->ConfigureGet()->DownResourceCachePath() + std::to_string(id) + ".cache";
   task->Name(R"(反恐精英CS1.6)");
   task->DownCacheFilePathname(cache_pathname);
   task->FinishPath(gpPCHackerObj->ConfigureGet()->FinishInstalledPath() + std::to_string(id) + "\\");
   task->LogoUrl(R"(http://test.xitieba.com//admin/images/未标题-1.jpg)");
   task->Url(R"(http://92.204.144.86:8888/down/CkpYt4Lwlq9P)");
   //task->Url(R"(http://92.204.144.86:8888/down/fYkleOHxiuPV)");
#endif
  }break;
  default: {
  }break;
  };
  if (task)
   task->Perform();
 }

 shared::Win::MainProcess(
  [&](const auto& input, auto& exit) {
   if (input == "q") {
    gpPCHackerObj->Stop();
    if (api_object_uninit)
     api_object_uninit();
    SK_FREE_LIBRARY(hPELoad);
    exit = true;
    //shared::ISpdlog::DestoryInterface(pLogger);
   }
   else if (input == "test") {
    gpPCHackerObj->TaskAction(4027, pchacker::EnActionType::DownStop);
   }
   else {

   }


  });

 return 0;
}
#endif

#if TEST_WIN7_CREATE_FILE
int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif

 auto create_file_pathname = shared::Win::GetModulePathA() + "666.cache";
 shared::Win::File::WriteAddto(create_file_pathname, "66666");

 return 0;
}
#endif



#if TEST_LIBCURLPP
int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif
 HMODULE hPCHacker = nullptr;
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);
 tf_api_object_init api_object_init = nullptr;
 tf_api_object_uninit api_object_uninit = nullptr;
 const std::string CurrentPath = shared::Win::GetModulePathA();
 void* hPELoad = nullptr;
#if MMLOAD
 do {
  std::string peLoadBuffer = shared::Win::File::Read((CurrentPath + "peload.dll").c_str());
  if (peLoadBuffer.empty())
   break;
  hPELoad = shared::Win::PE::MemoryLoadLibrary(peLoadBuffer.data(), peLoadBuffer.size());
  if (!hPELoad)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(shared::Win::PE::MemoryGetProcAddress(hPELoad, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(shared::Win::PE::MemoryGetProcAddress(hPELoad, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;
  std::string mircrosoft_runtime_pe = shared::Win::GetModulePathA() + "msvcr100.dll";
  gpPCHackerObj = reinterpret_cast<decltype(gpPCHackerObj)>(api_object_init(mircrosoft_runtime_pe.data(), static_cast<unsigned long>(mircrosoft_runtime_pe.size())));
  if (!gpPCHackerObj)
   break;
 } while (0);
#else
 do {
  hPCHacker = ::LoadLibraryA((CurrentPath + "pchacker.dll").c_str());
  if (!hPCHacker)
   break;
  api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hPCHacker, "api_object_init"));
  api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hPCHacker, "api_object_uninit"));
  if (!api_object_init || !api_object_uninit)
   break;
  gpPCHackerObj = reinterpret_cast<decltype(gpPCHackerObj)>(api_object_init(nullptr, 0));
  if (!gpPCHackerObj)
   break;
 } while (0);
#endif

 do {
  if (!gpPCHackerObj)
   break;
  auto pConfig = gpPCHackerObj->ConfigureGet();
  pConfig->EnableLibcurlpp(true);
  pConfig->ProjectCurrentPath(shared::Win::GetModulePathA());
  gpPCHackerObj->Start(pConfig);

  auto pHttpObj = gpPCHackerObj->LibcurlppGet();
  pHttpObj->RegisterPerformNotifyCallback(
   [](const pchacker::libcurlpp::IResponse* resObj) {
    switch (resObj->Status()) {
    case pchacker::libcurlpp::EnRequestAction::Stopped: {

     auto sk = 0;
    }break;
    case pchacker::libcurlpp::EnRequestAction::Running: {
     std::cout << std::format("speed({}),total({}),{:.3f}%",
      shared::Win::RealtimeSpeed(static_cast<long long>(resObj->DownProgress()->speed_s())),
      shared::Win::Time::TimePeriodUnMade(resObj->DownProgress()->time_s()),
      resObj->DownProgress()->percentage()) << std::endl;
    }break;
    default:
     break;
    }

    auto sk = 0;
   });

  auto pReqObj = pHttpObj->CreateRequest(13252, pchacker::libcurlpp::EnPerformType::MultiAsync);
  pReqObj->RequestUrl(R"(http://92.204.144.86:8888/down/CkpYt4Lwlq9P)");
  pReqObj->Header(false);
  pReqObj->Progress(true);
  pReqObj->EnableWriteStream(false);
  pReqObj->CachePathname(R"(d:\66688.cache)");
  pReqObj->FinishCb(
   [&](const pchacker::libcurlpp::IResponse* resObj) {

    auto sss = 0;
   });


  pReqObj->Action(pchacker::libcurlpp::EnRequestAction::Start);
  auto sk = 0;
 } while (0);





 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
    gpPCHackerObj->LibcurlppGet()->Requests(
     [&](pchacker::libcurlpp::IRequest* reqObj) {
      reqObj->Action(pchacker::libcurlpp::EnRequestAction::Stop);
     });
    gpPCHackerObj->LibcurlppGet()->Stop();
    gpPCHackerObj->Stop();
    SK_FREE_LIBRARY(hPCHacker);
    exit = true;
   }
   else if (input == "exit") {
    gpPCHackerObj->LibcurlppGet()->Requests(
     [&](pchacker::libcurlpp::IRequest* reqObj) {
      reqObj->Action(pchacker::libcurlpp::EnRequestAction::Stop);
     });
    gpPCHackerObj->LibcurlppGet()->Stop();
    gpPCHackerObj->Stop();
    SK_FREE_LIBRARY(hPCHacker);
    exit = true;

   }
   else if (input == "start") {
    auto pReqObj = gpPCHackerObj->LibcurlppGet()->GetRequest(13252);
    pReqObj->Action(pchacker::libcurlpp::EnRequestAction::Start);
   }
   else if (input == "stop") {
    auto pReqObj = gpPCHackerObj->LibcurlppGet()->GetRequest(13252);
    pReqObj->Action(pchacker::libcurlpp::EnRequestAction::Stop);
   }
  });

 return 0;
}
#endif///TEST_LIBCURLPP

#if TEST_PCHACKER
static void __stdcall pchacker_taskstatus_cb(pchacker::ITaskNode* pTask) {
 do {
  if (!pTask)
   break;

  auto pResult = pTask->Result();

  const auto task_status = pTask->Status();
  switch (task_status) {
  case pchacker::EnActionType::DownBeworking: {
   std::cout << std::format("speed({}),total({}),{:.3f}%",
    shared::Win::RealtimeSpeed(static_cast<long long>(pResult->down_speed_s())),
    shared::Win::Time::TimePeriodUnMade(pResult->down_time_s()),
    pResult->down_percentage()) << std::endl;
  }break;
  case pchacker::EnActionType::DownRemove: {

   auto sk = 0;
  }break;
  case pchacker::EnActionType::DownFailed: {
   if (!pTask->FinalResult())
    std::cout << "Download final result failed." << std::endl;

   pTask->Action(pchacker::EnActionType::DownStop);
  }break;
  case pchacker::EnActionType::DownFinished: {

   //pTask->Action(pchacker::EnActionType::InstallStart);
   //pTask->Action(pchacker::EnActionType::DownRemove);
  }break;

  case pchacker::EnActionType::InstallStart: {

   pTask->Install();
   auto sk = 0;
   //pTask->Action(pchacker::EnActionType::InstallStart);
   //pTask->Action(pchacker::EnActionType::DownRemove);
  }break;
  case pchacker::EnActionType::InstallBeworking: {

   std::cout << std::format("install progress(total({}),current({}),progress({}))", pResult->extract_total(), pResult->extract_current(), pResult->extract_percentage()) << std::endl;
   auto sk = 0;
  }break;
  default:
   break;
  }


 } while (0);
}




int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif
 if (!gpPCHackerObj) {
  gpPCHackerObj = pchacker::IPCHacker::CreateInterface((shared::Win::GetModulePathA() + "pchacker.dll").c_str());
  auto pConfig = gpPCHackerObj->ConfigureGet();
  pConfig->DefaultDownloadCacheFileFormat(".666");
  pConfig->EnableLibuvppServer(false);
  pConfig->EnableLibcurlpp(true);
  gpPCHackerObj->Start(pConfig);
  gpPCHackerObj->RegisterTaskResultStatusCallback(pchacker_taskstatus_cb);
 }


 //https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_CS16_chs.rar?gamename=反恐精英CS1.6&sys=WinXP&filesize=163.84M&pic=http://img4.yxdimg.com/2018/1/26/e41d13ea-a79d-4fdf-9bc7-b64f293f04e5.jpg&gameid=4027&pcgametype=0
 //https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_CS16_chs.rar?反恐精英CS1.6中文版?WinXP?163.84M?pic=http://img4.yxdimg.com/2018/1/26/e41d13ea-a79d-4fdf-9bc7-b64f293f04e5.jpg&gameid=4027&pcgametype=0

 /*static std::vector<pchacker::ITaskNode*> pctasks;*/
 static std::vector<int> taskids = { 4027 };
 for (const auto& id : taskids) {
  pchacker::ITaskNode* task = nullptr;
  switch (id) {
  case 4027: {
   task = gpPCHackerObj->TaskCreate(id);
   if (!task)
    break;
   auto cache_pathname = shared::Win::GetModulePathA() + std::to_string(id) + ".cache";
   task->Name(R"(反恐精英CS1.6)");
   task->DownCacheFilePathname(cache_pathname);
   task->LogoUrl(R"(http://test.xitieba.com//admin/images/未标题-1.jpg)");
   task->Url(R"(http://92.204.144.86:8888/down/CkpYt4Lwlq9P)");
  }break;
  default: {
  }break;
  };
  if (task)
   task->Perform();
 }

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
     gpPCHackerObj->Stop();
     pchacker::IPCHacker::DestoryInterface(gpPCHackerObj);

     //shared::ISpdlog::DestoryInterface(pLogger);
     break;
    }
    else if (cmds[0] == "test") {
     gpPCHackerObj->TaskAction(4027, pchacker::EnActionType::DownStop);
  }
    else {
    }
} while (1);
  });
 maint.join();

 return 0;
}

#endif///TEST_PCHACKER
