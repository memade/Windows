#include "stdafx.h"

namespace shared {

	std::string Win::Resource::Load(_In_ const HINSTANCE& hInstance, _In_ const DWORD& CustomRes, _In_ LPCSTR pCustomResName) {
		std::string result;
		HRSRC hRsrc = nullptr;
		do {
			hRsrc = ::FindResourceA(hInstance, MAKEINTRESOURCEA(CustomRes), pCustomResName);
			if (!hRsrc)
				break;
			size_t nSize = ::SizeofResource(hInstance, hRsrc);
			if (nSize <= 0)
				break;
			HGLOBAL hGlobal = ::LoadResource(hInstance, hRsrc);
			if (!hGlobal)
				break;
			void* pBuffer = ::LockResource(hGlobal);
			if (pBuffer) {
				result.append((char*)pBuffer, nSize);
				::GlobalUnlock(hGlobal);
			}
		} while (0);
		return result;
	}

	bool Win::Resource::UnMadeRoute(const tfRoutePak& route_data, tfRouteRes& outpak) {
		bool result = false;
		outpak.clear();
		do {
			if (route_data.empty())
				break;
			const char* begin = route_data.data();
			auto pHead = (shared::nt::PROUTEDATAHEAD)(begin);
			if (!pHead->Verify())
				break;
			unsigned long info_pos = sizeof(shared::nt::ROUTEDATAHEAD);
			for (decltype(pHead->data_count) i = 0; i < pHead->data_count; ++i) {
				auto pInfo = (shared::nt::ROUTEDATAINFO*)(begin + info_pos);
				info_pos += sizeof(shared::nt::ROUTEDATAINFO);
				outpak.emplace(pInfo->data_identify, std::string((char*)(begin + info_pos), pInfo->data_size));
				info_pos += pInfo->data_size;
			}
			result = !outpak.empty();
		} while (0);
		return result;
	}
	bool Win::Resource::MadeRoute(const tfRouteRes& ress, tfRoutePak& outres) {
		bool result = false;
		outres.clear();
		do {
			if (ress.empty())
				break;
			shared::nt::ROUTEDATAHEAD head;
			head.data_count = static_cast<decltype(head.data_count)>(ress.size());
			outres.append((char*)&head, sizeof(head));

			std::for_each(ress.begin(), ress.end(),
				[&](const auto& node) {
					shared::nt::ROUTEDATAINFO dataInfo;
					dataInfo.data_identify = node.first;
					dataInfo.data_size = static_cast<decltype(dataInfo.data_size)>(node.second.size());
					dataInfo.data_postion = static_cast<decltype(dataInfo.data_postion)>(outres.size());
					outres.append((char*)&dataInfo, sizeof(dataInfo));
					if (dataInfo.data_size > 0)
						outres.append(node.second);
					((shared::nt::ROUTEDATAHEAD*)outres.data())->data_size += dataInfo.data_size;
				});
			result = !outres.empty();
		} while (0);
		return result;
	}

}///namespace shared