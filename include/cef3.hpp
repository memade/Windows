/*===- cef3.hpp - Wrapper for 'extern "C"' ---------------*- C -*-=========*\
|*                                                                                                                       *|
|* Part of the cef3® Project, under the Apache License v2.0 with cef3®             *|
|* Exceptions.                                                                                                     *|
|* See http://skstu.com/malware/LICENSE.txt for license information.                *|
|* SPDX-License-Identifier: Apache-2.0 WITH CEF3-exception                      *|
|*                                                                                                                      *|
|*===----------------------------------------------------------------------====== *|
|*                                                                                                                      *|
|* This file is a summary of the interface for this project.                                    *|
|*                                                                                                                      *|
\*===----------------------------------------------------------------------======*/
#if !defined(INC_H___C14FA218_48BF_443F_BE3F_55B984FA9F98__HEAD__)
#define INC_H___C14FA218_48BF_443F_BE3F_55B984FA9F98__HEAD__

namespace shared {
 namespace cef3 {
  using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
  using tf_api_object_uninit = void(__stdcall*)(void);

  enum EnCreateType : unsigned long long {
   Undefined = 0,
   MainWindow = 1,
  };

  class IBrowserObj {
  public:
   virtual void Url(const std::string&) = 0;
   virtual void BackgroundColor(const DWORD&) = 0;
   virtual void Position(const int& x,const int& y,const int& cx,const int& cy) = 0;
   virtual void Parent(const HWND&) = 0;
   virtual const HWND& GetHwnd() const = 0;
  };

  class ICef3Api {
  protected:
   inline ICef3Api();
   inline ~ICef3Api();
  public:
   inline static ICef3Api* CreateInterface(const char* module_pathname, const void* param, unsigned long size_param);
   inline static void DestoryInterface(ICef3Api*&);
  public:
   virtual IBrowserObj* CreateBrowserObj() = 0;
   virtual bool Create(const HWND&) = 0;
  protected:
   void* hModule = nullptr;
   tf_api_object_init api_object_init = nullptr;
   tf_api_object_uninit api_object_uninit = nullptr;
  };

  //////////////////////////////////////////////////////////////////////////////////////////
  inline ICef3Api::ICef3Api() {}
  inline ICef3Api::~ICef3Api() {}
  inline void ICef3Api::DestoryInterface(ICef3Api*& instance) {
   do {
    if (!instance)
     break;
    if (!instance->hModule || !instance->api_object_uninit)
     break;
    HMODULE freeMod = reinterpret_cast<HMODULE>(instance->hModule);
    instance->api_object_uninit();
    instance = nullptr;
    ::FreeLibrary(freeMod);
    freeMod = nullptr;
   } while (0);
  }
  inline ICef3Api* ICef3Api::CreateInterface(const char* module_pathname, const void* param = nullptr, unsigned long size_param = 0) {
   ICef3Api* result = nullptr;
   HMODULE hModule = nullptr;
   do {
    if (!module_pathname)
     break;
    hModule = ::LoadLibraryA(module_pathname);
    if (!hModule)
     break;
    auto api_object_init = reinterpret_cast<tf_api_object_init>(::GetProcAddress(hModule, "api_object_init"));
    auto api_object_uninit = reinterpret_cast<tf_api_object_uninit>(::GetProcAddress(hModule, "api_object_uninit"));
    if (!api_object_init || !api_object_uninit)
     break;
    result = reinterpret_cast<decltype(result)>(api_object_init(param, size_param));
    if (!result)
     break;
    result->hModule = hModule;
    result->api_object_init = api_object_init;
    result->api_object_uninit = api_object_uninit;
   } while (0);
   if (nullptr == result && hModule != nullptr) {
    ::FreeLibrary(hModule);
    hModule = nullptr;
   }
   return result;
  }




 }///namespace cef3
}///namespace shared


/// /*新生®（上海）**/
/// /*2022_09_23T05:33:16.9900632Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___C14FA218_48BF_443F_BE3F_55B984FA9F98__HEAD__