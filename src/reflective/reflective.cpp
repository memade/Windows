#include "stdafx.h"

namespace shared {
#if ENABLE_SHARED_REFLECTIVE_VERSION_1_0
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef _WIN64
#pragma warning(disable: 4244 4312)
#elif _WIN32
#pragma warning(disable: 4244)
#endif

#if 0
		template<typename T = UINT_PTR>
		static T TDEREF(T in) { return *(T*)(in); }
#endif

#define DEREF(name) *(UINT_PTR *)(name)
#define DEREF_64(name) *(DWORD64 *)(name)
#define DEREF_32(name) *(DWORD *)(name)
#define DEREF_16(name) *(WORD *)(name)
#define DEREF_8(name) *(BYTE *)(name)

		using tfLoadLibraryA = HMODULE(WINAPI*)(LPCSTR);
		using tfGetProcAddress = FARPROC(WINAPI*)(HMODULE, LPCSTR);
		using tfVirtualAlloc = LPVOID(WINAPI*)(LPVOID, SIZE_T, DWORD, DWORD);
		using tfNtFlushInstructionCache = DWORD(NTAPI*)(HANDLE, PVOID, ULONG);
		using tfDllMain = BOOL(WINAPI*)(HINSTANCE, DWORD, LPVOID);

		const DWORD HASH_KERNEL32DLL = 0x6A4ABC5B;
		const DWORD HASH_NTDLLDLL = 0x3CFA685D;
		const DWORD HASH_LOADLIBRARYA = 0xEC0E4E8E;
		const DWORD HASH_GETPROCADDRESS = 0x7C0DFCAA;
		const DWORD HASH_VIRTUALALLOC = 0x91AFCA54;
		const DWORD HASH_NTFLUSHINSTRUCTIONCACHE = 0x534C0AB8;

		const DWORD HASH_KEY = 13;
#pragma intrinsic( _rotr )
		__forceinline DWORD ror(DWORD d) {
			return _rotr(d, HASH_KEY);
		}
		__forceinline DWORD hash(char* c) {
			DWORD h = 0;
			do {
				h = ror(h);
				h += *c;
			} while (*++c);
			return h;
		}

#pragma intrinsic(_ReturnAddress)
		__declspec(noinline)
			ULONG_PTR caller(VOID) {
			return (ULONG_PTR)_ReturnAddress();
		}

