﻿#include "stdafx.h"

pchacker::IPCHacker* pPCHackerObj = nullptr;

static void __stdcall pchacker_taskstatus_cb(pchacker::ITaskNode* pTask) {
 do {
  if (!pTask)
   break;

  auto pResult = pTask->Result();

  const auto task_status = pTask->Status();
  switch (task_status) {
  case pchacker::EnActionType::DownAppend: {
   pTask->Preparation();
  }break;
  case pchacker::EnActionType::DownReady: {
   pTask->Perform();
   auto sk = 0;
  }break;
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

   pTask->Action(pchacker::EnActionType::DownRemove);
   auto sk = 0;
  }break;
  case pchacker::EnActionType::DownFinished: {
   pTask->Action(pchacker::EnActionType::InstallStart);
   //pTask->Action(pchacker::EnActionType::DownRemove);
  }break;

  case pchacker::EnActionType::InstallStart: {

   pTask->Install();
   auto sk = 0;
   //pTask->Action(pchacker::EnActionType::InstallStart);
   //pTask->Action(pchacker::EnActionType::DownRemove);
  }break;
  case pchacker::EnActionType::InstallBeworking: {

   std::cout << std::format("install progress(total({}),current({}),progress({}))",pResult->extract_total(),pResult->extract_current(),pResult->extract_percentage()) << std::endl;
   auto sk = 0;
  }break;
  default:
   break;
  }


  auto skx = 0;

#if 0
  auto pTaskNode = reinterpret_cast<pchacker::ITaskNode*>(result->TaskNodePtr());
  if (!pTaskNode)
   break;
  if (pTaskNode->Status() == pchacker::EnActionType::DownReady) {
   pTaskNode->Action(pchacker::EnActionType::DownStart);
   //pTaskNode->DownLimitSpeed(100 * 1024);
   //pTaskNode->DownLimitSpeed(1024 * 1024 * 5);
  }

  if (pTaskNode->Status() == pchacker::EnActionType::DownFinish) {
   pTaskNode->Action(pchacker::EnActionType::InstallAppend);
   break;
  }

  if (pTaskNode->Status() == pchacker::EnActionType::DownStop) {

   break;
  }

  if (pTaskNode->Status() == pchacker::EnActionType::DownRemove) {





   break;
  }

  std::cout << std::format("speed({}),total({}),{:.3f}%",
   shared::Win::RealtimeSpeed(static_cast<long long>(result->speed_s())),
   shared::Win::Time::TimePeriodUnMade(result->time_s()),
   result->percentage()) << std::endl;
#endif
  auto sk = 0;
 } while (0);
}



int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3869);
#endif

#if 0

 tfEnumFolderNode founds;
 shared::Win::FindFileAssignPath(R"(D:\__Github__\Windows\bin\Win32\Debug\pchacker\finishs\4027)", { "#YoUXun#" ,"gameyxdown.dat","双击安装游戏.bat"}, founds);

 std::string ini_cache = shared::Win::File::Read(R"(D:\__Github__\Windows\bin\Win32\Debug\pchacker\finishs\4027\CS16_chs_setup\CS16_chs\gameyxdown.dat\gameyxdown.dat)");

#if 0
 DWORD GetPrivateProfileStringA(
  LPCTSTR lpAppName,            // 同1(1)
  LPCTSTR lpKeyName,            // 同1(1)
  LPCTSTR lpDefault,            // 同1(1)
  LPTSTR lpReturnedString,      // 同1(1)
  DWORD nSize,                  // 同1(1)
  LPCTSTR lpFileName            // 读取信息的文件名。若该ini文件与程序在同一个目录下，也可使用相      
  //对路径,否则需要给出绝度路径。
 )
#endif



 auto skkk = 0;
 return 0;
#endif

#if 0
 std::string route_data_s;
 for (int i = 0; i < 5; ++i) {
  std::string temp;
  shared::Win::Packet::Made("asdlfkahglkadsfg@#$%#$%^2o3451i3y425i你好啊。嗨", temp);
  route_data_s.append(temp);
 }
 route_data_s.insert(0, "asdflkahgklasdgl@%@^@&&askldfgklhasdgk");
 route_data_s.append("asdflkahgklasdgl@%@^@&&askldfgklhasdgk");
 std::vector<std::string> unpacket_s;
 shared::Win::Packet::UnMade(route_data_s, unpacket_s);
 return 0;

