#include "stdafx.h"

namespace shared {
#if 0
	//!@ Com写注册表测试
	//! 
	//! 
	bool result = false;
	HRESULT hResult = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	IWbemLocator* pIWbemLocator = nullptr;
	IWbemClassObject* pObject = nullptr;
	IWbemClassObject* pInSignature = nullptr;
	IWbemServices* pIWbemServices = nullptr;
	IWbemClassObject* pOutParams = nullptr;
	do {
		if (FAILED(hResult))
			break;
		hResult = ::CoInitializeSecurity(
			NULL,
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
		);
		if (FAILED(hResult))
			break;
		hResult = ::CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void**)&pIWbemLocator);
		if (FAILED(hResult))
			break;
		pIWbemServices = nullptr;
		wchar_t strNetworkResource[] = L"root\\default";
		hResult = pIWbemLocator->ConnectServer(&strNetworkResource[0], NULL, NULL, 0, NULL, 0, 0, &pIWbemServices);
		if (FAILED(hResult))
			break;
		hResult = ::CoSetProxyBlanket(
			pIWbemServices,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
		);
		if (FAILED(hResult))
			break;
		pObject = nullptr;
		wchar_t strObjectPath[] = L"StdRegProv";
		hResult = pIWbemServices->GetObjectW(strObjectPath, 0, 0, &pObject, 0);
		if (FAILED(hResult))
			break;
		pInSignature = nullptr;
		wchar_t strObjectInSignaturePath[] = L"SetStringValue";
		hResult = pObject->GetMethod(strObjectInSignaturePath, 0, &pInSignature, 0);
		if (FAILED(hResult))
			break;

#if 0
		HKEY_CLASSES_ROOT(2147483648)
			HKEY_CURRENT_USER(2147483649)
			HKEY_LOCAL_MACHINE(2147483650)
			HKEY_USERS(2147483651)
			HKEY_CURRENT_CONFIG(2147483653)
#endif
			//注册表根目录
			_variant_t var1(L"2147483649");
		hResult = pInSignature->Put(L"hDefKey", 0, &var1, CIM_UINT32);
		if (FAILED(hResult))
			break;
		//设置路径
		_variant_t var2(LR"(SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\SKTime)");
		hResult = pInSignature->Put(L"sSubKeyName", 0, &var2, CIM_STRING);
		if (FAILED(hResult))
			break;


		//设置项名
		_variant_t var3(L"DllName");
		hResult = pInSignature->Put(L"sValueName", 0, &var3, CIM_STRING);
		if (FAILED(hResult))
			break;
		//设置值
		_variant_t var4(LR"(C:\Users\k34ub\ProgramData\SK\services\SKTime.dll)");
		hResult = pInSignature->Put(L"sValue", 0, &var4, CIM_STRING);
		if (FAILED(hResult))
			break;

		//设置项名
		_variant_t var5(L"Enabled");
		hResult = pInSignature->Put(L"sValueName", 0, &var5, CIM_STRING);
		if (FAILED(hResult))
			break;
		//设置值
		_variant_t var6(LR"(1)");
		hResult = pInSignature->Put(L"sValue", 0, &var6, CIM_UINT32);
		if (FAILED(hResult))
			break;

		//设置项名
		_variant_t var7(L"InputProvider");
		hResult = pInSignature->Put(L"sValueName", 0, &var7, CIM_STRING);
		if (FAILED(hResult))
			break;
		//设置值
		_variant_t var8(LR"(1)");
		hResult = pInSignature->Put(L"sValue", 0, &var8, CIM_UINT32);
		if (FAILED(hResult))
			break;

		pOutParams = NULL;
		hResult = pIWbemServices->ExecMethod(strObjectPath, strObjectInSignaturePath, 0, 0, pInSignature, &pOutParams, 0);
		if (FAILED(hResult))
			break;
		std::string readReg;
		shared::Win::Registry::Read(R"(HKEY_CURRENT_USER\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\SKTime\DllName)", readReg);

		::MessageBoxA(NULL, readReg.c_str(), NULL, MB_TOPMOST);
		result = true;
	} while (0);

	if (pOutParams) {
		pOutParams->Release();
		pOutParams = nullptr;
	}
	if (pInSignature) {
		pInSignature->Release();
		pInSignature = nullptr;
	}
	if (pObject) {
		pObject->Release();
		pObject = nullptr;
	}
	if (pIWbemServices) {
		pIWbemServices->Release();
		pIWbemServices = nullptr;
	}
	if (pIWbemLocator) {
		pIWbemLocator->Release();
		pIWbemLocator = nullptr;
	}
	::CoUninitialize();

#if !defined(_DEBUG)
	::MessageBoxW(
		NULL,
		std::format(L"({})({})\n ***本工具不得传阅复制\n***本工具包装仅提供给团队测试人员使用.\n", L"请注意!", result ? L"success" : L"failed").c_str(),
		L"恶意软件专杀工具®",
		MB_TOPMOST);
#endif
	return 0;
