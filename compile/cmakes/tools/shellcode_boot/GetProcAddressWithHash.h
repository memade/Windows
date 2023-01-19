#include <windows.h>
#include <winternl.h>

// This compiles to a ROR instruction
// This is needed because _lrotr() is an external reference
// Also, there is not a consistent compiler intrinsic to accomplish this across all three platforms.
#define ROTR32(value, shift)	(((DWORD) value >> (BYTE) shift) | ((DWORD) value << (32 - (BYTE) shift)))

// Redefine PEB structures. The structure definitions in winternl.h are incomplete.
typedef struct _MY_PEB_LDR_DATA {
 ULONG Length;
 BOOL Initialized;
 PVOID SsHandle;
 LIST_ENTRY InLoadOrderModuleList;
 LIST_ENTRY InMemoryOrderModuleList;
 LIST_ENTRY InInitializationOrderModuleList;
} MY_PEB_LDR_DATA, * PMY_PEB_LDR_DATA;

typedef struct _MY_LDR_DATA_TABLE_ENTRY
{
 LIST_ENTRY InLoadOrderLinks;
 LIST_ENTRY InMemoryOrderLinks;
 LIST_ENTRY InInitializationOrderLinks;
 PVOID DllBase;
 PVOID EntryPoint;
 ULONG SizeOfImage;
 UNICODE_STRING FullDllName;
 UNICODE_STRING BaseDllName;
} MY_LDR_DATA_TABLE_ENTRY, * PMY_LDR_DATA_TABLE_ENTRY;

HMODULE GetProcAddressWithHash(DWORD dwModuleFunctionHash) {
 HMODULE result = NULL;
 PPEB PebAddress = NULL;
 PMY_PEB_LDR_DATA pLdr = NULL;
 PMY_LDR_DATA_TABLE_ENTRY pDataTableEntry;
 PVOID pModuleBase;
 PIMAGE_NT_HEADERS pNTHeader = NULL;
 DWORD dwExportDirRVA = 0;
 PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
 PLIST_ENTRY pNextModule = NULL;
 DWORD dwNumFunctions = 0;
 USHORT usOrdinalTableIndex = 0;
 PDWORD pdwFunctionNameBase = NULL;
 PCSTR pFunctionName = NULL;
 UNICODE_STRING BaseDllName = { 0 };
 DWORD dwModuleHash = 0;
 DWORD dwFunctionHash = 0;
 PCSTR pTempChar = NULL;

#if defined(_WIN64)
 PebAddress = (PPEB)__readgsqword(0x60);
#else
 PebAddress = (PPEB)__readfsdword(0x30);
#endif

 pLdr = (PMY_PEB_LDR_DATA)PebAddress->Ldr;
 pNextModule = pLdr->InLoadOrderModuleList.Flink;
 pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY)pNextModule;

 while (pDataTableEntry->DllBase != NULL) {
  dwModuleHash = 0;
  pModuleBase = pDataTableEntry->DllBase;
  BaseDllName = pDataTableEntry->BaseDllName;
  pNTHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)pModuleBase + ((PIMAGE_DOS_HEADER)pModuleBase)->e_lfanew);
  dwExportDirRVA = pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress;

  // Get the next loaded module entry
  pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY)pDataTableEntry->InLoadOrderLinks.Flink;

  // If the current module does not export any functions, move on to the next module.
  if (dwExportDirRVA == 0)
   continue;
  // Calculate the module hash
  for (DWORD i = 0; i < BaseDllName.MaximumLength; i++) {
   pTempChar = ((PCSTR)BaseDllName.Buffer + i);
   dwModuleHash = ROTR32(dwModuleHash, 13);
   if (*pTempChar >= 0x61)
    dwModuleHash += *pTempChar - 0x20;
   else
    dwModuleHash += *pTempChar;
  }

  pExportDir = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)pModuleBase + dwExportDirRVA);
  dwNumFunctions = pExportDir->NumberOfNames;
  pdwFunctionNameBase = (PDWORD)((PCHAR)pModuleBase + pExportDir->AddressOfNames);

  for (DWORD i = 0; i < dwNumFunctions; i++) {
   dwFunctionHash = 0;
   pFunctionName = (PCSTR)(*pdwFunctionNameBase + (ULONG_PTR)pModuleBase);
   pdwFunctionNameBase++;
   pTempChar = pFunctionName;
   do {
    dwFunctionHash = ROTR32(dwFunctionHash, 13);
    dwFunctionHash += *pTempChar;
    pTempChar++;
   } while (*(pTempChar - 1) != 0);
   dwFunctionHash += dwModuleHash;
   if (dwFunctionHash == dwModuleFunctionHash) {
    usOrdinalTableIndex = *(PUSHORT)(((ULONG_PTR)pModuleBase + pExportDir->AddressOfNameOrdinals) + (2 * i));
    result = (HMODULE)((ULONG_PTR)pModuleBase + *(PDWORD)(((ULONG_PTR)pModuleBase + pExportDir->AddressOfFunctions) + (4 * usOrdinalTableIndex)));
   }
  }
 }
 // All modules have been exhausted and the function was not found.
 return result;
}
