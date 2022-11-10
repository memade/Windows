#include "stdafx.h"

namespace local {
 static Global __gsGlobal;
 Global* GlobalGet() { return &__gsGlobal; }
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

 Global::Global() {
  Init();
 }

 Global::~Global() {
  UnInit();
 }

 void Global::Init() {
  do {
   if (m_Ready.load())
    break;

   do {///!@ Init pchacker module.
#if defined(_DEBUG)
    m_hPCHacker = ::LoadLibraryA(R"(D:\__Github__\Windows\bin\x64\Debug\pchacker.dll)");
#else
    m_hPCHacker = ::LoadLibraryA(shared::Win::GetModulePathA() + "pchacker.dll");
#endif
    if (!m_hPCHacker)
     break;
    tf_api_object_init api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(m_hPCHacker, "api_object_init"));
    if (!api_object_init)
     break;
    m_pPCHacker = reinterpret_cast<decltype(m_pPCHacker)>(api_object_init(nullptr, 0));
    if (!m_pPCHacker)
     break;
    auto pPCHackerConfig = m_pPCHacker->ConfigureGet();
    pPCHackerConfig->EnableLibuvppServer(false);
    pPCHackerConfig->LocalServiceTcpAddr("0.0.0.0:8100");
    if (!m_pPCHacker->Start(pPCHackerConfig))
     break;
    m_pLibuv = m_pPCHacker->LibuvppGet();
    if (!m_pLibuv)
     break;
   } while (0);

   do {///!@ Init uiframework module.
#if defined(_DEBUG)
    m_hUIFrameWork = ::LoadLibraryA(R"(D:\__Github__\Windows\bin\x64\Debug\uiframework.dll)");
#else
    m_hUIFrameWork = ::LoadLibraryA(shared::Win::GetModulePathA() + "uiframework.dll");
#endif
    if (!m_hUIFrameWork)
     break;
    tf_api_object_init api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(m_hUIFrameWork, "api_object_init"));
    if (!api_object_init)
     break;
    m_pUIFrameWork = reinterpret_cast<decltype(m_pUIFrameWork)>(api_object_init(nullptr, 0));
    if (!m_pUIFrameWork)
     break;
   } while (0);

   m_pCore = new Core();
   m_Ready.store(true);
  } while (0);
 }

 void Global::UnInit() {
  do {
   if (!m_Ready.load())
    break;

   //!@ Destory uiframework module.
   //! 
   //! 
   SK_FREE_LIBRARY(m_hUIFrameWork);
   if (m_hUIFrameWork) {
    tf_api_object_uninit uiframework_api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(m_hUIFrameWork, "api_object_uninit"));
    if (uiframework_api_object_uninit)
     uiframework_api_object_uninit();
   }

   //!@ Destory pchacker module.
   //! 
   //! 
   SK_DELETE_PTR(m_pCore);
   if (m_pPCHacker)
    m_pPCHacker->Stop();
   if (m_hPCHacker) {
    tf_api_object_uninit pchacker_api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(m_hPCHacker, "api_object_uninit"));
    if (pchacker_api_object_uninit)
     pchacker_api_object_uninit();
   }
   SK_FREE_LIBRARY(m_hPCHacker);

   m_Ready.store(false);
  } while (0);
 }

 Core* Global::CoreGet() {
  return __gsGlobal.m_pCore;
 }
 pchacker::IPCHacker* Global::PCHackerGet() {
  return __gsGlobal.m_pPCHacker;
 }
 pchacker::libuvpp::ILibuv* Global::LibuvGet() {
  return __gsGlobal.m_pLibuv;
 }
 uiframework::IUIFrameWork* Global::UIFrameWorkGet() {
  return __gsGlobal.m_pUIFrameWork;
 }
}///namespace local