#endif
	static void ImpersonateCb(const std::function<void(const HKEY&)>& call, const DWORD& UserProcessId) {
		BOOL bImpersonateLoggedOnUser = FALSE;
		HANDLE hTokenUser = nullptr;
		HANDLE hProcess = nullptr;
		do {
			DWORD dwCurrentUserProcessId = \
				UserProcessId <= 0 ? Win::Process::GetCurrentUserProcessId() : UserProcessId;
			if (dwCurrentUserProcessId <= 0)
				break;
			//DWORD dwCurrentUserProcessId = ::WTSGetActiveConsoleSessionId();
			//if (dwCurrentUserProcessId <= 0)
			// break;
			//if (!::WTSQueryUserToken(dwCurrentUserProcessId, &hTokenUser))
			// break;
			hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCurrentUserProcessId);
			if (!hProcess)
				break;
			if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hTokenUser))
				break;
			bImpersonateLoggedOnUser = ::ImpersonateLoggedOnUser(hTokenUser);
			if (bImpersonateLoggedOnUser) {
				HKEY hKeyUser = nullptr;
				if (ERROR_SUCCESS == ::RegOpenCurrentUser(KEY_ALL_ACCESS, &hKeyUser)) {
					call(hKeyUser);
				}
				if (hKeyUser) {
					::RegCloseKey(hKeyUser);
					hKeyUser = nullptr;
				}
			}
		} while (0);
		SK_CLOSE_HANDLE(hProcess);
		if (bImpersonateLoggedOnUser == TRUE)
			::RevertToSelf();
	}
	bool Win::Registry::HKCU_ReadSafe(const std::string& keyPathname, const std::string& keyValue, std::string& OutVal, DWORD& OutDwType, ULONG& OutDataSize, const DWORD& UserProcessId) {
		bool result = false;
		ImpersonateCb(
			[&](const HKEY& hKey) {
				result = Registry::Read(keyPathname, keyValue, OutVal, OutDwType, OutDataSize, hKey);
			}, UserProcessId);
		return result;
	}
	bool Win::Registry::HKCU_ReadSafe(const std::string& keyPathname, const std::string& keyValue, std::string& OutVal, const DWORD& UserProcessId) {
		bool result = false;
		ImpersonateCb(
			[&](const HKEY& hKey) {
				DWORD dwReadType = 0;
				ULONG ulReadSize = 0;
				result = Registry::Read(keyPathname, keyValue, OutVal, dwReadType, ulReadSize, hKey);
			}, UserProcessId);
		return result;
	}
	bool Win::Registry::HKCU_WriteSafe(const std::string& keyPathname, const std::string& keyData, const DWORD& keyType, const DWORD& UserProcessId) {
		bool result = false;
		ImpersonateCb(
			[&](const HKEY& hKey) {
				result = Registry::Write(keyPathname, keyData, keyType, hKey);
			}, UserProcessId);
		return result;
	}
	bool Win::Registry::HKCU_DeleteSafe(const std::string& keyPathname, const std::string& keyValue /*= ""*/, const DWORD& UserProcessId) {
		bool result = false;
		ImpersonateCb(
			[&](const HKEY& hKey) {
				result = Registry::Delete(keyPathname, keyValue, hKey);
			}, UserProcessId);
		return result;
	}
	bool Win::Registry::HKCU_EmptySafe(const std::string& keyPathname, const DWORD& UserProcessId /*= 0*/) {
		bool result = false;
		ImpersonateCb(
			[&](const HKEY& hKey) {
				result = Registry::Empty(keyPathname, hKey);
			}, UserProcessId);
		return result;
	}
	void Win::Registry::HKCU_EnumValuesSafe(const std::string& keyPathname, std::map<std::string, std::string>& outVals, const DWORD& UserProcessId /*= 0*/) {
		ImpersonateCb(
			[&](const HKEY& hKey) {
				Registry::EnumValues(keyPathname, outVals, hKey);
			}, UserProcessId);
	}

	bool Win::Registry::Exists(const std::string& keyPathname, const HKEY& hKeyIn /*= nullptr*/) {
		bool result = false;
		HKEY keyRoot = hKeyIn;
		do {
			std::string keyPath, keyName;
			if (!keyRoot)
				keyRoot = PathParse(keyPathname, keyPath);
			else
				keyPath = PathParse(keyPathname);
			if (!keyRoot)
				break;
			CRegKey regKeyObj;
			do {
				result = ERROR_SUCCESS == regKeyObj.Open(keyRoot, shared::IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS);
				if (result)
					break;
				result = ERROR_SUCCESS == regKeyObj.Open(keyRoot, shared::IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS | KEY_WOW64_32KEY);
				if (result)
					break;
				result = ERROR_SUCCESS == regKeyObj.Open(keyRoot, shared::IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS | KEY_WOW64_64KEY);
				if (result)
					break;
			} while (0);
			if (result)
				regKeyObj.Close();
		} while (0);
		if (keyRoot && !hKeyIn) {
			::RegCloseKey(keyRoot);
			keyRoot = nullptr;
		}
		return result;
	}
	bool Win::Registry::Empty(const std::string& keyPathname, const HKEY& hKeyIn /*= nullptr*/) {
		bool result = false;
		do {
			if (keyPathname.empty())
				break;
			std::map<std::string, std::string> values;
			Win::Registry::EnumValues(keyPathname, values);
			if (values.empty())
				break;
			for (const auto& node : values) {
				Win::Registry::Delete(keyPathname, node.first, hKeyIn);
			}
			result = true;
		} while (0);
		return result;
	}
	bool Win::Registry::DeleteValue(const std::string& keyPathOrPathname) {
		bool result = false;
		HKEY hKeyRoot = nullptr;
		CRegKey regKeyObj;
		LSTATUS retResult = ERROR_INVALID_FUNCTION;
		do {
			if (keyPathOrPathname.empty())
				break;
			if (shared::Win::IsPathA(keyPathOrPathname)) {
				result = Write(keyPathOrPathname, "", REG_SZ);
				break;
			}
			std::string outPath;
			hKeyRoot = PathParse(keyPathOrPathname, outPath);
			if (!hKeyRoot)
				break;
			std::wstring wKeyPath, wKeyValueName;
			wKeyPath = IConv::MBytesToWString(shared::Win::GetPathByPathnameA(outPath));
			wKeyValueName = IConv::MBytesToWString(shared::Win::GetNameByPathnameA(outPath));
			if (wKeyPath.empty() || wKeyValueName.empty())
				break;
			retResult = regKeyObj.Open(hKeyRoot, wKeyPath.c_str(), KEY_ALL_ACCESS);
			if (ERROR_SUCCESS != retResult)
				break;
			retResult = regKeyObj.DeleteValue(wKeyValueName.c_str());
			if (ERROR_SUCCESS != retResult)
				break;
			result = true;
		} while (0);
		if (hKeyRoot) {
			::RegCloseKey(hKeyRoot);
			hKeyRoot = nullptr;
		}
		return result;
	}
	bool Win::Registry::DeleteKey(const std::string& keyPathOrPathname) {
		bool result = false;
		HKEY hKeyRoot = nullptr;
		CRegKey regKeyObj;
		LSTATUS retResult = ERROR_INVALID_FUNCTION;
		do {
			std::string FixKeyPathOrPathname{ keyPathOrPathname };
			if (FixKeyPathOrPathname.empty())
				break;
			if (shared::Win::IsPathA(FixKeyPathOrPathname))
				FixKeyPathOrPathname = shared::Win::PathToPathnameA(FixKeyPathOrPathname);

			std::string outPath;
			hKeyRoot = PathParse(FixKeyPathOrPathname, outPath);
			if (!hKeyRoot)
				break;
			std::wstring wKeyPath, wKeyName;
			wKeyPath = IConv::MBytesToWString(shared::Win::GetPathByPathnameA(outPath));
			wKeyName = IConv::MBytesToWString(shared::Win::GetNameByPathnameA(outPath));
			if (wKeyPath.empty() || wKeyName.empty())
				break;
			retResult = regKeyObj.Open(hKeyRoot, wKeyPath.c_str(), KEY_ALL_ACCESS);
			if (ERROR_SUCCESS != retResult)
				break;
			retResult = regKeyObj.RecurseDeleteKey(wKeyName.c_str());
			if (ERROR_SUCCESS != retResult)
				break;
			result = true;
		} while (0);
		if (hKeyRoot) {
			::RegCloseKey(hKeyRoot);
			hKeyRoot = nullptr;
		}
		return result;
	}
	bool Win::Registry::Delete(const std::string& keyPathname, const std::string& keyValue, const HKEY& hKeyIn /*= nullptr*/) {
		bool result = false;
		HKEY keyRoot = hKeyIn;
		do {
			if (keyPathname.empty())
				break;
			std::string keyPath;
			if (!keyRoot)
				keyRoot = PathParse(keyPathname, keyPath);
			else
				keyPath = PathParse(keyPathname);
			if (!keyRoot)
				break;
			CRegKey regKeyObj;
			LSTATUS retResult = ERROR_INVALID_FUNCTION;
			if (keyValue.empty()) {//!@ 删除注册表[项]
				std::string delete_path = Win::GetPathByPathnameA(Win::PathToPathnameA(keyPath));
				std::string delete_name = Win::GetNameByPathnameA(Win::PathToPathnameA(keyPath));
				do {
#ifdef _WIN64
					retResult = regKeyObj.Open(keyRoot, IConv::MBytesToWString(delete_path).c_str(), KEY_ALL_ACCESS);
#else
					retResult = regKeyObj.Open(keyRoot, IConv::MBytesToWString(delete_path).c_str(), KEY_ALL_ACCESS | KEY_WOW64_64KEY);
#endif
					if (ERROR_SUCCESS != retResult)
						break;
					retResult = regKeyObj.RecurseDeleteKey(IConv::MBytesToWString(delete_name).c_str());
					if (ERROR_SUCCESS != retResult)
						break;
					result = true;
				} while (0);
			}
			else {//!@ 删除注册表[项键值]
				do {
#ifdef _WIN64
					retResult = regKeyObj.Open(keyRoot, IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS);
#else
					retResult = regKeyObj.Open(keyRoot, IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS | KEY_WOW64_64KEY);
#endif
					if (ERROR_SUCCESS != retResult)
						break;
					retResult = regKeyObj.DeleteValue(IConv::MBytesToWString(keyValue).c_str());
					if (ERROR_SUCCESS != retResult)
						break;
					result = true;
				} while (0);
			}
			regKeyObj.Close();
		} while (0);
		if (keyRoot && !hKeyIn) {
			::RegCloseKey(keyRoot);
			keyRoot = nullptr;
		}
		return result;
	}
	bool Win::Registry::Write(const std::string& keyPathname, const std::string& keyData, const DWORD& keyType, const HKEY& hKeyIn /*= nullptr*/) {
		bool result = false;
		HKEY keyRoot = hKeyIn;
		do {
#if 0
			if (keyData.empty() && keyType != REG_NONE)
				break;
#endif
			if (keyPathname.empty())
				break;
			std::string keyPath, keyName;
			if (!keyRoot)
				keyRoot = PathParse(keyPathname, keyPath);
			else
				keyPath = PathParse(keyPathname);
			if (!keyRoot)
				break;
			keyPath = PathFixedA(GetPathByPathnameA(keyPath));
			if (keyPath.empty())
				break;
			if (keyName.empty())
				keyName = GetNameByPathnameA(keyPathname);
			CRegKey regKeyObj;
			LSTATUS retResult = ERROR_INVALID_FUNCTION;
			do {
#ifndef _WIN64
				retResult = regKeyObj.Open(keyRoot, IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS | KEY_WOW64_64KEY);
#else
				retResult = regKeyObj.Open(keyRoot, IConv::MBytesToWString(keyPath).c_str(), KEY_ALL_ACCESS);
#endif
				if (ERROR_SUCCESS != retResult) {
					retResult = regKeyObj.Create(keyRoot, IConv::MBytesToWString(keyPath).c_str());
				}
				if (ERROR_SUCCESS != retResult)
					break;
				/*if (!keyData.empty())
				{*/
				std::string write_data{ keyData };
				if (keyType == REG_SZ || keyType == REG_EXPAND_SZ) {
					std::wstring unicode = IConv::MBytesToWString(write_data);
					if (!unicode.empty()) {
						write_data.clear();
						write_data.append((char*)&unicode[0], unicode.size() * sizeof(wchar_t));
					}
				}
				retResult = regKeyObj.SetValue(IConv::MBytesToWString(keyName).c_str(), keyType, &write_data[0], static_cast<ULONG>(write_data.size()));
				if (ERROR_SUCCESS != retResult)
					break;
				/*}*/
				result = true;
			} while (0);
			regKeyObj.Close();
		} while (0);
		if (keyRoot && !hKeyIn) {
			::RegCloseKey(keyRoot);
			keyRoot = nullptr;
		}
		return result;
	}
	bool Win::Registry::UserLayer(const std::string& regPathname) {
		bool result = false;
		do {
			if (regPathname.empty())
				break;
			result = ::StrStrIA(regPathname.c_str(), "HKEY_CURRENT_USER") != nullptr;
		} while (0);
		return result;
	}
	bool Win::Registry::Read(const std::string& keyPathname, std::string& OutVal) {
		DWORD outDwType = 0;
		ULONG outDataSize = 0;
		return Read(keyPathname, "", OutVal, outDwType, outDataSize, nullptr);
	}
	bool Win::Registry::Read(const std::string& keyPathname, const std::string& keyVal, std::string& outVal, DWORD& outDwType, ULONG& outDataSize, const HKEY& hKeyIn /*= nullptr*/) {
		outDwType = 0;
		outDataSize = 0;
		outVal.clear();
		bool result = false;
		HKEY hKey = hKeyIn;
		std::string findPath{  };
		std::string findKey{ keyVal };
		do {
			if (keyPathname.empty())
				break;
			std::string keyPath;
			if (!hKey)
				hKey = PathParse(keyPathname, keyPath);
			else
				keyPath = PathParse(keyPathname);
			if (!hKey)
				break;
			findPath = PathFixedA(keyPath);
			if (findKey.empty()) {
				findPath = PathFixedA(GetPathByPathnameA(keyPath));
				findKey = GetNameByPathnameA(keyPathname);
			}
			CRegKey regKeyObj;
			LSTATUS realRet = ERROR_INVALID_FUNCTION;
			do {
#ifndef _WIN64
				REGSAM open_access = KEY_READ | KEY_WOW64_64KEY;
#else
				REGSAM open_access = KEY_READ;
#endif

#ifdef _UNICODE
				realRet = regKeyObj.Open(hKey, IConv::MBytesToWString(findPath).c_str(), open_access);
#else
				realRet = regKeyObj.Open(hKey, findPath.c_str(), open_access);
#endif
				if (ERROR_SUCCESS != realRet)
					break;
#ifdef _UNICODE
				realRet = regKeyObj.QueryValue(IConv::MBytesToWString(findKey).c_str(), &outDwType, nullptr, &outDataSize);
#else
				realRet = regKeyObj.QueryValue(findKey, &outDwType, nullptr, &outDataSize);
#endif
				if (ERROR_SUCCESS != realRet || outDataSize <= 0)
					break;
				outVal.resize(outDataSize, 0x00);
#ifdef _UNICODE
				realRet = regKeyObj.QueryValue(IConv::MBytesToWString(findKey).c_str(), &outDwType, &outVal[0], &outDataSize);
#else
				realRet = regKeyObj.QueryValue(findKey, &outDwType, &outVal[0], &outDataSize);
#endif
				result = !outVal.empty();
			} while (0);
			regKeyObj.Close();
		} while (0);
		if (hKey && !hKeyIn) {
			::RegCloseKey(hKey);
			hKey = nullptr;
		}
		if (result) {
			switch (outDwType) {
			case REG_SZ:
			case REG_EXPAND_SZ: {
				if (outDataSize <= 0 || outVal.empty())
					break;
				auto temp = new char[outDataSize + sizeof(wchar_t)];
				::memset(temp, 0x00, outDataSize + sizeof(wchar_t));
				::memcpy(temp, &outVal[0], outDataSize);
				outVal = IConv::WStringToMBytes((wchar_t*)temp);
				SK_DELETE_PTR_BUFFER(temp);
			}break;
			default:
				break;
			}
		}
		return result;
	}
	std::string Win::Registry::PathParse(const std::string& regPath) {
		std::string result;
		do {
			if (regPath.empty())
				break;
			std::string findStr{ regPath };
			for (auto it = findStr.begin(); it != findStr.end(); ++it) {
				if (*it == '\\' || *it == '/') {
					auto current_pos = std::distance(findStr.begin(), it);
					if (findStr.size() - current_pos > 0) {
						result.append(findStr.data() + current_pos, findStr.size() - current_pos);
						do {
							if (result.empty())
								break;
							if (result[0] == '\\' || result[0] == '/')
								result.erase(result.begin());
							else
								break;
						} while (1);
					}
					findStr.resize(std::distance(findStr.begin(), it));
					break;
				}
			}
		} while (0);
		return result;
	}
	HKEY Win::Registry::PathParse(const std::string& regPath, std::string& path) {
		HKEY result = nullptr;
		path.clear();
		do {
			if (regPath.empty())
				break;
			std::string findStr{ regPath };
			for (auto it = findStr.begin(); it != findStr.end(); ++it) {
				if (*it == '\\' || *it == '/') {
					auto current_pos = std::distance(findStr.begin(), it);
					if (findStr.size() - current_pos > 0) {
						path.append(findStr.data() + current_pos, findStr.size() - current_pos);
						do {
							if (path.empty())
								break;
							if (path[0] == '\\' || path[0] == '/')
								path.erase(path.begin());
							else
								break;
						} while (1);
					}
					findStr.resize(std::distance(findStr.begin(), it));
					break;
				}
			}
			if (findStr.empty())
				break;
			if (0 == findStr.compare("HKEY_CLASSES_ROOT"))
				result = HKEY_CLASSES_ROOT;
			else if (0 == findStr.compare("HKEY_CURRENT_USER"))
				result = HKEY_CURRENT_USER;
			else if (0 == findStr.compare("HKEY_LOCAL_MACHINE"))
				result = HKEY_LOCAL_MACHINE;
			else if (0 == findStr.compare("HKEY_USERS"))
				result = HKEY_USERS;
			else if (0 == findStr.compare("HKEY_PERFORMANCE_DATA"))
				result = HKEY_PERFORMANCE_DATA;
			else if (0 == findStr.compare("HKEY_PERFORMANCE_TEXT"))
				result = HKEY_PERFORMANCE_TEXT;
			else if (0 == findStr.compare("HKEY_PERFORMANCE_NLSTEXT"))
				result = HKEY_PERFORMANCE_NLSTEXT;
#if(WINVER >= 0x0400)
			else if (0 == findStr.compare("HKEY_CURRENT_CONFIG"))
				result = HKEY_CURRENT_CONFIG;
			else if (0 == findStr.compare("HKEY_DYN_DATA"))
				result = HKEY_DYN_DATA;
#endif
		} while (0);
		return result;
	}
