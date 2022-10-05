#include "stdafx.h"

namespace shared {
#if 0
 include"stdafx.h"
#define   INITGUID 
#include<iostream>
#include<Windows.h>
#include<GPEdit.h>
#include<Guiddef.h>
  using namespace std;
 //本地组策略是否禁用任务管理器
 LRESULT DisableTaskMgr(int mark)
 {
  ::CoInitialize(NULL);
  LRESULT status;
  LRESULT hr = S_OK;
  IGroupPolicyObject* pGPO = NULL;
  hr = CoCreateInstance(CLSID_GroupPolicyObject, NULL, CLSCTX_INPROC_SERVER, IID_IGroupPolicyObject, (LPVOID*)&pGPO);
  if (hr == S_OK)
  {
   cout << "GPO创建成功\n";
  }
  else
  {
   cout << "GPO创建失败\n";
   return E_FAIL;
  }
  DWORD dwSection = GPO_SECTION_USER;
  HKEY hGPOKey = 0;
  hr = pGPO->OpenLocalMachineGPO(GPO_OPEN_LOAD_REGISTRY);
  if (SUCCEEDED(hr))
  {
   cout << "打开本地机器成功\n";
  }
  else
  {
   cout << "打开本地失败\n";
   return E_FAIL;
  }
  hr = pGPO->GetRegistryKey(dwSection, &hGPOKey);
  if (SUCCEEDED(hr))
  {
   cout << "加载注册表成功\n";
  }
  else
  {
   cout << "加载注册表失败\n";
   return E_FAIL;
  }
  //禁用任务管理器
  HKEY hKey = NULL;
  if (mark == 1)
  {


   status = RegOpenKeyEx(hGPOKey, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0,
    KEY_WRITE, &hKey);
   if (status != ERROR_SUCCESS)
   {
    status = RegCreateKeyEx(hGPOKey, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0,
     NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (status == S_OK)
    {
     cout << "创建键值成功\n";
    }
    else
    {
     cout << "创建键值失败\n";
     return E_FAIL;
    }
   }
   DWORD lpData = 1;
   status = RegSetKeyValue(hKey, NULL, "DisableTaskMgr", REG_DWORD, (LPCVOID)&lpData, 4);
   status = RegCloseKey(hKey);


  }
  GUID Registerid = REGISTRY_EXTENSION_GUID;
  GUID ThisGuid = {
  0x0F6B957E,
  0x509E,
  0x11D1,
  {0xA7, 0xCC, 0x00, 0x00, 0xF8, 0x75, 0x71, 0xE3}
  };
  RegCloseKey(hGPOKey);

  status = pGPO->Save(FALSE, TRUE, &Registerid, &ThisGuid);
  pGPO->Release();
  ::CoUninitialize();
 }
 int main()
 {

  DisableTaskMgr(1);
 }
#endif

#if 0
#include <gpedit.h>

 DWORD val, val_size = sizeof(DWORD);
 HRESULT hr;
 IGroupPolicyObject* pLGPO;
 HKEY machine_key, dsrkey;
 // MSVC is finicky about these ones => redefine them
 const IID my_IID_IGroupPolicyObject =
 { 0xea502723, 0xa23d, 0x11d1, {0xa7, 0xd3, 0x0, 0x0, 0xf8, 0x75, 0x71, 0xe3} };
 const IID my_CLSID_GroupPolicyObject =
 { 0xea502722, 0xa23d, 0x11d1, {0xa7, 0xd3, 0x0, 0x0, 0xf8, 0x75, 0x71, 0xe3} };
 GUID ext_guid = REGISTRY_EXTENSION_GUID;
 // This next one can be any GUID you want
 GUID snap_guid = { 0x3d271cfc, 0x2bc6, 0x4ac2, {0xb6, 0x33, 0x3b, 0xdf, 0xf5, 0xbd, 0xab, 0x2a} };

 // Create an instance of the IGroupPolicyObject class
 CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
 CoCreateInstance(&my_CLSID_GroupPolicyObject, NULL, CLSCTX_INPROC_SERVER,
  &my_IID_IGroupPolicyObject, (LPVOID*)&pLGPO);

 // We need the machine LGPO (if C++, no need to go through the lpVtbl table)
 pLGPO->lpVtbl->OpenLocalMachineGPO(pLGPO, GPO_OPEN_LOAD_REGISTRY);
 pLGPO->lpVtbl->GetRegistryKey(pLGPO, GPO_SECTION_MACHINE, &machine_key);

 // The disable System Restore is a DWORD value of Policies\Microsoft\Windows\DeviceInstall\Settings
 RegCreateKeyEx(machine_key, "Software\\Policies\\Microsoft\\Windows\\DeviceInstall\\Settings",
  0, NULL, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, NULL, &dsrkey, NULL);

 // Create the value
 val = 1;
 RegSetKeyValue(dsrkey, NULL, "DisableSystemRestore", REG_DWORD, &val, sizeof(val));
 RegCloseKey(dsrkey);

 // Apply policy and free resources
 pLGPO->lpVtbl->Save(pLGPO, TRUE, TRUE, &ext_guid, &snap_guid);
 RegCloseKey(machine_key);
 pLGPO->lpVtbl->Release(pLGPO);
#endif

 bool Win::GroupPolicy::Read(const std::string&, std::string&) {
  bool result = false;
  do {




   result = true;
  } while (0);
  return result;
 }






}///namespace shared