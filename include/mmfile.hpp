/*===- malware.hpp - Wrapper for 'extern "C"' ---------------*- C -*-=======*\
|*                                                                                                                        *|
|* Part of the Malware® Project, under the Apache License v2.0 with Malware® *|
|* Exceptions.                                                                                                      *|
|* See http://skstu.com/malware/LICENSE.txt for license information.                  *|
|* SPDX-License-Identifier: Apache-2.0 WITH MALWARE-exception                      *|
|*                                                                                                                        *|
|*===----------------------------------------------------------------------====== *|
|*                                                                                                                        *|
|* This file is a summary of the interface for this project.                                      *|
|*                                                                                                                        *|
\*===----------------------------------------------------------------------======*/
#if !defined(INC_H___0C762477_855F_488C_8D6A_65BDA5BAC3CF__HEAD__)
#define INC_H___0C762477_855F_488C_8D6A_65BDA5BAC3CF__HEAD__

/// Projects are compiled using static libraries
#define ENABLE_STATIC_COMPILATION 0



#if ENABLE_STATIC_COMPILATION
#ifdef SHARED_API
#undef SHARED_API
#endif
#ifdef _USRDLL
#undef _USRDLL
#endif
#else
#ifndef SHARED_API
#define SHARED_API
#endif
#ifndef _USRDLL
#define _USRDLL
#endif
#endif


namespace malware {


 namespace http {//!@ Short connection request interface
  using TypeIdentify = unsigned long long;
  using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
  using tf_api_object_uninit = void(__stdcall*)(void);

  class IResponse {
  public:

  protected:
   IResponse() {}
   ~IResponse() {}
  };

  using tf_response_cb = std::function<void(const IResponse*)>;

  class IRequest {
  public:
   virtual TypeIdentify Identify() const = 0;

   virtual void ResponseCb(const tf_response_cb&) = 0;
  protected:
   IRequest() {}
   ~IRequest() {}
  };

  class IHttpSpi {
  public:

  };

  class IHttpApi {
  protected:
   IHttpApi() {}
   ~IHttpApi() {}
  private:

  public:
   inline static IHttpApi* CreateInterface(const char* module_pathname);
   inline static void DestoryInterface(IHttpApi*);
  public:
   virtual IRequest* CreateRequest() = 0;
   virtual bool Perform(const IRequest*) = 0;
  protected:
   void* hModule = nullptr;
   tf_api_object_init api_object_init = nullptr;
   tf_api_object_uninit api_object_uninit = nullptr;
  };
  inline void IHttpApi::DestoryInterface(IHttpApi* instance) {
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
  inline IHttpApi* IHttpApi::CreateInterface(const char* module_pathname) {
   IHttpApi* result = nullptr;
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
    result = reinterpret_cast<decltype(result)>(api_object_init(nullptr, 0));
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
 }///namespace http




}///namespace malware

/// /*新生®（上海）**/
/// /*2022_09_07T01:08:08.0147270Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___0C762477_855F_488C_8D6A_65BDA5BAC3CF__HEAD__
