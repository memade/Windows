/*===- libuvpp.hpp - Wrapper for 'extern "C"' ---------------*- C -*-=======*\
|*                                                                                                                       *|
|* Part of the Libuvpp® Project, under the Apache License v2.0 with Libuvpp®  *|
|* Exceptions.                                                                                                     *|
|* See http://skstu.com/libuvpp/LICENSE.txt for license information.                   *|
|* SPDX-License-Identifier: Apache-2.0 WITH LIBUVPP-exception                        *|
|*                                                                                                                        *|
|*===----------------------------------------------------------------------====== *|
|*                                                                                                                        *|
|* This file is a summary of the interface for this project.                                      *|
|*                                                                                                                        *|
\*===----------------------------------------------------------------------======*/
#if !defined(INC_H___19A77F54_4FFC_418D_BFEB_0CA3C94A278D__HEAD__)
#define INC_H___19A77F54_4FFC_418D_BFEB_0CA3C94A278D__HEAD__

namespace libuvpp {
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

 class ILibuv {
 public:









 protected:
  ILibuv() {}
  ~ILibuv() {}
 public:
  inline static ILibuv* CreateInterface(const char* module_pathname);
  inline static void DestoryInterface(ILibuv*);
 protected:
  void* hModule = nullptr;
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
 };

 inline void ILibuv::DestoryInterface(ILibuv* instance) {
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
 inline ILibuv* ILibuv::CreateInterface(const char* module_pathname) {
  ILibuv* result = nullptr;
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

}///namespace libuvpp

/// /*新生®（上海）**/
/// /*2022_10_09T00:53:53.1398001Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___19A77F54_4FFC_418D_BFEB_0CA3C94A278D__HEAD__