#if 0
	HKEY Win::Registry::HKCUGetBySystem() {
		HKEY result = nullptr;
		HANDLE hProcess = nullptr;
		HANDLE hToken = nullptr;
		do {
			DWORD dwCurrentUserProcessId = Win::Process::GetCurrentUserProcessId();
			if (dwCurrentUserProcessId <= 0)
				break;
			hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCurrentUserProcessId);
			if (!hProcess)
				break;
			if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
				break;
			if (!::ImpersonateLoggedOnUser(hToken))
				break;
			if (ERROR_SUCCESS != ::RegOpenCurrentUser(KEY_ALL_ACCESS, &result))
				break;
		} while (0);
		SK_CLOSE_HANDLE(hToken);
		SK_CLOSE_HANDLE(hProcess);
		return result;
	}
#endif
	/*
std::set<std::string> keys;
EnumKey(R"(HKEY_CURRENT_USER\Software\Microsoft\WindowsApps\SK\LAUNCH\)", keys);
*/
	void Win::Registry::EnumKeys(const std::string& keyPathname, std::set<std::string>& outKeys, const HKEY& hKeyIn /*= nullptr*/) {
		HKEY hKeyRoot = hKeyIn;
		HKEY hKeyTarget = nullptr;
		do {
			std::string keyPath;
			if (!hKeyRoot)
				hKeyRoot = Win::Registry::PathParse(keyPathname, keyPath);
			else
				keyPath = PathParse(keyPathname);
			if (keyPath.empty() || !hKeyRoot)
				break;
			if (ERROR_SUCCESS != ::RegOpenKeyExW(hKeyRoot,
				shared::IConv::MBytesToWString(keyPath).c_str(), 0, KEY_ALL_ACCESS, &hKeyTarget))
				break;
			if (!hKeyTarget)
				break;
#if 0
			DWORD dwCountOfSubKey = 0;
			DWORD dwCountOfValue = 0;
			if (ERROR_SUCCESS != ::RegQueryInfoKeyW(hKeyTarget, NULL, NULL, NULL, &dwCountOfSubKey, NULL, NULL, &dwCountOfValue, NULL, NULL, NULL, NULL))
				break;
#else
			DWORD dwCountOfSubKey = 0;
			DWORD dwCountOfValue = 0;
			DWORD lpcchClass = 0;
			LPDWORD pReserved = nullptr;
			DWORD lpcbMaxSubKeyLen = 0;
			DWORD lpcbMaxClassLen = 0;
			DWORD lpcbMaxValueNameLen = 0;
			DWORD lpcbMaxValueLen = 0;
			DWORD lpcbSecurityDescriptor = 0;
			FILETIME lpftLastWriteTime = { 0 };
			if (ERROR_SUCCESS != ::RegQueryInfoKeyW(hKeyTarget, NULL, &lpcchClass, pReserved, &dwCountOfSubKey, &lpcbMaxSubKeyLen, &lpcbMaxClassLen, &dwCountOfValue, &lpcbMaxValueNameLen, &lpcbMaxValueLen, &lpcbSecurityDescriptor, &lpftLastWriteTime))
				break;
#endif
			if (dwCountOfSubKey == 0)
				break;
			for (DWORD i = 0; i < dwCountOfSubKey; ++i) {
				wchar_t lpName[_MAX_PATH] = { 0 };
				DWORD cchName = _MAX_PATH;
				::RegEnumKeyW(hKeyTarget, i, lpName, cchName);
				if (lpName[0])
					outKeys.emplace(shared::IConv::WStringToMBytes(lpName));
			}
		} while (0);

		if (hKeyTarget) {
			::RegCloseKey(hKeyTarget);
			hKeyTarget = nullptr;
		}
		if (hKeyRoot && !hKeyIn) {
			::RegCloseKey(hKeyRoot);
			hKeyRoot = nullptr;
		}
	}
	void Win::Registry::EnumValues(const std::string& keyPathname, std::map<std::string, std::string>& outVals, const HKEY& hKeyIn /*= nullptr*/) {
		HKEY hKeyRoot = hKeyIn;
		HKEY hKeyTarget = nullptr;
		do {
			std::string keyPath;
			if (!hKeyRoot)
				hKeyRoot = Win::Registry::PathParse(keyPathname, keyPath);
			else
				keyPath = PathParse(keyPathname);
			if (keyPath.empty() || !hKeyRoot)
				break;
			if (ERROR_SUCCESS != ::RegOpenKeyExA(hKeyRoot, keyPath.c_str(), 0, KEY_ALL_ACCESS, &hKeyTarget))
				break;
			if (!hKeyTarget)
				break;
#if 0
			DWORD dwCountOfSubKey = 0;
			DWORD dwCountOfValue = 0;
			if (ERROR_SUCCESS != ::RegQueryInfoKeyA(hKeyTarget, NULL, NULL, NULL, &dwCountOfSubKey, NULL, NULL, &dwCountOfValue, NULL, NULL, NULL, NULL))
				break;
#else
			DWORD dwCountOfSubKey = 0;
			DWORD dwCountOfValue = 0;
			DWORD lpcchClass = 0;
			LPDWORD pReserved = nullptr;
			DWORD lpcbMaxSubKeyLen = 0;
			DWORD lpcbMaxClassLen = 0;
			DWORD lpcbMaxValueNameLen = 0;
			DWORD lpcbMaxValueLen = 0;
			DWORD lpcbSecurityDescriptor = 0;
			FILETIME lpftLastWriteTime = { 0 };
			if (ERROR_SUCCESS != ::RegQueryInfoKeyA(hKeyTarget, NULL, &lpcchClass, pReserved, &dwCountOfSubKey, &lpcbMaxSubKeyLen, &lpcbMaxClassLen, &dwCountOfValue, &lpcbMaxValueNameLen, &lpcbMaxValueLen, &lpcbSecurityDescriptor, &lpftLastWriteTime))
				break;
#endif
			if (dwCountOfValue == 0)
				break;
			for (DWORD i = 0; i < dwCountOfValue; ++i) {
				std::string keyName;
				std::string value;
				char lpValueName[_MAX_PATH] = { 0 };
				DWORD lpcchValueName = _MAX_PATH;
				DWORD lpType = 0;
				BYTE* lpData = new BYTE[_MAX_PATH];
				DWORD lpcbData = _MAX_PATH;
				DWORD lpReserved = 0;
				::RegEnumValueA(hKeyTarget, i, lpValueName, &lpcchValueName, nullptr, &lpType, lpData, &lpcbData);
				if (lpcbData > _MAX_PATH) {
					SK_DELETE_PTR_BUFFER(lpData);
					lpData = new BYTE[lpcbData];
					::RegEnumValueA(hKeyTarget, i, lpValueName, &lpcchValueName, nullptr, &lpType, lpData, &lpcbData);
				}
				if (lpcchValueName > 0)
					switch (lpType) {
					case REG_SZ:
					case REG_EXPAND_SZ: {
						if (lpcbData <= 0 || !lpData)
							break;
						auto temp = new char[lpcbData + sizeof(char)];
						::memset(temp, 0x00, lpcbData + sizeof(char));
						::memcpy(temp, lpData, lpcbData);
						value = (char*)temp;
						SK_DELETE_PTR_BUFFER(temp);
					}break;
					default: {
						value.append((char*)lpData, lpcbData);
					}break;
					}
				if (lpcchValueName > 0 && lpValueName[0])
					keyName = std::string(lpValueName, lpcchValueName);
				if (!keyName.empty())
					outVals.emplace(keyName, value);
				SK_DELETE_PTR_BUFFER(lpData);
			}
		} while (0);

		if (hKeyTarget) {
			::RegCloseKey(hKeyTarget);
			hKeyTarget = nullptr;
		}
		if (hKeyRoot && !hKeyIn) {
			::RegCloseKey(hKeyRoot);
			hKeyRoot = nullptr;
		}
	}

