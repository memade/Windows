#include "stdafx.h"

namespace local {

 Core::Core() {
  Init();
 }

 Core::~Core() {
  UnInit();
 }

 void Core::Init() {
  m_pUIMain = Global::UIFrameWorkGet()->CreateWxMain();
  m_pServer = new Server();
 }

 void Core::UnInit() {
  SK_DELETE_PTR(m_pServer);
 }

 bool Core::Open() {
  do {
   if (m_IsOpen.load())
    break;

   m_pUIMain->Create();
   m_pServer->Start();

   m_IsOpen.store(true);
  } while (0);
  return m_IsOpen.load();
 }

 void Core::Close() {
  do {
   if (!m_IsOpen.load())
    break;

   m_pServer->Stop();

   m_IsOpen.store(false);
  } while (0);
 }

}///namespace local