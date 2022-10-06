#if !defined(INC_H___0B46E071_F544_4C1E_A449_DEDB38AA14E4__HEAD__)
#define INC_H___0B46E071_F544_4C1E_A449_DEDB38AA14E4__HEAD__

namespace shared {
	namespace injection {

#if ENABLE_SHARED_REFLECTIVE_VERSION_1_0
#ifdef __cplusplus
		extern "C" {
#endif

			__declspec(dllexport) std::uintptr_t __stdcall api_object_loader(const std::uintptr_t&);

#ifdef __cplusplus
		}
#endif
#endif///ENABLE_SHARED_REFLECTIVE_VERSION_1_0

		enum struct EnShellCodeFlag : DWORD {
			EN_SC_UNDEFINED = 0x0,
			EN_SC_CLEARHEADER = 0x1,
			EN_SC_CLEARMEMORY = EN_SC_CLEARHEADER << 1,
			EN_SC_OBFUSCATEIMPORTS = EN_SC_CLEARHEADER << 2,
		};

		static EnShellCodeFlag operator|(const EnShellCodeFlag& flag1, const EnShellCodeFlag& flag2) {
			return EnShellCodeFlag(DWORD(flag1) | DWORD(flag2));
		}
		//#if ENABLE_SHARED_REFLECTIVE_VERSION_2_0
		class Reflecive final {
		public:
			using tfModuleGet = UINT_PTR(WINAPI*)(void);
#if 0
			typedef UINT_PTR(WINAPI* RDI)();
			typedef void(WINAPI* Function)();
			typedef BOOL(__cdecl* EXPORTEDFUNCTION)(LPVOID, DWORD);
#endif
		public:
			static BOOL Is64BitDLL(UINT_PTR uiLibraryAddress);
			static DWORD HashFunctionName(LPCSTR name);
			static FARPROC GetProcAddressR(HMODULE hModule, LPCSTR lpProcName);
			//!@ GetFileContents() Have been abandoned.
			static DWORD GetFileContents(LPCSTR filename, LPSTR* data, DWORD& size);
			static BOOL GenerateShellcode(const std::string& pe_buffer, const std::string& init_function, const std::string& user_data, const EnShellCodeFlag& flags, std::string& shellcode);
			static BOOL GenerateShellcode(LPVOID inBytes, DWORD length, DWORD userFunction, LPVOID userData, DWORD userLength, const EnShellCodeFlag& flags, LPSTR& outBytes, DWORD& outLength);
		};
		//#endif///ENABLE_SHARED_REFLECTIVE_VERSION_2_0


	}///namespace injection
}///namespace shared

#pragma comment(lib,"reflective")
/// /*新生®（上海）**/
/// /*2022_02_19T14:00:59.9716484Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___0B46E071_F544_4C1E_A449_DEDB38AA14E4__HEAD__

