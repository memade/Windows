#include "stdafx.h"

namespace shared {
	namespace nt {
		DWORD NTObj::RVAToOffset(const DWORD& dwRva, const UINT_PTR& uiBaseAddress) {
			DWORD result = 0;
			do {
				//得到nt头在内存中的实际地址
				PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew);
				if (!pNtHeaders)
					break;
				//获得节表
				PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((UINT_PTR)(&pNtHeaders->OptionalHeader) + pNtHeaders->FileHeader.SizeOfOptionalHeader);
				if (!pSectionHeader)
					break;
				//不在任意块内
				if (dwRva < pSectionHeader[0].PointerToRawData) {
					result = dwRva;
					break;
				}
				//通过遍历块，来找到相对偏移地址对应的文件偏移地址
				for (WORD wIndex = 0; wIndex < pNtHeaders->FileHeader.NumberOfSections; wIndex++) {
					if (dwRva >= pSectionHeader[wIndex].VirtualAddress && dwRva < (pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].SizeOfRawData)) {
						result = (dwRva - pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].PointerToRawData);
						break;
					}
					//		\------------------块内偏移-------------------/	\-----------块在文件中的偏移------------/
				}
			} while (0);
			return result;
		}

		bool NTObj::EnumImageExportDirectory(const void* pImageBuffer, std::map<std::string, DWORD>& outExportDirectory) {
			bool result = false;
			outExportDirectory.clear();
			do {
				//基址->在Dropper进程中开辟的堆空间的起始地址
				UINT_PTR uiBaseAddress = (UINT_PTR)pImageBuffer;
				if (!uiBaseAddress)
					break;
				//得到NT头的文件地址
				UINT_PTR uiExportDir = (UINT_PTR)uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;
				if (!uiExportDir)
					break;
				//获得导出表结构体指针的地址
				UINT_PTR uiNameArray = (UINT_PTR) & (((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
				if (!uiNameArray)
					break;
				//该调用中，第一个参数即为导出表结构体映射到内存的相对虚拟地址
				//结果为找到到导出表结构体的内存地址
				uiExportDir = uiBaseAddress + RVAToOffset(((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress, uiBaseAddress);
				if (!uiExportDir)
					break;
				//得到导出表名称数组在内存中的地址RVA
				uiNameArray = uiBaseAddress + RVAToOffset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNames, uiBaseAddress);
				if (!uiNameArray)
					break;
				//得到导出函数地址表在内存中的地址RVA
				UINT_PTR uiAddressArray = uiBaseAddress + RVAToOffset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
				if (!uiAddressArray)
					break;
				//得到函数序号地址表在内存中的地址
				UINT_PTR uiNameOrdinals = uiBaseAddress + RVAToOffset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNameOrdinals, uiBaseAddress);
				if (!uiNameOrdinals)
					break;
				//导出函数的数量
				DWORD dwCounter = ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->NumberOfNames;
				if (dwCounter <= 0)
					break;
				while (dwCounter--) {
					do {
						//这里需要将获取到的各表的RVA转化为各表实际的文件偏移
						char* FunctionName = (char*)(uiBaseAddress + RVAToOffset((*(DWORD*)uiNameArray), uiBaseAddress));
						if (!FunctionName)
							break;
						//获取地址表起始地址的实际位置
						uiAddressArray = uiBaseAddress + RVAToOffset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
						//根据序号找到序号对应的函数地址
						uiAddressArray += (*(WORD*)(uiNameOrdinals) * sizeof(DWORD));
						// 返回ReflectiveLoader函数的文件偏移，即函数机器码的起始地址
						DWORD FunctionAddress = RVAToOffset((*(DWORD*)uiAddressArray), uiBaseAddress);
						if (!FunctionAddress)
							break;
						outExportDirectory.emplace(std::make_pair(FunctionName, FunctionAddress));
#if 0
						if (::strstr(cpExportedFunctionName, "ReflectiveLoader") != nullptr) {
							//获取地址表起始地址的实际位置
							uiAddressArray = uiBaseAddress + RVAToOffset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
							//根据序号找到序号对应的函数地址
							uiAddressArray += (*(WORD*)(uiNameOrdinals) * sizeof(DWORD));
							// 返回ReflectiveLoader函数的文件偏移，即函数机器码的起始地址
							return RVAToOffset((*(DWORD*)uiAddressArray), uiBaseAddress);
						}
#endif
					} while (0);
					uiNameArray += sizeof(DWORD);
					uiNameOrdinals += sizeof(WORD);
				}
				result = !outExportDirectory.empty();
			} while (0);
			return result;
		}


	}///namespace nt
}///namespace shared