		__declspec(dllexport) std::uintptr_t __stdcall api_object_loader(const std::uintptr_t& INPtr) {
			std::uintptr_t result = 0;
			do {
				tfLoadLibraryA pLoadLibraryA = nullptr;
				tfGetProcAddress pGetProcAddress = nullptr;
				tfVirtualAlloc pVirtualAlloc = nullptr;
				tfNtFlushInstructionCache pNtFlushInstructionCache = nullptr;

				USHORT usCounter = 0;
				ULONG_PTR uiLibraryAddress = 0;
				ULONG_PTR uiBaseAddress = 0;
				ULONG_PTR uiAddressArray = 0;
				ULONG_PTR uiNameArray = 0;
				ULONG_PTR uiExportDir = 0;
				ULONG_PTR uiNameOrdinals = 0;
				DWORD dwHashValue = 0;
				ULONG_PTR uiHeaderValue = 0;
				ULONG_PTR uiValueA = 0;
				ULONG_PTR uiValueB = 0;
				ULONG_PTR uiValueC = 0;
				ULONG_PTR uiValueD = 0;
				ULONG_PTR uiValueE = 0;

				// 第一步：计算镜像当前基地址
				uiLibraryAddress = caller();
				while (1) {
					if (((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_magic == IMAGE_DOS_SIGNATURE) {
						uiHeaderValue = ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;
						if (uiHeaderValue >= sizeof(IMAGE_DOS_HEADER) && uiHeaderValue < 1024) {
							uiHeaderValue += uiLibraryAddress;
							if (((PIMAGE_NT_HEADERS)uiHeaderValue)->Signature == IMAGE_NT_SIGNATURE)
								break;
						}
					}
					uiLibraryAddress--;
				}

				// STEP 1: process the kernels exports for the functions our loader needs...
				// get the Process Enviroment Block
#if defined (_WIN64)
				uiBaseAddress = __readgsqword(0x60);
#else
				uiBaseAddress = __readfsdword(0x30);
#endif
				// get the processes loaded modules. ref: http://msdn.microsoft.com/en-us/library/aa813708(VS.85).aspx
				uiBaseAddress = (ULONG_PTR)((shared::PPEB)uiBaseAddress)->Ldr;
				// get the first entry of the InMemoryOrder module list
				uiValueA = (ULONG_PTR)((PPEB_LDR_DATA)uiBaseAddress)->InMemoryOrderModuleList.Flink;
				while (uiValueA) {
					// get pointer to current modules name (unicode string)
					uiValueB = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY_FOR_PELOADER)uiValueA)->BaseDllName.Buffer;
					// set bCounter to the length for the loop
					usCounter = ((PLDR_DATA_TABLE_ENTRY_FOR_PELOADER)uiValueA)->BaseDllName.Length;
					uiValueC = 0;
					do {// compute the hash of the module name...
						uiValueC = ror((DWORD)uiValueC);
						if (*((BYTE*)uiValueB) >= 'a')
							uiValueC += *((BYTE*)uiValueB) - 0x20;
						else
							uiValueC += *((BYTE*)uiValueB);
						uiValueB++;
					} while (--usCounter);
					// compare the hash with that of kernel32.dll
					if ((DWORD)uiValueC == HASH_KERNEL32DLL) {
						// get this modules base address
						uiBaseAddress = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY_FOR_PELOADER)uiValueA)->DllBase;
						// get the VA of the modules NT Header
						uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;
						// uiNameArray = the address of the modules export directory entry
						uiNameArray = (ULONG_PTR) & ((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
						// get the VA of the export directory
						uiExportDir = (uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress);
						// get the VA for the array of name pointers
						uiNameArray = (uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNames);
						// get the VA for the array of name ordinals
						uiNameOrdinals = (uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNameOrdinals);
						usCounter = 3;
						while (usCounter > 0) {// loop while we still have imports to find
							// compute the hash values for this function name
							dwHashValue = hash((char*)(uiBaseAddress + DEREF_32(uiNameArray)));
							// if we have found a function we want we get its virtual address
							if (dwHashValue == HASH_LOADLIBRARYA || dwHashValue == HASH_GETPROCADDRESS || dwHashValue == HASH_VIRTUALALLOC) {
								// get the VA for the array of addresses
								uiAddressArray = (uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions);
								// use this functions name ordinal as an index into the array of name pointers
								uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));
								// store this functions VA
								if (dwHashValue == HASH_LOADLIBRARYA)
									pLoadLibraryA = (tfLoadLibraryA)(uiBaseAddress + DEREF_32(uiAddressArray));
								else if (dwHashValue == HASH_GETPROCADDRESS)
									pGetProcAddress = (tfGetProcAddress)(uiBaseAddress + DEREF_32(uiAddressArray));
								else if (dwHashValue == HASH_VIRTUALALLOC)
									pVirtualAlloc = (tfVirtualAlloc)(uiBaseAddress + DEREF_32(uiAddressArray));
								usCounter--;// decrement our counter
							}
							// get the next exported function name
							uiNameArray += sizeof(DWORD);
							// get the next exported function name ordinal
							uiNameOrdinals += sizeof(WORD);
						}
					}
					else if ((DWORD)uiValueC == HASH_NTDLLDLL) {
						// get this modules base address
						uiBaseAddress = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY_FOR_PELOADER)uiValueA)->DllBase;
						// get the VA of the modules NT Header
						uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;
						// uiNameArray = the address of the modules export directory entry
						uiNameArray = (ULONG_PTR) & ((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
						// get the VA of the export directory
						uiExportDir = (uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress);
						// get the VA for the array of name pointers
						uiNameArray = (uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNames);
						// get the VA for the array of name ordinals
						uiNameOrdinals = (uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNameOrdinals);
						usCounter = 1;
						while (usCounter > 0) {// loop while we still have imports to find
							// compute the hash values for this function name
							dwHashValue = hash((char*)(uiBaseAddress + DEREF_32(uiNameArray)));
							// if we have found a function we want we get its virtual address
							if (dwHashValue == HASH_NTFLUSHINSTRUCTIONCACHE) {
								// get the VA for the array of addresses
								uiAddressArray = (uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions);
								// use this functions name ordinal as an index into the array of name pointers
								uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));
								// store this functions VA
								if (dwHashValue == HASH_NTFLUSHINSTRUCTIONCACHE)
									pNtFlushInstructionCache = (tfNtFlushInstructionCache)(uiBaseAddress + DEREF_32(uiAddressArray));
								// decrement our counter
								usCounter--;
							}
							// get the next exported function name
							uiNameArray += sizeof(DWORD);
							// get the next exported function name ordinal
							uiNameOrdinals += sizeof(WORD);
						}
					}
					// we stop searching when we have found everything we need.
					if (pLoadLibraryA && pGetProcAddress && pVirtualAlloc && pNtFlushInstructionCache)
						break;
					// get the next entry
					uiValueA = DEREF(uiValueA);
				}//end while

				if (!pLoadLibraryA || !pGetProcAddress || !pVirtualAlloc || !pNtFlushInstructionCache)
					break;


				// STEP 2: load our image into a new permanent location in memory...
				// 
				// get the VA of the NT Header for the PE to be loaded
				uiHeaderValue = uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;
				// allocate all the memory for the DLL to be loaded into. we can load at any address because we will  
				// relocate the image. Also zeros all memory and marks it as READ, WRITE and EXECUTE to avoid any problems.
				uiBaseAddress = (ULONG_PTR)pVirtualAlloc(NULL,
					((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.SizeOfImage,
					MEM_RESERVE | MEM_COMMIT,
					PAGE_EXECUTE_READWRITE);

				// we must now copy over the headers
				uiValueA = ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.SizeOfHeaders;
				uiValueB = uiLibraryAddress;
				uiValueC = uiBaseAddress;
				while (uiValueA--)
					*(BYTE*)uiValueC++ = *(BYTE*)uiValueB++;

				// STEP 3: load in all of our sections...

				// uiValueA = the VA of the first section
				uiValueA = ((ULONG_PTR) & ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader + \
					((PIMAGE_NT_HEADERS)uiHeaderValue)->FileHeader.SizeOfOptionalHeader);
				// itterate through all sections, loading them into memory.
				uiValueE = ((PIMAGE_NT_HEADERS)uiHeaderValue)->FileHeader.NumberOfSections;
				while (uiValueE--) {
					// uiValueB is the VA for this section
					uiValueB = (uiBaseAddress + ((PIMAGE_SECTION_HEADER)uiValueA)->VirtualAddress);
					// uiValueC if the VA for this sections data
					uiValueC = (uiLibraryAddress + ((PIMAGE_SECTION_HEADER)uiValueA)->PointerToRawData);
					// copy the section over
					uiValueD = ((PIMAGE_SECTION_HEADER)uiValueA)->SizeOfRawData;
					while (uiValueD--)
						*(BYTE*)uiValueB++ = *(BYTE*)uiValueC++;
					// get the VA of the next section
					uiValueA += sizeof(IMAGE_SECTION_HEADER);
				}

				// STEP 4: process our images import table...

				// uiValueB = the address of the import directory
				uiValueB = (ULONG_PTR) & ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
				// we assume their is an import table to process
				// uiValueC is the first entry in the import table
				uiValueC = (uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiValueB)->VirtualAddress);

				// itterate through all imports
				while (((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->Name) {
					// use LoadLibraryA to load the imported module into memory
					uiLibraryAddress = (ULONG_PTR)pLoadLibraryA((LPCSTR)(uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->Name));
					// uiValueD = VA of the OriginalFirstThunk
					uiValueD = (uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->OriginalFirstThunk);
					// uiValueA = VA of the IAT (via first thunk not origionalfirstthunk)
					uiValueA = (uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->FirstThunk);
					// itterate through all imported functions, importing by ordinal if no name present
					while (DEREF(uiValueA)) {
						// sanity check uiValueD as some compilers only import by FirstThunk
						if (uiValueD && ((PIMAGE_THUNK_DATA)uiValueD)->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
							// get the VA of the modules NT Header
							uiExportDir = uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;
							// uiNameArray = the address of the modules export directory entry
							uiNameArray = (ULONG_PTR) & ((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
							// get the VA of the export directory
							uiExportDir = (uiLibraryAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress);
							// get the VA for the array of addresses
							uiAddressArray = (uiLibraryAddress + ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions);
							// use the import ordinal (- export ordinal base) as an index into the array of addresses
							uiAddressArray += ((IMAGE_ORDINAL(((PIMAGE_THUNK_DATA)uiValueD)->u1.Ordinal) - ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->Base) * sizeof(DWORD));
							// patch in the address for this imported function
							*(UINT_PTR*)(uiValueA) = (uiLibraryAddress + DEREF_32(uiAddressArray));
						}
						else {
							// get the VA of this functions import by name struct
							uiValueB = (uiBaseAddress + DEREF(uiValueA));
							// use GetProcAddress and patch in the address for this imported function
							*(UINT_PTR*)(uiValueA) = (ULONG_PTR)pGetProcAddress((HMODULE)uiLibraryAddress, (LPCSTR)((PIMAGE_IMPORT_BY_NAME)uiValueB)->Name);
						}
						// get the next imported function
						uiValueA += sizeof(ULONG_PTR);
						if (uiValueD)//INT
							uiValueD += sizeof(ULONG_PTR);
					}
					// get the next import
					uiValueC += sizeof(IMAGE_IMPORT_DESCRIPTOR);
				}

				// STEP 5: process all of our images relocations...
				//
				// calculate the base address delta and perform relocations (even if we load at desired image base)
				uiLibraryAddress = uiBaseAddress - ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.ImageBase;
				// uiValueB = the address of the relocation directory
				uiValueB = (ULONG_PTR) & ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
				// check if their are any relocations present
				if (((PIMAGE_DATA_DIRECTORY)uiValueB)->Size) {
					// uiValueC is now the first entry (IMAGE_BASE_RELOCATION)
					uiValueC = (uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiValueB)->VirtualAddress);
					// and we itterate through all entries...
					while (((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock) {
						// uiValueA = the VA for this relocation block
						uiValueA = (uiBaseAddress + ((PIMAGE_BASE_RELOCATION)uiValueC)->VirtualAddress);
						// uiValueB = number of entries in this relocation block
						uiValueB = (((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_RELOC);
						// uiValueD is now the first entry in the current relocation block
						uiValueD = uiValueC + sizeof(IMAGE_BASE_RELOCATION);
						// we itterate through all the entries in the current block...
						while (uiValueB--) {
							// perform the relocation, skipping IMAGE_REL_BASED_ABSOLUTE as required.
							// we dont use a switch statement to avoid the compiler building a jump table
							// which would not be very position independent!
							if (((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_DIR64)
								*(ULONG_PTR*)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += uiLibraryAddress;
							else if (((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_HIGHLOW)
								*(DWORD*)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += (DWORD)uiLibraryAddress;
							else if (((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_HIGH)
								*(WORD*)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += HIWORD(uiLibraryAddress);
							else if (((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_LOW)
								*(WORD*)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += LOWORD(uiLibraryAddress);
							// get the next entry in the current relocation block
							uiValueD += sizeof(IMAGE_RELOC);
						}
						// get the next entry in the relocation directory
						uiValueC = uiValueC + ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock;
					}
				}
				// STEP 6: call our images entry point
				// uiValueA = the VA of our newly loaded DLL/EXE's entry point
				uiValueA = (uiBaseAddress + ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.AddressOfEntryPoint);
				// We must flush the instruction cache to avoid stale code being used which was updated by our relocation processing.
				pNtFlushInstructionCache((HANDLE)-1, NULL, 0);
				((tfDllMain)uiValueA)((HINSTANCE)uiBaseAddress, DLL_PROCESS_ATTACH, NULL);
				// STEP 8: return our new entry point address so whatever called us can call DllMain() if needed.
				result = uiValueA;
			} while (0);
			return result;
		}


#ifdef __cplusplus
	}
#endif
#endif///ENABLE_SHARED_REFLECTIVE_VERSION_1_0






}///namespace shared