#if 0//!@ 写入DWORD 类型例子
	Here is my code to change a registry DWORD value on a foreign box by
		WMI.

		IWbemLocator* ppiWmiLoc = NULL;

	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*)&ppiWmiLoc);

	IWbemServices* pSvc = NULL;
	CComBSTR bstrserv(L"\\\\MyComputer\\root\\default");
	CComBSTR bstrUsername(m_csUserName), bstrPassword(m_csPassword);

	hres = ppiWmiLoc->ConnectServer(bstrserv, NULL, NULL, 0, NULL, 0, 0,
		&pSvc);
	if (SUCCEEDED(hres))
	{
		hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
			EOAC_NONE);

		if (SUCCEEDED(hres))
		{
			// Connect to the local root\cimv2 namespace
			// and obtain pointer pSvc to make IWbemServices calls.
			BSTR MethodName = SysAllocString(L"SetDWORDValue");
			BSTR ClassName = SysAllocString(L"StdRegProv");

			IWbemClassObject* pClass = NULL;
			hres = pSvc->GetObject(ClassName, 0, NULL, &pClass, NULL);

			if (SUCCEEDED(hres))
			{
				IWbemClassObject* pInParamsDefinition = NULL;
				hres = pClass->GetMethod(MethodName, 0,
					&pInParamsDefinition, NULL);

				if (SUCCEEDED(hres))
				{
					IWbemClassObject* pClassInstance = NULL;
					hres = pInParamsDefinition->SpawnInstance(0, &pClassInstance);

					if (SUCCEEDED(hres))
					{
						/*// Create the values for the in parameters
						VARIANT varCommand;
						varCommand.vt = VT_UINT;
						varCommand.uintVal = &H80000002;

						// Store the value for the in parameters
						hres = pClassInstance->Put(L"hDefKey", 0,
						&varCommand, 0);*/

						// Create the values for the in parameters
						VARIANT varCommand;
						varCommand.vt = VT_BSTR;
						varCommand.bstrVal = L"System\CurrentControlSet\Control\Lsa";

						// Store the value for the in parameters
						hres = pClassInstance->Put(L"sSubKeyName", 0,
							&varCommand, 0);

						// Create the values for the in parameters

						varCommand.vt = VT_BSTR;
						varCommand.bstrVal = L"ForceGuest";

						// Store the value for the in parameters
						hres = pClassInstance->Put(L"sValueName", 0,
							&varCommand, 0);

						// Create the values for the in parameters

						varCommand.vt = VT_I4;
						varCommand.lVal = 0;

						// Store the value for the in parameters
						hres = pClassInstance->Put(L"lValue", 0,
							&varCommand, CIM_UINT8);

						// Execute Method
						IWbemClassObject* pOutParams = NULL;
						hres = pSvc->ExecMethod(ClassName, MethodName, 0,
							NULL, pClassInstance, &pOutParams, NULL);

						if (FAILED(hres))
						{
							MessageBox("Command Failed", "SetStringValue", MB_OK);
							/* VariantClear(&varCommand);
							SysFreeString(ClassName);
							SysFreeString(MethodName);
							pClass->Release();
							pInParamsDefinition->Release();
							pOutParams->Release();
							pSvc->Release();*/
						}
						else
						{
							VARIANT varReturnValue;
							CComBSTR bstrret(L"ReturnValue");
							BSTR ret;
							hres = pOutParams->GetObjectText(0, &ret);
							char mess[MAX_PATH];
							wsprintf(mess, "%S", ret);
							MessageBox(mess, "Error message", MB_OK);
							/*hres = pOutParams->Get(bstrret, 0, &varReturnValue, NULL,
							0);
							SHORT toto = varReturnValue.iVal;*/
						}

						pClassInstance->Put function needs a Variant.So
							I have to pass a dword, but by the variant.As I passed my string
							value by Variant : var.vt = VT_BSTR and then var.bstrval =
							L"MyVariant"; Do you know what I mean ? I have to find the exact type
							I have to use and the exact format I have to use for my DWORD value.

							IWbemClassObject* pClassInstance;


						HRESULT Put(
							LPCWSTR wszName,
							LONG lFlags,
							VARIANT * pVal,
							CIMTYPE vtType
						);
#endif
						bool Win::Registry::ComSetValue(const std::string & regPathname, const std::string & valueBin, const DWORD & valueType) {
							bool result = false;
							HKEY hKeyRoot = nullptr;
							IWbemLocator* pIWbemLocator = nullptr;
							IWbemServices* pIWbemServices = nullptr;

							IWbemClassObject* pObjectStdRegProv = nullptr;
							IWbemClassObject* pObjectSetStringValue = nullptr;
							IWbemClassObject* pObjectSetDWORDValue = nullptr;
							IWbemClassObject* pOutParams = nullptr;

							HRESULT hResult = ::CoInitializeEx(NULL, COINIT_MULTITHREADED/*COINIT_APARTMENTTHREADED*/);
							do {
								if (FAILED(hResult))
									break;
								hResult = ::CoInitializeSecurity(
									NULL,
									-1,                          // COM authentication
									NULL,                        // Authentication services
									NULL,                        // Reserved
									RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
									RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
									NULL,                        // Authentication info
									EOAC_NONE,                   // Additional capabilities 
									NULL                         // Reserved
								);
								if (FAILED(hResult))
									break;

								hResult = ::CoCreateInstance(
									CLSID_WbemLocator,
									0,
									CLSCTX_INPROC_SERVER,
									IID_IWbemLocator, (LPVOID*)&pIWbemLocator);
								if (FAILED(hResult))
									break;


								hResult = pIWbemLocator->ConnectServer(
									_bstr_t(L"ROOT\\DEFAULT"), // Object path of WMI namespace
									NULL,                    // User name. NULL = current user
									NULL,                    // User password. NULL = current
									0,                       // Locale. NULL indicates current
									NULL,                    // Security flags.
									0,                       // Authority (for example, Kerberos)
									0,                       // Context object 
									&pIWbemServices                    // pointer to IWbemServices proxy
								);
								if (FAILED(hResult))
									break;

								hResult = ::CoSetProxyBlanket(
									pIWbemServices,                        // Indicates the proxy to set
									RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
									RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
									NULL,                        // Server principal name 
									RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
									RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
									NULL,                        // client identity
									EOAC_NONE                    // proxy capabilities 
								);
								if (FAILED(hResult))
									break;

								hResult = pIWbemServices->GetObjectW(_bstr_t(L"StdRegProv"), 0, 0, &pObjectStdRegProv, 0);
								if (FAILED(hResult))
									break;
								hResult = pObjectStdRegProv->GetMethod(_bstr_t(L"SetStringValue"), 0, &pObjectSetStringValue, 0);
								if (FAILED(hResult))
									break;
								hResult = pObjectStdRegProv->GetMethod(_bstr_t(L"SetDWORDValue"), 0, &pObjectSetDWORDValue, 0);
								if (FAILED(hResult))
									break;
								if (regPathname.empty())
									break;
								if (valueBin.empty())
									break;
								if (Win::IsPathA(regPathname))
									break;
								if (/*valueType != REG_DWORD &&*/ valueType != REG_SZ)
									break;
								std::string keyPath, keyValue;
								hKeyRoot = Registry::PathParse(regPathname, keyPath);
								if (!hKeyRoot)
									break;

								keyValue = Win::GetNameByPathnameA(keyPath);
								keyPath = Win::GetPathByPathnameA(keyPath);
								if (keyValue.empty() || keyPath.empty())
									break;
								//!@ 根目录
								_variant_t hDefKeyValue(std::to_wstring(static_cast<DWORD>(reinterpret_cast<std::uintptr_t>(hKeyRoot))).c_str());
								hResult = pObjectSetStringValue->Put(_bstr_t(L"hDefKey"), 0, &hDefKeyValue, CIM_UINT32);
								if (FAILED(hResult))
									break;

								//!@ 注册表路径
								_variant_t sSubKeyNameValue(IConv::MBytesToWString(keyPath).c_str());
								hResult = pObjectSetStringValue->Put(_bstr_t("sSubKeyName"), 0, &sSubKeyNameValue, CIM_STRING);
								if (FAILED(hResult))
									break;
								//!@ 设置项名
								_variant_t sValueNameValue(IConv::MBytesToWString(keyValue).c_str());
								hResult = pObjectSetStringValue->Put(_bstr_t(L"sValueName"), 0, &sValueNameValue, CIM_STRING);
								if (FAILED(hResult))
									break;
								//!@ 设置值
								if (valueType == REG_DWORD) {
									DWORD dwValue = 0;
									::memcpy(&dwValue, valueBin.data(), sizeof(dwValue));
									_variant_t sValueValue;
									sValueValue.vt = VT_I4;
									sValueValue.lVal = dwValue;
									hResult = pObjectSetDWORDValue->Put(_bstr_t(L"lValue"), 0, &sValueValue, CIM_UINT32);
								}
								else {
									_variant_t sValueValue(IConv::MBytesToWString(valueBin).c_str());
									hResult = pObjectSetStringValue->Put(_bstr_t(L"sValue"), 0, &sValueValue, CIM_STRING);
								}


								if (FAILED(hResult))
									break;

								hResult = pIWbemServices->ExecMethod(_bstr_t(L"StdRegProv"),
									valueType == REG_SZ ? _bstr_t(L"SetStringValue") : _bstr_t(L"SetDWORDValue"), 0, 0,
									valueType == REG_SZ ? pObjectSetStringValue : pObjectSetDWORDValue,
									&pOutParams, 0);
								if (FAILED(hResult))
									break;

								result = true;
							} while (0);

							if (pOutParams) {
								pOutParams->Release();
								pOutParams = nullptr;
							}
							if (pObjectSetDWORDValue) {
								pObjectSetDWORDValue->Release();
								pObjectSetDWORDValue = nullptr;
							}
							if (pObjectSetStringValue) {
								pObjectSetStringValue->Release();
								pObjectSetStringValue = nullptr;
							}
							if (pObjectStdRegProv) {
								pObjectStdRegProv->Release();
								pObjectStdRegProv = nullptr;
							}
							if (pIWbemServices) {
								pIWbemServices->Release();
								pIWbemServices = nullptr;
							}
							if (pIWbemLocator) {
								pIWbemLocator->Release();
								pIWbemLocator = nullptr;
							}
							if (hKeyRoot) {
								::RegCloseKey(hKeyRoot);
								hKeyRoot = nullptr;
							}
							::CoUninitialize();
							return result;
						}

						//HKEY_CLASSES_ROOT
						//HKEY_CURRENT_USER
						//HKEY_LOCAL_MACHINE
						//HKEY_USERS
						//HKEY_CURRENT_CONFIG
						/*@ https://docs.microsoft.com/zh-cn/troubleshoot/windows-client/application-management/change-registry-values-permissions
						当你使用 等在用户模式下获取对注册表的访问权限时，字符串在内核模式下 HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER 转换，如下所示：
						HKEY_LOCAL_MACHINE转换为 \registry\machine 。
						HKEY_USERS转换为 \registry\user 。
						HKEY_CURRENT_USER 转换为 ，其中 user_sid \registry\user\user_sid 是与用户关联的安全 ID。
						HKEY_CLASSES_ROOT转换为 \registry\machine\software\classes 。
						*/
						bool Win::Registry::NtFixPathHeadA(const std::string & inputPathOrPathname, std::string & output, std::vector<std::string>&outKeyPaths, const std::string & user_sid /*= ""*/) {
							bool result = false;
							outKeyPaths.clear();
							output = inputPathOrPathname;

							std::string replace_string;
							do {
								if (output.empty())
									break;
								if (::StrStrIA(output.c_str(), R"(\REGISTRY\)")) {
									result = true;
									break;
								}
								if (!::memicmp("HKEY_CLASSES_ROOT", output.c_str(), ::strlen("HKEY_CLASSES_ROOT"))) {
									replace_string = R"(\REGISTRY\MACHINE\SOFTWARE\CLASSES)";
									output.replace(0, ::strlen("HKEY_CLASSES_ROOT"), replace_string);
									result = true;
								}
								else if (!::memicmp("HKEY_CURRENT_USER", output.c_str(), ::strlen("HKEY_CURRENT_USER"))) {
									if (!user_sid.empty()) {
										replace_string = std::vformat(R"(\REGISTRY\USER\{})", std::make_format_args(user_sid));
										output.replace(0, ::strlen("HKEY_CURRENT_USER"), replace_string);
										result = true;
									}
								}
								else if (!::memicmp("HKEY_LOCAL_MACHINE", output.c_str(), ::strlen("HKEY_LOCAL_MACHINE"))) {
									replace_string = R"(\REGISTRY\MACHINE)";
									output.replace(0, ::strlen("HKEY_LOCAL_MACHINE"), replace_string);
									result = true;
								}
								else if (!::memicmp("HKEY_USERS", output.c_str(), ::strlen("HKEY_USERS"))) {
									replace_string = R"(\REGISTRY\USER)";
									output.replace(0, ::strlen("HKEY_USERS"), R"(\REGISTRY\USER)");
									result = true;
								}
							} while (0);

							do {
								if (!result || replace_string.empty() || output.empty())
									break;
								std::string begin{ shared::Win::PathFixedA(replace_string + "\\") };
								std::string src{ shared::Win::PathFixedA(output) };
								if (src.find(begin) != 0)
									break;
								src.erase(0, begin.size());
								do {
									if (src.empty())
										break;
									if (*src.begin() == '\\') {
										src.erase(src.begin());
										continue;
									}
									auto found = src.find("\\");
									if (found == std::string::npos)
										break;
									begin = outKeyPaths.emplace_back(begin + src.substr(0, found)) + "\\";
									src.erase(0, found);
								} while (1);


							} while (0);



							return result;
						}
						bool Win::Registry::NtFixPathHeadW(const std::string & inputPathOrPathname, std::wstring & outputPathOrPathnameW, std::vector<std::wstring>&outKeyPathsW, const std::string & user_sid /*= ""*/) {
							bool result = false;
							outKeyPathsW.clear();
							std::string outputPathOrPathnameA;
							std::vector<std::string> outKeyPaths;
							result = NtFixPathHeadA(inputPathOrPathname, outputPathOrPathnameA, outKeyPaths, user_sid);
							for (const auto& keyPathA : outKeyPaths)
								outKeyPathsW.emplace_back(shared::IConv::MBytesToWString(keyPathA));
							if (result && !outputPathOrPathnameA.empty())
								outputPathOrPathnameW = IConv::MBytesToWString(outputPathOrPathnameA);
							return result;
						}

						static void InitUnicodeString(UNICODE_STRING & outString, const std::wstring & input = L"") {
							::memset(&outString, 0x00, sizeof(outString));
							outString.Buffer = input.empty() ? nullptr : (decltype(outString.Buffer))&input[0];
							outString.Length = input.empty() ? 0 : static_cast<decltype(UNICODE_STRING::Length)>(input.size() * sizeof(wchar_t));
							outString.MaximumLength = input.empty() ? 0 : static_cast<decltype(UNICODE_STRING::MaximumLength)>(input.size() * sizeof(wchar_t));
						}
						static void InitUnicodeStringObj(const std::wstring & input, UNICODE_STRING & outString, OBJECT_ATTRIBUTES & outObject) {
							::memset(&outString, 0x00, sizeof(outString));
							::memset(&outObject, 0x00, sizeof(outObject));
							InitUnicodeString(outString, input);
							InitializeObjectAttributes(&outObject, &outString, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
						}

						bool Win::Registry::NtRead(const std::string & regPathOrPathname, std::string & outBinData, const std::string & user_sid /*= ""*/) {
							bool result = false;
							outBinData.clear();

							using tfZwClose = LONG(__stdcall*)(_In_ HANDLE);
							using tfZwOpenKey = LONG(__stdcall*)(_Out_ PHANDLE, _In_ ACCESS_MASK, _In_ POBJECT_ATTRIBUTES);
							using tfZwQueryKey = LONG(__stdcall*)(_In_ HANDLE, _In_ KEY_INFORMATION_CLASS, _In_opt_ PVOID, _In_ ULONG, _Out_ PULONG);
							using tfZwQueryValueKey = LONG(__stdcall*)(_In_ HANDLE, _In_ PUNICODE_STRING, _In_ KEY_VALUE_INFORMATION_CLASS, _Out_opt_ PVOID, _In_ ULONG, _Out_ PULONG);

							bool load_ntdll_flag = false;
							HMODULE hNTDLL = nullptr;
							HANDLE hOpenRegRoot = nullptr;
							PKEY_VALUE_PARTIAL_INFORMATION pKEY_VALUE_PARTIAL_INFORMATION = nullptr;
							tfZwClose ZwClose = nullptr;
							tfZwOpenKey ZwOpenKey = nullptr;
							tfZwQueryKey ZwQueryKey = nullptr;
							tfZwQueryValueKey ZwQueryValueKey = nullptr;
							do {
								hNTDLL = ::GetModuleHandleW(L"ntdll");
								if (!hNTDLL) {
									hNTDLL = ::LoadLibraryW(LR"(ntdll.dll)");
									if (hNTDLL)
										load_ntdll_flag = true;
									else
										break;
								}
								std::vector<std::wstring> wKeyPaths;
								std::wstring regPathOrPathnameW;
								if (!NtFixPathHeadW(regPathOrPathname, regPathOrPathnameW, wKeyPaths, user_sid) || regPathOrPathnameW.empty())
									break;
								ZwClose = reinterpret_cast<decltype(ZwClose)>(::GetProcAddress(hNTDLL, "ZwClose"));
								if (!ZwClose)
									break;
								ZwOpenKey = reinterpret_cast<decltype(ZwOpenKey)>(::GetProcAddress(hNTDLL, "ZwOpenKey"));
								if (!ZwOpenKey)
									break;
								ZwQueryKey = reinterpret_cast<decltype(ZwQueryKey)>(::GetProcAddress(hNTDLL, "ZwQueryKey"));
								if (!ZwQueryKey)
									break;
								ZwQueryValueKey = reinterpret_cast<decltype(ZwQueryValueKey)>(::GetProcAddress(hNTDLL, "ZwQueryValueKey"));
								if (!ZwQueryValueKey)
									break;

								std::wstring KeyPath, KeyName;
								if (!Win::IsPathW(regPathOrPathnameW)) {
									KeyPath = Win::GetPathByPathnameW(regPathOrPathnameW);
									KeyName = Win::GetNameByPathnameW(regPathOrPathnameW);
								}
								else {
									KeyPath = regPathOrPathnameW;
								}


								long status = 0;
								ULONG ulSize = 0;
								OBJECT_ATTRIBUTES KeyPathObj = { 0 };
								UNICODE_STRING KeyPathStr = { 0 };
								InitUnicodeStringObj(KeyPath, KeyPathStr, KeyPathObj);
								status = ZwOpenKey(&hOpenRegRoot, KEY_ALL_ACCESS, &KeyPathObj);
								if (!NT_SUCCESS(status))
									break;

								UNICODE_STRING QueryKeyNameStr = { 0 };
								InitUnicodeString(QueryKeyNameStr, KeyName);
								status = ZwQueryValueKey(hOpenRegRoot,
									&QueryKeyNameStr,
									KEY_VALUE_INFORMATION_CLASS::KeyValuePartialInformation,
									nullptr,
									ulSize,
									&ulSize);
								if (ulSize <= 0) {
									result = NT_SUCCESS(status);
									break;
								}

								pKEY_VALUE_PARTIAL_INFORMATION = (PKEY_VALUE_PARTIAL_INFORMATION)::LocalAlloc(LPTR, ulSize);
								status = ZwQueryValueKey(hOpenRegRoot,
									&QueryKeyNameStr,
									KEY_VALUE_INFORMATION_CLASS::KeyValuePartialInformation,
									pKEY_VALUE_PARTIAL_INFORMATION,
									ulSize,
									&ulSize);
								if (!NT_SUCCESS(status) || !pKEY_VALUE_PARTIAL_INFORMATION)
									break;

								switch (pKEY_VALUE_PARTIAL_INFORMATION->Type) {
								case REG_DWORD:
								case REG_QWORD:
								case REG_BINARY: {
									if (pKEY_VALUE_PARTIAL_INFORMATION->DataLength > 0 && pKEY_VALUE_PARTIAL_INFORMATION->Data)
										outBinData.append((char*)pKEY_VALUE_PARTIAL_INFORMATION->Data, pKEY_VALUE_PARTIAL_INFORMATION->DataLength);
									result = true;
								}break;
								case REG_MULTI_SZ:
								case REG_EXPAND_SZ:
								case REG_SZ: {
									if (pKEY_VALUE_PARTIAL_INFORMATION->Data && pKEY_VALUE_PARTIAL_INFORMATION->DataLength > 0) {
										std::wstring wData((wchar_t*)pKEY_VALUE_PARTIAL_INFORMATION->Data, pKEY_VALUE_PARTIAL_INFORMATION->DataLength);
										outBinData = IConv::WStringToMBytes(wData);
									}
									result = true;
								}break;
								default: {
									result = false;
									break;
								}
								}
							} while (0);
							if (hOpenRegRoot && ZwClose) {
								ZwClose(hOpenRegRoot);
								hOpenRegRoot = nullptr;
							}
							if (pKEY_VALUE_PARTIAL_INFORMATION) {
								::LocalFree(pKEY_VALUE_PARTIAL_INFORMATION);
								pKEY_VALUE_PARTIAL_INFORMATION = nullptr;
							}
							if (load_ntdll_flag && hNTDLL)
								SK_FREE_LIBRARY(hNTDLL);
							return result;
						}
						//\REGISTRY\MACHINE\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\SKTime\666\ -- 如果值不为空 则 写入默认路径
						//\REGISTRY\MACHINE\SYSTEM\CurrentControlSet\Services\W32Time\TimeProviders\SKTime\666 --如果键名不为空则创建新的键或者设置旧的键 为新的值 
						bool Win::Registry::NtWrite(const std::string & regPathOrPathname, const std::string & valueBin, const unsigned long& valueType, const std::string & user_sid /*= ""*/) {
							bool result = false;
							bool load_ntdll_flag = false;
							HMODULE hNTDLL = nullptr;
							HANDLE hOpenRegRoot = nullptr;
							using tfZwClose = LONG(__stdcall*)(_In_ HANDLE);
							using tfZwOpenKey = LONG(__stdcall*)(_Out_ PHANDLE, _In_ ACCESS_MASK, _In_ POBJECT_ATTRIBUTES);
							using tfZwCreateKey = LONG(__stdcall*)(_Out_ PHANDLE, _In_ ACCESS_MASK, _In_ POBJECT_ATTRIBUTES, _In_ ULONG, _In_opt_  PUNICODE_STRING, _In_ ULONG, _Out_opt_ PULONG);
							using tfZwSetValueKey = LONG(__stdcall*)(_In_ HANDLE, _In_ PUNICODE_STRING, _In_opt_ ULONG, _In_ ULONG, _In_opt_ PVOID, _In_ ULONG);
							tfZwClose ZwClose = nullptr;
							tfZwOpenKey ZwOpenKey = nullptr;
							tfZwCreateKey ZwCreateKey = nullptr;
							tfZwSetValueKey ZwSetValueKey = nullptr;
							do {
								hNTDLL = ::GetModuleHandleW(L"ntdll");
								if (!hNTDLL) {
									hNTDLL = ::LoadLibraryW(LR"(ntdll.dll)");
									if (hNTDLL)
										load_ntdll_flag = true;
									else
										break;
								}
								std::vector<std::wstring> wKeyPaths;
								std::wstring regPathOrPathnameW;
								if (!NtFixPathHeadW(regPathOrPathname, regPathOrPathnameW, wKeyPaths, user_sid) || regPathOrPathnameW.empty())
									break;

								ZwClose = reinterpret_cast<decltype(ZwClose)>(::GetProcAddress(hNTDLL, "ZwClose"));
								if (!ZwClose)
									break;
								ZwOpenKey = reinterpret_cast<decltype(ZwOpenKey)>(::GetProcAddress(hNTDLL, "ZwOpenKey"));
								if (!ZwOpenKey)
									break;
								ZwCreateKey = reinterpret_cast<decltype(ZwCreateKey)>(::GetProcAddress(hNTDLL, "ZwCreateKey"));
								if (!ZwCreateKey)
									break;
								ZwSetValueKey = reinterpret_cast<decltype(ZwSetValueKey)>(::GetProcAddress(hNTDLL, "ZwSetValueKey"));
								if (!ZwSetValueKey)
									break;

								std::wstring KeyPath, KeyName;
								if (!Win::IsPathW(regPathOrPathnameW)) {
									KeyPath = Win::GetPathByPathnameW(regPathOrPathnameW);
									KeyName = Win::GetNameByPathnameW(regPathOrPathnameW);
								}
								else {
									KeyPath = regPathOrPathnameW;
								}

								long status = 0;
								HANDLE hCreate = nullptr;
								ULONG Disposition = 0;
								OBJECT_ATTRIBUTES KeyPathObj = { 0 };
								UNICODE_STRING KeyPathStr = { 0 };
								ZwCreateKey(&hCreate, KEY_ALL_ACCESS, &KeyPathObj, 0, NULL, REG_OPTION_NON_VOLATILE, &Disposition);

								if (hCreate)
									hOpenRegRoot = hCreate;
								else {
									std::vector<HANDLE> createHds;
									for (auto it = wKeyPaths.begin(); it != wKeyPaths.end(); ++it) {
										InitUnicodeStringObj(*it, KeyPathStr, KeyPathObj);
										ZwCreateKey(&hCreate,
											KEY_ALL_ACCESS,
											&KeyPathObj,
											0, NULL, REG_OPTION_NON_VOLATILE, &Disposition);
										if (hCreate != nullptr) {
											createHds.emplace_back(hCreate);
										}
									}
									if (createHds.empty())
										break;
									hOpenRegRoot = *std::prev(createHds.end());
									createHds.pop_back();
									for (auto it = createHds.begin(); it != createHds.end(); ++it)
									{
										NtClose(*it);
										*it = nullptr;
									}
								}

								UNICODE_STRING QueryKeyNameStr = { 0 };
								InitUnicodeString(QueryKeyNameStr, KeyName);

								std::string writeValueCache;
								switch (valueType) {
								case REG_SZ:
								case REG_MULTI_SZ:
								case REG_EXPAND_SZ: {
									auto unicode = IConv::MBytesToWString(valueBin);
									if (!unicode.empty())
										writeValueCache = std::string((char*)unicode.data(), unicode.size() * sizeof(wchar_t));
								}break;
								default: {
									writeValueCache = valueBin;
								}break;
								}
								status = ZwSetValueKey(hOpenRegRoot, &QueryKeyNameStr,
									0,
									valueType,
									writeValueCache.empty() ? NULL : reinterpret_cast<void*>(const_cast<char*>(writeValueCache.data())),
									static_cast<ULONG>(writeValueCache.size()));
								if (!NT_SUCCESS(status))
									break;
								result = true;
							} while (0);
							if (hOpenRegRoot && ZwClose) {
								ZwClose(hOpenRegRoot);
								hOpenRegRoot = nullptr;
							}
							if (load_ntdll_flag && hNTDLL)
								SK_FREE_LIBRARY(hNTDLL);
							return result;
						}

						bool Win::Registry::NtDelete(const std::string & regPathOrPathname, const std::string & user_sid /*= ""*/) {
							bool result = false;
							bool load_ntdll_flag = false;
							HMODULE hNTDLL = nullptr;
							HANDLE hOpenRegRoot = nullptr;
							using tfZwClose = LONG(__stdcall*)(_In_ HANDLE);
							using tfZwOpenKey = LONG(__stdcall*)(_Out_ PHANDLE, _In_ ACCESS_MASK, _In_ POBJECT_ATTRIBUTES);
							using tfZwDeleteKey = LONG(__stdcall*)(_In_ HANDLE);
							using tfZwDeleteValueKey = LONG(__stdcall*)(_In_ HANDLE, _In_ PUNICODE_STRING);
							tfZwClose ZwClose = nullptr;
							tfZwOpenKey ZwOpenKey = nullptr;
							tfZwDeleteKey ZwDeleteKey = nullptr;
							tfZwDeleteValueKey ZwDeleteValueKey = nullptr;
							do {
								hNTDLL = ::GetModuleHandleW(L"ntdll");
								if (!hNTDLL) {
									hNTDLL = ::LoadLibraryW(LR"(ntdll.dll)");
									if (hNTDLL)
										load_ntdll_flag = true;
									else
										break;
								}
								std::vector<std::wstring> wKeyPaths;
								std::wstring regPathOrPathnameW;
								if (!NtFixPathHeadW(regPathOrPathname, regPathOrPathnameW, wKeyPaths, user_sid) || regPathOrPathnameW.empty())
									break;
								ZwClose = reinterpret_cast<decltype(ZwClose)>(::GetProcAddress(hNTDLL, "ZwClose"));
								if (!ZwClose)
									break;
								ZwOpenKey = reinterpret_cast<decltype(ZwOpenKey)>(::GetProcAddress(hNTDLL, "ZwOpenKey"));
								if (!ZwOpenKey)
									break;
								ZwDeleteKey = reinterpret_cast<decltype(ZwDeleteKey)>(::GetProcAddress(hNTDLL, "ZwDeleteKey"));
								if (!ZwDeleteKey)
									break;
								ZwDeleteValueKey = reinterpret_cast<decltype(ZwDeleteValueKey)>(::GetProcAddress(hNTDLL, "ZwDeleteValueKey"));
								if (!ZwDeleteValueKey)
									break;

								std::wstring KeyPath, KeyName;
								if (!Win::IsPathW(regPathOrPathnameW)) {
									KeyPath = Win::GetPathByPathnameW(regPathOrPathnameW);
									KeyName = Win::GetNameByPathnameW(regPathOrPathnameW);
								}
								else {
									KeyPath = regPathOrPathnameW;
								}

								long status = 0;
								ULONG ulSize = 0;
								OBJECT_ATTRIBUTES KeyPathObj = { 0 };
								UNICODE_STRING KeyPathStr = { 0 };
								InitUnicodeStringObj(KeyPath, KeyPathStr, KeyPathObj);
								status = ZwOpenKey(&hOpenRegRoot, KEY_ALL_ACCESS, &KeyPathObj);
								if (STATUS_ACCESS_DENIED == status)
									break;
								else if (STATUS_INVALID_HANDLE == status)
									break;
								else if (0xC0000034L == status) {
									result = true;
									break;
								}
								else if (!NT_SUCCESS(status) || !hOpenRegRoot)
									break;

								if (!KeyName.empty()) {
									UNICODE_STRING QueryKeyNameStr = { 0 };
									InitUnicodeString(QueryKeyNameStr, KeyName);
									status = ZwDeleteValueKey(hOpenRegRoot, &QueryKeyNameStr);
								}
								else {
									status = ZwDeleteKey(hOpenRegRoot);
								}

								//STATUS_CANNOT_DELETE 0xC0000121
								if (!NT_SUCCESS(status))
									break;
								result = true;
							} while (0);
							if (hOpenRegRoot && ZwClose) {
								ZwClose(hOpenRegRoot);
								hOpenRegRoot = nullptr;
							}
							if (load_ntdll_flag && hNTDLL)
								SK_FREE_LIBRARY(hNTDLL);
							return result;
						}

						bool Win::Registry::NtEnumKeys(const std::string & regPathOrPathname, std::set<std::string>&outKeys, const std::string & user_sid /*= ""*/) {
							bool result = false;
							outKeys.clear();
							bool load_ntdll_flag = false;
							HMODULE hNTDLL = nullptr;
							KEY_FULL_INFORMATION* pKeyFullInformation = nullptr;
							HANDLE hOpenRegRoot = nullptr;
							using tfZwClose = LONG(__stdcall*)(_In_ HANDLE);
							using tfZwOpenKey = LONG(__stdcall*)(_Out_ PHANDLE, _In_ ACCESS_MASK, _In_ POBJECT_ATTRIBUTES);
							using tfZwQueryKey = LONG(__stdcall*)(_In_ HANDLE, _In_ KEY_INFORMATION_CLASS, _In_opt_ PVOID, _In_ ULONG, _Out_ PULONG);
							using tfZwEnumerateKey = LONG(__stdcall*)(_In_ HANDLE, _In_ ULONG, _In_ KEY_INFORMATION_CLASS, _Out_opt_ PVOID, _In_ ULONG, _Out_ PULONG);

							tfZwClose ZwClose = nullptr;
							tfZwOpenKey ZwOpenKey = nullptr;
							tfZwQueryKey ZwQueryKey = nullptr;
							tfZwEnumerateKey ZwEnumerateKey = nullptr;
							do {
								hNTDLL = ::GetModuleHandleW(L"ntdll");
								if (!hNTDLL) {
									hNTDLL = ::LoadLibraryW(LR"(ntdll.dll)");
									if (hNTDLL)
										load_ntdll_flag = true;
									else
										break;
								}
								std::vector<std::wstring> wKeyPaths;
								std::wstring regPathOrPathnameW;
								if (!NtFixPathHeadW(regPathOrPathname, regPathOrPathnameW, wKeyPaths, user_sid) || regPathOrPathnameW.empty())
									break;
								ZwClose = reinterpret_cast<decltype(ZwClose)>(::GetProcAddress(hNTDLL, "ZwClose"));
								if (!ZwClose)
									break;
								ZwOpenKey = reinterpret_cast<decltype(ZwOpenKey)>(::GetProcAddress(hNTDLL, "ZwOpenKey"));
								if (!ZwOpenKey)
									break;
								ZwQueryKey = reinterpret_cast<decltype(ZwQueryKey)>(::GetProcAddress(hNTDLL, "ZwQueryKey"));
								if (!ZwQueryKey)
									break;
								ZwEnumerateKey = reinterpret_cast<decltype(ZwEnumerateKey)>(::GetProcAddress(hNTDLL, "ZwEnumerateKey"));
								if (!ZwEnumerateKey)
									break;

								long status = 0;
								ULONG ulSize = 0;
								OBJECT_ATTRIBUTES KeyPathObj = { 0 };
								UNICODE_STRING KeyPathStr = { 0 };
								InitUnicodeStringObj(regPathOrPathnameW, KeyPathStr, KeyPathObj);
								status = ZwOpenKey(&hOpenRegRoot, KEY_ALL_ACCESS, &KeyPathObj);
								if (STATUS_ACCESS_DENIED == status || !hOpenRegRoot)
									break;

								ULONG ResultLength = 0;
								ZwQueryKey(hOpenRegRoot, KEY_INFORMATION_CLASS::KeyFullInformation, nullptr, 0, &ResultLength);
								if (ResultLength <= 0)
									break;

								pKeyFullInformation = (KEY_FULL_INFORMATION*)::LocalAlloc(LPTR, ResultLength + sizeof(wchar_t));
								status = ZwQueryKey(hOpenRegRoot, KEY_INFORMATION_CLASS::KeyFullInformation, pKeyFullInformation, ResultLength, &ResultLength);
								if (!NT_SUCCESS(status) || !pKeyFullInformation)
									break;
								if (pKeyFullInformation->SubKeys == 0) {
									result = true;
									break;
								}

								for (ULONG i = 0; i < pKeyFullInformation->SubKeys; ++i) {
									ResultLength = 0;
									KEY_BASIC_INFORMATION* pKeyBasicInformation = nullptr;
									do {
										ZwEnumerateKey(hOpenRegRoot, i, KEY_INFORMATION_CLASS::KeyBasicInformation, nullptr, ResultLength, &ResultLength);
										if (ResultLength <= 0)
											break;
										pKeyBasicInformation = (KEY_BASIC_INFORMATION*)::LocalAlloc(LPTR, ResultLength + sizeof(wchar_t));
										status = ZwEnumerateKey(hOpenRegRoot, i, KEY_INFORMATION_CLASS::KeyBasicInformation, pKeyBasicInformation, ResultLength, &ResultLength);
										if (!NT_SUCCESS(status) || !pKeyBasicInformation)
											break;
										if (!pKeyBasicInformation->Name || pKeyBasicInformation->NameLength <= 0)
											break;
										outKeys.emplace(IConv::WStringToMBytes(std::wstring(pKeyBasicInformation->Name, pKeyBasicInformation->NameLength)));
									} while (0);
									if (pKeyBasicInformation) {
										::LocalFree(pKeyBasicInformation);
										pKeyBasicInformation = nullptr;
									}
								}

								result = true;
							} while (0);
							if (hOpenRegRoot && ZwClose) {
								ZwClose(hOpenRegRoot);
								hOpenRegRoot = nullptr;
							}
							if (pKeyFullInformation) {
								::LocalFree(pKeyFullInformation);
								pKeyFullInformation = nullptr;
							}
							if (load_ntdll_flag && hNTDLL)
								SK_FREE_LIBRARY(hNTDLL);
							return result;
						}
						bool Win::Registry::NtEnumKeyValues(const std::string & regPathOrPathname, std::map<std::string, std::string>&outKeyValues, const std::string & user_sid /*= ""*/) {
							bool result = false;
							outKeyValues.clear();
							bool load_ntdll_flag = false;
							HMODULE hNTDLL = nullptr;
							HANDLE hOpenRegRoot = nullptr;
							KEY_FULL_INFORMATION* pKeyFullInformation = nullptr;
							using tfZwClose = LONG(__stdcall*)(_In_ HANDLE);
							using tfZwOpenKey = LONG(__stdcall*)(_Out_ PHANDLE, _In_ ACCESS_MASK, _In_ POBJECT_ATTRIBUTES);
							using tfZwQueryKey = LONG(__stdcall*)(_In_ HANDLE, _In_ KEY_INFORMATION_CLASS, _In_opt_ PVOID, _In_ ULONG, _Out_ PULONG);
							using tfZwQueryValueKey = LONG(__stdcall*)(_In_ HANDLE, _In_ PUNICODE_STRING, _In_ KEY_VALUE_INFORMATION_CLASS, _Out_opt_ PVOID, _In_ ULONG, _Out_ PULONG);
							using tfZwEnumerateValueKey = LONG(__stdcall*) (_In_ HANDLE, _In_ ULONG, _In_ KEY_VALUE_INFORMATION_CLASS, _Out_opt_ PVOID, _In_ ULONG, _Out_ PULONG);
							tfZwClose ZwClose = nullptr;
							tfZwOpenKey ZwOpenKey = nullptr;
							tfZwQueryKey ZwQueryKey = nullptr;
							tfZwQueryValueKey ZwQueryValueKey = nullptr;
							tfZwEnumerateValueKey ZwEnumerateValueKey = nullptr;
							do {
								hNTDLL = ::GetModuleHandleW(L"ntdll");
								if (!hNTDLL) {
									hNTDLL = ::LoadLibraryW(LR"(ntdll.dll)");
									if (hNTDLL)
										load_ntdll_flag = true;
									else
										break;
								}
								std::vector<std::wstring> wKeyPaths;
								std::wstring regPathOrPathnameW;
								if (!NtFixPathHeadW(regPathOrPathname, regPathOrPathnameW, wKeyPaths, user_sid) || regPathOrPathnameW.empty())
									break;
								ZwClose = reinterpret_cast<decltype(ZwClose)>(::GetProcAddress(hNTDLL, "ZwClose"));
								if (!ZwClose)
									break;
								ZwOpenKey = reinterpret_cast<decltype(ZwOpenKey)>(::GetProcAddress(hNTDLL, "ZwOpenKey"));
								if (!ZwOpenKey)
									break;
								ZwQueryKey = reinterpret_cast<decltype(ZwQueryKey)>(::GetProcAddress(hNTDLL, "ZwQueryKey"));
								if (!ZwQueryKey)
									break;
								ZwQueryValueKey = reinterpret_cast<decltype(ZwQueryValueKey)>(::GetProcAddress(hNTDLL, "ZwQueryValueKey"));
								if (!ZwQueryValueKey)
									break;
								ZwEnumerateValueKey = reinterpret_cast<decltype(ZwEnumerateValueKey)>(::GetProcAddress(hNTDLL, "ZwEnumerateValueKey"));
								if (!ZwEnumerateValueKey)
									break;

								std::wstring KeyPathName = Win::PathToPathnameW(regPathOrPathnameW);

								long status = 0;
								ULONG ulSize = 0;
								OBJECT_ATTRIBUTES KeyPathNameObj = { 0 };
								UNICODE_STRING KeyPathNameStr = { 0 };
								InitUnicodeStringObj(KeyPathName, KeyPathNameStr, KeyPathNameObj);
								status = ZwOpenKey(&hOpenRegRoot, KEY_ALL_ACCESS, &KeyPathNameObj);
								if (STATUS_ACCESS_DENIED == status)
									break;

								ULONG ReturnLength = 0;
								ZwQueryKey(hOpenRegRoot, KEY_INFORMATION_CLASS::KeyFullInformation, nullptr, 0, &ReturnLength);
								if (ReturnLength <= 0)
									break;
								pKeyFullInformation = (KEY_FULL_INFORMATION*)::LocalAlloc(LPTR, ReturnLength);
								status = ZwQueryKey(hOpenRegRoot, KEY_INFORMATION_CLASS::KeyFullInformation, pKeyFullInformation, ReturnLength, &ReturnLength);
								if (!NT_SUCCESS(status) || !pKeyFullInformation)
									break;
								if (pKeyFullInformation->Values <= 0) {
									result = true;
									break;
								}
								for (ULONG i = 0; i < pKeyFullInformation->Values; i++) {
									ULONG ulSizeBasicInfo = 0;
									ULONG ulSizePartialInfo = 0;
									ZwEnumerateValueKey(hOpenRegRoot, i, KEY_VALUE_INFORMATION_CLASS::KeyValuePartialInformation, nullptr, 0, &ulSizePartialInfo);
									ZwEnumerateValueKey(hOpenRegRoot, i, KEY_VALUE_INFORMATION_CLASS::KeyValueBasicInformation, nullptr, 0, &ulSizeBasicInfo);
									if (ulSizePartialInfo <= 0 || ulSizeBasicInfo <= 0)
										break;
									KEY_VALUE_BASIC_INFORMATION* pKeyValueBasicInformation = (KEY_VALUE_BASIC_INFORMATION*)::LocalAlloc(LPTR, ulSizeBasicInfo + sizeof(wchar_t));
									KEY_VALUE_PARTIAL_INFORMATION* pKeyValuePartialInformation = (KEY_VALUE_PARTIAL_INFORMATION*)::LocalAlloc(LPTR, ulSizePartialInfo + sizeof(wchar_t));

									do {
										status = ZwEnumerateValueKey(hOpenRegRoot, i, KEY_VALUE_INFORMATION_CLASS::KeyValueBasicInformation, pKeyValueBasicInformation, ulSizeBasicInfo, &ulSizeBasicInfo);
										if (!NT_SUCCESS(status))
											break;
										status = ZwEnumerateValueKey(hOpenRegRoot, i, KEY_VALUE_INFORMATION_CLASS::KeyValuePartialInformation, pKeyValuePartialInformation, ulSizePartialInfo, &ulSizePartialInfo);
										if (!NT_SUCCESS(status))
											break;
										if (!pKeyValuePartialInformation || !pKeyValueBasicInformation || !pKeyValueBasicInformation->Name || pKeyValueBasicInformation->NameLength <= 0)
											break;

										std::wstring wKeyName(pKeyValueBasicInformation->Name, pKeyValueBasicInformation->NameLength);
										std::string KeyValueData;
										switch (pKeyValuePartialInformation->Type) {
										case REG_DWORD:
										case REG_QWORD:
										case REG_BINARY: {
											if (pKeyValuePartialInformation->DataLength > 0 && pKeyValuePartialInformation->Data)
												KeyValueData.append((char*)pKeyValuePartialInformation->Data, pKeyValuePartialInformation->DataLength);
											result = true;
										}break;
										case REG_MULTI_SZ:
										case REG_EXPAND_SZ:
										case REG_SZ: {
											if (pKeyValuePartialInformation->Data && pKeyValuePartialInformation->DataLength > 0) {
												std::wstring wData((wchar_t*)pKeyValuePartialInformation->Data, pKeyValuePartialInformation->DataLength);
												KeyValueData = IConv::WStringToMBytes(wData);
											}
										}break;
										default:
											break;
										}
										outKeyValues.emplace(IConv::WStringToMBytes(wKeyName), KeyValueData);
									} while (0);
									if (pKeyValueBasicInformation) {
										::LocalFree(pKeyValueBasicInformation);
										pKeyValueBasicInformation = nullptr;
									}
									if (pKeyValuePartialInformation) {
										::LocalFree(pKeyValuePartialInformation);
										pKeyValuePartialInformation = nullptr;
									}
								}
								result = true;
							} while (0);
							if (pKeyFullInformation) {
								::LocalFree(pKeyFullInformation);
								pKeyFullInformation = nullptr;
							}
							if (hOpenRegRoot && ZwClose) {
								ZwClose(hOpenRegRoot);
								hOpenRegRoot = nullptr;
							}
							if (load_ntdll_flag && hNTDLL)
								SK_FREE_LIBRARY(hNTDLL);
							return result;
						}



}///namespace shared