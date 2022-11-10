#if !defined(INC_H___BDAB9E1B_8B18_4737_9262_CE217922F8BD__HEAD__)
#define INC_H___BDAB9E1B_8B18_4737_9262_CE217922F8BD__HEAD__


namespace uiframework {
 using TypeIdentify = unsigned long long;
 using tf_api_object_init = void* (__stdcall*)(const void*, unsigned long);
 using tf_api_object_uninit = void(__stdcall*)(void);

 class IWxMain {
 public:
  virtual const TypeIdentify& Identify() const = 0;
  virtual void Create() = 0;
  virtual void Destory() = 0;
 };

 class IUIFrameWork {
 public:
  virtual IWxMain* CreateWxMain() = 0;
  virtual void DestoryWxMain(IWxMain*&) = 0;
  virtual void DestoryWxMain(const TypeIdentify&) = 0;
 protected:
  void* hModule = nullptr;
  tf_api_object_init api_object_init = nullptr;
  tf_api_object_uninit api_object_uninit = nullptr;
 protected:
  inline IUIFrameWork();
  inline ~IUIFrameWork();
 public:
  inline static IUIFrameWork* CreateInterface(
   const char* pchacher_pe_pathname, const void* route, unsigned long nroute);
  inline static void DestoryInterface(IUIFrameWork*& pchacher_obj);
 };
 //////////////////////////////////////////////////////////////////////////////////////////
 inline IUIFrameWork::IUIFrameWork() {}
 inline IUIFrameWork::~IUIFrameWork() {}
 inline void IUIFrameWork::DestoryInterface(IUIFrameWork*& instance) {
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
 inline IUIFrameWork* IUIFrameWork::CreateInterface(const char* module_pathname, const void* param = nullptr, unsigned long size_param = 0) {
  IUIFrameWork* result = nullptr;
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

}///namespace uiframework



/// /*新生®（上海）**/
/// /*2022_11_02T10:28:38.2129169Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___BDAB9E1B_8B18_4737_9262_CE217922F8BD__HEAD__