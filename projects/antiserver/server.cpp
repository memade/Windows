#include "stdafx.h"

namespace local {

 Server::Server() {
  Init();
 }

 Server::~Server() {
  UnInit();
 }

 void Server::Init() {
  m_pServer = Global::LibuvGet()->CreateServer();
 }

 void Server::UnInit() {
  Global::LibuvGet()->DestoryServer(m_pServer);
 }

 bool Server::Start() {
  do {
   if (m_IsOpen.load())
    break;
   m_pServer->SessionCreateAfterCb(
    [this](pchacker::libuvpp::ISession* pSession) {
     pSession->Write(pchacker::TYPE_TASKMAN_MSG_WELCOME, "");
    });
   m_pServer->SessionDestoryBeforeCb(
    [&](pchacker::libuvpp::ISession* pSession) {
     //m_TaskmanPtrQ.pop(pSession->BindTaskId(),
     // [&](const auto&, auto& pTaskman) {
     //  pTaskman->Release();
     // });
    });
   m_pServer->MessageCb(
    [&](pchacker::libuvpp::ISession* pSession, const std::string& message) {
     auto pMsg = (shared::PPACKETHEAD)message.data();
     if (pMsg->Verify()) {
      auto pTaskMsg = reinterpret_cast<pchacker::PTASKMANMSG>(pMsg->data);
      switch (pMsg->command) {
      case pchacker::TYPE_TASKMAN_MSG_HELLO: {
       pSession->BindTaskId(pTaskMsg->TaskId);
       //m_TaskmanPtrQ.search(pTaskMsg->TaskId,
       // [&](Taskman* pTaskman) {
       //  pTaskMsg->ProcessId = ::GetCurrentProcessId();
       //  ::memcpy(pTaskMsg->InPathname, pTaskman->InPathname().data(), pTaskman->InPathname().size());
       //  ::memcpy(pTaskMsg->OutPath, pTaskman->OutPath().data(), pTaskman->OutPath().size());
       //  pSession->Write(pchacker::TYPE_TASKMAN_MSG_EXTRACT, std::string((char*)pTaskMsg, pchacker::LENTASKMANMSG));
       // });
      }break;
      case pchacker::TYPE_TASKMAN_MSG_EXTRACT_NOTIFY: {
       //m_TaskPool.search(pTaskMsg->TaskId,
       // [&](const auto&, pchacker::ITaskNode* pTask) {
       //  pchacker::ExtractProgressInfo progress;
       //  progress.extract_current = pTaskMsg->extract_current_extract_total_size;
       //  progress.extract_percentage = pTaskMsg->extract_progress;
       //  progress.extract_total = pTaskMsg->extract_total_size;

       //  *pTask << progress;

       //  if (pTask->Status() != EnActionType::InstallBeworking)
       //   pTask->Action(EnActionType::InstallBeworking);
       //  m_ResponseResultQ.push(pTask);
       // });

      }break;
      case pchacker::TYPE_TASKMAN_MSG_EXTRACT_SUCCESS: {

       //m_TaskPool.search(pTaskMsg->TaskId,
       // [&](const auto&, pchacker::ITaskNode* pTask) {
       //  *pTask << pTaskMsg->Details;
       //  pTask->Action(EnActionType::InstallFinish);
       //  m_ResponseResultQ.push(pTask);
       // });
       //shared::Win::Process::Terminate(pTaskMsg->ProcessId);
      }break;
      case pchacker::TYPE_TASKMAN_MSG_EXTRACT_FAILED: {
       //shared::Win::Process::Terminate(pTaskMsg->ProcessId);
       //m_TaskPool.search(pTaskMsg->TaskId,
       // [&](const auto&, pchacker::ITaskNode* pTask) {
       //  pTask->Action(EnActionType::InstallFailed);
       //  m_ResponseResultQ.push(pTask);
       // });
      }break;
      case pchacker::TYPE_CARDPACKAGE_MSG_HELLO: {
       auto sk = 0;
      }break;
      case pchacker::TYPE_CARDPACKAGE_MSG_EXIT: {

       //if (!m_OpenResourceCb)
       // break;
       std::vector<std::string> parses;
       shared::Win::File::ParseA(message, '@', parses);
       if (parses.size() < 2)
        break;
       unsigned long long id = atoll(parses[1].c_str());
       if (id <= 0)
        break;
       //m_OpenResourceCb(id);
      }break;
      case pchacker::TYPE_CARDPACKAGE_MSG_CHECK_FAILED: {
       auto sk = 0;
      }break;
      case pchacker::TYPE_CARDPACKAGE_MSG_CHECK_SUCCESS: {
       auto sk = 0;
      }break;

      default:
       break;
      }
     }

    });

   if (!m_pServer->Start(\
    pchacker::libuvpp::EnSocketType::TCP, \
    pchacker::libuvpp::EnIPV::IPV4, \
    Global::PCHackerGet()->ConfigureGet()->LocalServiceTcpAddr()))
    break;

   m_IsOpen.store(true);
  } while (0);
  return m_IsOpen.load();
 }

 void Server::Stop() {
  do {
   if (!m_IsOpen.load())
    break;

   m_pServer->Stop();

   m_IsOpen.store(false);
  } while (0);
 }



}///namespace local