#endif


 const std::vector<std::string> js_data_a{
 R"(eyJtb2R1bGUiOiJydW5HYW1lIiwidmlwIjoxLCJnYW1lSWQiOjMsImdhbWVOYW1lIjoi5oiY5ZywMTk0Mi5yYXIiLCJnYW1lVGltZSI6MTIwLCJ0eXBlIjowLCJpY28iOiJodHRwOi8vdGVzdC54aXRpZWJhLmNvbS8vYWRtaW4vaW1hZ2VzL+acquagh+mimC0xLmpwZyIsInVybCI6Imh0dHBzOi8vc3MuYnNjc3RvcmFnZS5jb20vZ29vZGdhbWUtbWlmYW5nOWt1L3l4ZG93bi5jb21fQkYxOTQyX2VuLnJhciIsImNtZCI6IiJ9)",
 R"(eyJtb2R1bGUiOiJydW5HYW1lIiwidmlwIjoxLCJnYW1lSWQiOjUsImdhbWVOYW1lIjoi6ay85q2m6ICF6YeN5Yi254mILnJhciIsImdhbWVUaW1lIjoxMjAsInR5cGUiOjAsImljbyI6Imh0dHA6Ly90ZXN0LnhpdGllYmEuY29tLy9hZG1pbi9pbWFnZXMvZjI2MzIzYjFiYWYzNmFlYTM2YmE3ZjIxNGRhNWM5OTIucG5nIiwidXJsIjoiaHR0cHM6Ly9zcy5ic2NzdG9yYWdlLmNvbS9nb29kZ2FtZS1taWZhbmc5a3UveXhkb3duLmNvbV9BTEkyMTMtU2FuZ28uSGVyb2VzLjcuSHVtYW4uYW5kLm9yYy5XYXIudjEuM01PRF9jaHMucmFyIiwiY21kIjoiIn0=)",
 R"(eyJtb2R1bGUiOiJydW5HYW1lIiwidmlwIjoxLCJnYW1lSWQiOjcsImdhbWVOYW1lIjoi5LiN5piv5Zyw5LiL5Z+O77yIRHVuZ2VvbiBObyBEdW5nZW9u77yJLnJhciIsImdhbWVUaW1lIjoxMjAsInR5cGUiOjAsImljbyI6Imh0dHA6Ly90ZXN0LnhpdGllYmEuY29tLy9hZG1pbi9pbWFnZXMvZ2l0aHViLmpwZyIsInVybCI6Imh0dHBzOi8vc3MuYnNjc3RvcmFnZS5jb20vZ29vZGdhbWUtbWlmYW5nOWt1L3l4ZG93bi5jb21fQUFKaXVHdW9TaGlHdW9DaGFvSmlKaWFRaWFuZ0Jhbl9jaHMucmFyIiwiY21kIjoiIn0=)",
 R"(eyJtb2R1bGUiOiJydW5HYW1lIiwidmlwIjoxLCJnYW1lSWQiOjEwLCJnYW1lTmFtZSI6Iue6ouiJsuitpuaIkjLlsKTph4znmoTlpI3ku4dleGUiLCJnYW1lVGltZSI6MTIwLCJ0eXBlIjowLCJpY28iOiJodHRwOi8vdGVzdC54aXRpZWJhLmNvbS8vYWRtaW4vaW1hZ2VzLzhlN2FiMjA4NWM2ZjMzNzNlMDJiYWUyOGNiNTRkYTExLnBuZyIsInVybCI6Imh0dHBzOi8vc3MuYnNjc3RvcmFnZS5jb20vZ29vZGdhbWUtbWlmYW5nOWt1L3l4ZG93bi5jb21fUkEyWXVyaXNSZXZlbmdlUmF6b3JfY2hzLnJhciIsImNtZCI6IiJ9)",
 R"(eyJtb2R1bGUiOiJydW5HYW1lIiwidmlwIjoxLCJnYW1lSWQiOjgsImdhbWVOYW1lIjoiQ1MxLjYuN3oiLCJnYW1lVGltZSI6MTIwLCJ0eXBlIjowLCJpY28iOiJodHRwOi8vdGVzdC54aXRpZWJhLmNvbS8vYWRtaW4vaW1hZ2VzL+acquWRveWQjV/lia/mnKwxLmpwZyIsInVybCI6Imh0dHBzOi8vc3MuYnNjc3RvcmFnZS5jb20vZ29vZGdhbWUtbWlmYW5nOWt1L0NTMS42Ljd6IiwiY21kIjoiIn0=)",
 R"(eyJtb2R1bGUiOiJydW5HYW1lIiwidmlwIjoxLCJnYW1lSWQiOjksImdhbWVOYW1lIjoi57qi6Imy6K2m5oiSMeOAgTLjgIEz6LaF5aSn5ZCI6ZuGcmFyIiwiZ2FtZVRpbWUiOjEyMCwidHlwZSI6MCwiaWNvIjoiaHR0cDovL3Rlc3QueGl0aWViYS5jb20vL2FkbWluL2ltYWdlcy9lZjNhMTBjNGVmN2EyZTJlZTMwYzNmNWY1YzQzYTBjZC5wbmciLCJ1cmwiOiJodHRwczovL3NzLmJzY3N0b3JhZ2UuY29tL2dvb2RnYW1lLW1pZmFuZzlrdS95eGRvd24uY29tX1JBMk1vZENvbGxlY3Rpb25fY2hzLnJhciIsImNtZCI6IiJ9)",
 };



 auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
 std::mt19937 rand_generator(static_cast<unsigned int>(seed));
 std::uniform_int_distribution<int> dist(6, 20);
 for (int cnt = 0; cnt < 10; ++cnt)
  std::cout << dist(rand_generator) << std::endl;

