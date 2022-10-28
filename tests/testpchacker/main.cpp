#include "stdafx.h"


#define TEST_WIN7_CREATE_FILE 0
#define TEST_LIBCURLPP 0
#define TEST_PCHACKER 1

pchacker::IPCHacker* pPCHackerObj = nullptr;

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
 auto lib_pathname = shared::Win::GetModulePathA() + "libcurlpp.dll";
 libcurlpp::IHttpApi* pHttpObj = libcurlpp::IHttpApi::CreateInterface(lib_pathname.c_str());

 auto pReqTest = pHttpObj->CreateRequest();
 auto cache_pathname = shared::Win::GetModulePathA() + "4027.cache";
 auto finish_pathname = shared::Win::GetModulePathA() + "4027.finish";
 pReqTest->CachePathname(cache_pathname);
 pReqTest->RequestUrl(R"(http://92.204.144.86:8888/down/CkpYt4Lwlq9P)");
 pReqTest->Header(false);
 pReqTest->EnableWriteStream(false);
 pReqTest->RequestType(libcurlpp::EnRequestType::REQUEST_TYPE_GET);
 pReqTest->ProgressCb(
  [&](const libcurlpp::IProgressInfo* downProgressInfo, const libcurlpp::IProgressInfo*) {

   std::cout << std::format("speed({}),total({}),{:.3f}%",
    shared::Win::RealtimeSpeed(static_cast<long long>(downProgressInfo->speed_s())),
    shared::Win::Time::TimePeriodUnMade(downProgressInfo->time_s()),
    downProgressInfo->percentage()) << std::endl;
   return libcurlpp::EnProgressActionType::Continue;
  });
 pReqTest->FinishCb(
  [&](const libcurlpp::IResponse* resObj) {


   auto sk = 0;
  });

 pReqTest->Action(libcurlpp::EnRequestAction::Start);
 //pHttpObj->Perform(pReqTest);



 std::this_thread::sleep_for(std::chrono::milliseconds(1000000));
 libcurlpp::IHttpApi::DestoryInterface(pHttpObj);
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
 if (!pPCHackerObj) {
  pPCHackerObj = pchacker::IPCHacker::CreateInterface((shared::Win::GetModulePathA() + "pchacker.dll").c_str());
  auto pConfig = pPCHackerObj->ConfigureGet();
  pConfig->DefaultDownloadCacheFileFormat(".666");
  pConfig->EnableLibuvpp(false);
  pConfig->EnableLibcurlpp(true);
  pPCHackerObj->Start(pConfig);
  pPCHackerObj->RegisterTaskResultStatusCallback(pchacker_taskstatus_cb);
 }

 
 //https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_CS16_chs.rar?gamename=反恐精英CS1.6&sys=WinXP&filesize=163.84M&pic=http://img4.yxdimg.com/2018/1/26/e41d13ea-a79d-4fdf-9bc7-b64f293f04e5.jpg&gameid=4027&pcgametype=0
 //https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_CS16_chs.rar?反恐精英CS1.6中文版?WinXP?163.84M?pic=http://img4.yxdimg.com/2018/1/26/e41d13ea-a79d-4fdf-9bc7-b64f293f04e5.jpg&gameid=4027&pcgametype=0

 /*static std::vector<pchacker::ITaskNode*> pctasks;*/
 static std::vector<int> taskids = { 4027 };
 for (const auto& id : taskids) {
  pchacker::ITaskNode* task = nullptr;
  switch (id) {
  case 4027: {
   task = pPCHackerObj->TaskCreate(id);
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
     pPCHackerObj->Stop();
     pchacker::IPCHacker::DestoryInterface(pPCHackerObj);

     //shared::ISpdlog::DestoryInterface(pLogger);
     break;
    }
    else if (cmds[0] == "test") {
     pPCHackerObj->TaskAction(4027, pchacker::EnActionType::DownStop);
    }
    else {
    }
   } while (1);
  });
 maint.join();

 return 0;
}

#endif///TEST_PCHACKER
