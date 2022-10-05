#include "stdafx.h"

namespace shared {
	namespace nt {
		NTObj::RouteObj::RouteObj(const DWORD& ProcessId, const std::string& Identify, const HANDLE& hProcess) :
			m_ProcessId(ProcessId),
			m_Identify(Identify),
			m_hProcess(hProcess) {
		}
		NTObj::RouteObj::~RouteObj() {

		}
		const DWORD& NTObj::RouteObj::ProcessId() const {
			return m_ProcessId;
		}
		const std::string& NTObj::RouteObj::Identify() const {
			return m_Identify;
		}
		HANDLE& NTObj::RouteObj::handle() {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			return m_hProcess;
		}
		bool NTObj::RouteObj::Ready() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			return m_IsOpen.load();
		}
		bool NTObj::RouteObj::Open() {
			do {
				if (m_IsOpen.load()) {
					break;
				}
				if (!m_hProcess)
					break;

				m_IsOpen.store(true);
			} while (0);
			return m_IsOpen.load();
		}

		void NTObj::RouteObj::Close() {
			if (!m_IsOpen.load())
				return;

			m_IsOpen.store(false);
		}
		NTObj::RouteObj::MemoryModule* NTObj::RouteObj::MemoryModuleCreate(
			const std::string& processIdenfity, 
			const std::string& injectorCache, 
			const std::string& bindRespak) {
			NTObj::RouteObj::MemoryModule* result = nullptr;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				if (processIdenfity.empty())
					break;
				auto found = m_pMemoryModuleObjs.find(processIdenfity);
				if (found != m_pMemoryModuleObjs.end()) {
					result = found->second.get();
					break;
				}
				auto pMemoryModuleObj = std::make_shared<NTObj::RouteObj::MemoryModule>(m_hProcess, processIdenfity, injectorCache, bindRespak);
				if (!pMemoryModuleObj->Ready())
					break;
				m_pMemoryModuleObjs.emplace(processIdenfity, pMemoryModuleObj);
				result = pMemoryModuleObj.get();
			} while (0);
			return result;
		}
		NTObj::RouteObj::MemoryModule* NTObj::RouteObj::MemoryModuleCreate(const std::string& identify, const std::string& moduleBuffer, const std::map<unsigned long, std::string>& mmInitDatas) {
			NTObj::RouteObj::MemoryModule* result = nullptr;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				if (identify.empty() || moduleBuffer.empty())
					break;
				auto found = m_pMemoryModuleObjs.find(identify);
				if (found != m_pMemoryModuleObjs.end()) {
					result = found->second.get();
					break;
				}
				auto pMemoryModuleObj = std::make_shared<NTObj::RouteObj::MemoryModule>(m_hProcess, identify, moduleBuffer, mmInitDatas);
				if (!pMemoryModuleObj->Ready())
					break;
				m_pMemoryModuleObjs.emplace(identify, pMemoryModuleObj);
				result = pMemoryModuleObj.get();
			} while (0);
			return result;
		}
		bool NTObj::RouteObj::MemoryModuleDestory(NTObj::RouteObj::MemoryModule* pMemoryModule) {
			bool result = false;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				if (!pMemoryModule)
					break;
				auto found = m_pMemoryModuleObjs.find(pMemoryModule->Identify());
				if (found == m_pMemoryModuleObjs.end())
					break;
				m_pMemoryModuleObjs.erase(found);
				result = true;
			} while (0);
			return result;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		bool NTObj::RouteObjDestory(RouteObj* pRouteObj) {
			if (!pRouteObj)
				return false;
			RouteObjDestory(pRouteObj->Identify());
			RouteObjDestory(pRouteObj->ProcessId());
			return true;
		}
		bool NTObj::RouteObjDestory(const std::string& identify) {
			bool result = false;
			std::lock_guard<std::mutex> lock{ *m_SharedMutex };
			do {
				if (identify.empty())
					break;
				m_RouteObjQ.pop(identify,
					[&](const auto&, std::shared_ptr<RouteObj>& pRouteObj) {
						pRouteObj->Close();
						SK_NTCLOSE(pRouteObj->handle());
						result = true;
					});
			} while (0);
			return result;
		}
		bool NTObj::RouteObjDestory(const DWORD& ProcessId) {
			bool result = false;
			std::lock_guard<std::mutex> lock{ *m_SharedMutex };
			do {
				m_IDRouteObjQ.pop(ProcessId,
					[&](const auto&, std::shared_ptr<RouteObj>& pRouteObj) {
						pRouteObj->Close();
						SK_NTCLOSE(pRouteObj->handle());
						result = true;
					});
			} while (0);
			return result;
		}
		NTObj::RouteObj* NTObj::RouteObjCreate(const DWORD& ProcessId) {
			NTObj::RouteObj* result = nullptr;
			std::lock_guard<std::mutex> lock{ *m_SharedMutex };
			do {
				m_RouteObjQ.iterate_const(
					[&](const auto& routeObj, auto& itbreak) {
						if (routeObj->ProcessId() == ProcessId) {
							result = routeObj.get();
							itbreak = true;
						}
					});
				if (result)
					break;
				HANDLE hProcess = __OpenPorcess(ProcessId);
				if (!hProcess)
					break;
				std::string strProcessPathname;
				strProcessPathname.resize(_MAX_PATH, 0x00);
				::GetModuleFileNameExA(hProcess, NULL, &strProcessPathname[0], DWORD(strProcessPathname.size()));
				//!@ TODO: 此处不完善
				auto pRouteObj = std::make_shared<NTObj::RouteObj>(ProcessId, shared::Win::GetNameByPathnameA(strProcessPathname), hProcess);
				m_RouteObjQ.push(shared::Win::GetNameByPathnameA(strProcessPathname), pRouteObj);
				result = pRouteObj.get();
			} while (0);
			return result;
		}
		NTObj::RouteObj* NTObj::RouteObjCreate(const HANDLE& hProcess,const DWORD& ProcessId) {
			NTObj::RouteObj* result = nullptr;
			std::lock_guard<std::mutex> lock{ *m_SharedMutex };
			do {
				if (!hProcess || ProcessId<=0)
					break;
				auto pRouteObj = std::make_shared<NTObj::RouteObj>(ProcessId, std::to_string(ProcessId), hProcess);
				m_IDRouteObjQ.push(ProcessId, pRouteObj);
				result = pRouteObj.get();
			} while (0);
			return result;
		}
		NTObj::RouteObj* NTObj::RouteObjCreate(const DWORD& ProcessId, const std::function<void(const std::string&)>& errorcb) {
			NTObj::RouteObj* result = nullptr;
			std::lock_guard<std::mutex> lock{ *m_SharedMutex };
			do {
				if (ProcessId <= 4)
					break;
				HANDLE hProcess = __OpenPorcess(ProcessId);
				if (!hProcess) {
					if (errorcb) {
						errorcb(R"(__OpenPorcess result nullptr.)");
						errorcb(std::string("ProcessId is ").append(std::to_string(ProcessId)));
						errorcb(std::string("GetLastError is ").append(std::to_string(::GetLastError())));
					}
					break;
				}
				auto pRouteObj = std::make_shared<NTObj::RouteObj>(ProcessId, std::to_string(ProcessId), hProcess);
				m_IDRouteObjQ.push(ProcessId, pRouteObj);
				result = pRouteObj.get();
			} while (0);
			return result;
		}
		NTObj::RouteObj* NTObj::RouteObjCreate(const std::string& InjectProcessImagename, const std::function<void(const std::string&)>& errorcb) {
			NTObj::RouteObj* result = nullptr;
			std::lock_guard<std::mutex> lock{ *m_SharedMutex };
			do {
				if (InjectProcessImagename.empty())
					break;
				m_RouteObjQ.search(InjectProcessImagename,
					[&](const auto&, auto& pRouteObj) {
						result = pRouteObj.get();
					});
				if (result)
					break;
				DWORD dwProcessId = 0;
				if (!__ProcessGet(InjectProcessImagename,
					[&](const auto& processes) {
						dwProcessId = processes.begin()->UniqueProcessId;
					})) {
					if (errorcb)
						errorcb(R"(__ProcessGet({}) result failed.)");
					break;
				}
				if (dwProcessId <= 0) {
					if (errorcb)
						errorcb(R"(dwProcessId <= 0)");
					break;
				}
				HANDLE hProcess = __OpenPorcess(dwProcessId, PROCESS_ALL_ACCESS);
				if (!hProcess) {
					if (errorcb) {
						errorcb(R"(__OpenPorcess result nullptr.)");
						errorcb(std::string("ProcessId is ").append(std::to_string(dwProcessId)));
						errorcb(std::string("GetLastError is ").append(std::to_string(::GetLastError())));
					}
					break;
				}
				auto pRouteObj = std::make_shared<NTObj::RouteObj>(dwProcessId, InjectProcessImagename, hProcess);
				m_RouteObjQ.push(InjectProcessImagename, pRouteObj);
				result = pRouteObj.get();
			} while (0);
			return result;
		}

	}///namespace nt


}///namespace shared