#if 0
 auto view = libiconv::IConv::gbk_to_ucs2le("你好666abc..!#@");
 uv::EventLoop* loop = uv::EventLoop::DefaultLoop();

 uv::TcpServer server(loop);
 server.setMessageCallback([](uv::TcpConnectionPtr ptr, const char* data, ssize_t size)
  {
   ptr->write(data, size, nullptr);
  });
 //server.setTimeout(60); //heartbeat timeout.
 uv::SocketAddr addr("127.0.0.1", 3762, uv::SocketAddr::Ipv4);
 server.bindAndListen(addr);
 loop->run();

 shared::ISpdlog::CreateInterface(
  shared::Win::GetModuleNameA(),
  shared::Win::GetModulePathA() + "\\logs\\");
 //pLogger->LOG(shared::ISpdlog::FORMAT("{}//{}//{}//{:.2f}//", "哈哈哈", 666, 999ul, 0.3456), shared::EnLogType::EN_LOG_TYPE_SYSTEM);
#endif


 //auto pLogger = shared::ISpdlog::CreateInterface(
 // shared::Win::GetModuleNameA(),
 // shared::Win::GetModulePathA() + "\\logs\\");
 //pLogger->LOG(shared::ISpdlog::FORMAT("{}//{}//{}//{:.2f}//", "哈哈哈", 666, 999ul, 0.3456), shared::EnLogType::EN_LOG_TYPE_SYSTEM);

 pPCHackerObj = \
  pchacker::IPCHacker::CreateInterface(\
   (shared::Win::GetModulePathA() + "pchacker.dll").c_str());

 pPCHackerObj->RegisterTaskResultStatusCallback(pchacker_taskstatus_cb);
#if 0
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
#endif


 /*static std::vector<pchacker::ITaskNode*> pctasks;*/
 static std::vector<int> taskids = { 3,5,7,8,9,10 };
 for (const auto& id : taskids) {
  pchacker::ITaskNode* task = nullptr;
  switch (id) {
#if 0
  case 3: {
   task = pPCHackerObj->TaskCreate(id);
   if (!task)
    break;
   task->LogoUrl(R"(http://test.xitieba.com//admin/images/未标题-1.jpg)");
   task->Url(R"(https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_BF1942_en.rar)");
   task->Name(R"(战地1942.rar)");

   auto sk = 0;
  }break;
  case 5: {
   task = pPCHackerObj->TaskCreate(id);
   if (!task)
    break;
   task->LogoUrl(R"(http://test.xitieba.com//admin/images/f26323b1baf36aea36ba7f214da5c992.png)");
   task->Url(R"(https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_ALI213-Sango.Heroes.7.Human.and.orc.War.v1.3MOD_chs.rar)");
   task->Name(R"(鬼武者重制版.rar)");

   auto sk = 0;
  }break;
#endif
#if 0
  case 7: {
   task = pPCHackerObj->TaskCreate(id);
   if (!task)
    break;
   task->LogoUrl(R"(http://test.xitieba.com//admin/images/github.jpg)");
   task->Url(R"(https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_AAJiuGuoShiGuoChaoJiJiaQiangBan_chs.rar)");
   task->Name(R"(不是地下城（Dungeon No Dungeon）.rar)");

   auto sk = 0;
  }break;
#endif
  case 0: {

  }break;
  default: {
   task = pPCHackerObj->TaskCreate(4027);
   if (!task)
    break;
   task->LogoUrl(R"(http://img4.yxdimg.com/2018/1/26/e41d13ea-a79d-4fdf-9bc7-b64f293f04e5.jpg)");
   task->Url(R"(https://ss.bscstorage.com/goodgame-mifang9ku/yxdown.com_CS16_chs.rar)");
   task->Name(R"(反恐精英CS1.6)");
  }break;
  };
  if (task) {
   /*task->RoutePtr(this);*/
   task->Action(pchacker::EnActionType::DownAppend);
  }
 }




 auto sk = 0;
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
