#if !defined(AFX_SK_H__22B9E599_9685_40F1_8EFE_FA517F90C840__HEAD__)
#define AFX_SK_H__22B9E599_9685_40F1_8EFE_FA517F90C840__HEAD__

#include "md5.hpp"

namespace shared {

#pragma pack(push,1)
	typedef struct tagMAPFILEHEAD {
		struct Reserve {
			long long Reservell[10];
			unsigned long long Reserveull[10];
			char ReservellCh[5][256];
		};
		struct Cache {
			char buffer[2049];
		};

		unsigned long long		head;
		unsigned long size_data;
		unsigned long size_total;
		unsigned long size_head;
		long long time_create;
		long long time_update;
		char shared[64];
		char identify[64];
		union {
			Cache cache;
			Reserve reserve;
		};
		unsigned long long		tail;
		char view[1];

		tagMAPFILEHEAD() { Default(); }
		bool Verify() const {
			return head == 0xFAC9C2D0 && tail == 0xB4B4AAC1;
		}
		void Default() { Reset(); }
		void operator=(const tagMAPFILEHEAD& obj) {
			::memcpy(this, &obj, sizeof(*this));
		}
		bool operator<(const tagMAPFILEHEAD& obj) const {
			return ::memcmp(this, &obj, sizeof(*this)) < 0;
		}
		bool operator>(const tagMAPFILEHEAD& obj) const {
			return ::memcmp(this, &obj, sizeof(*this)) > 0;
		}
		bool operator==(const tagMAPFILEHEAD& obj) const {
			return ::memcmp(this, &obj, sizeof(*this)) == 0;
		}
		bool operator!=(const tagMAPFILEHEAD& obj) const {
			return ::memcmp(this, &obj, sizeof(*this)) != 0;
		}
	private:
		void Reset() {
			::memset(this, 0x00, sizeof(*this));
			head = 0xFAC9C2D0;
			tail = 0xB4B4AAC1;
			::memcpy(identify, "SK", 2);
			size_head = sizeof(*this);
			size_total = size_head;
			size_data = 0;
			time_create = ::time(0);
			time_update = time_create;
		}
	}MAPFILEHEAD;
	const size_t LENMAPFILEHEAD = sizeof(MAPFILEHEAD);
#pragma pack(pop)

	class MapFileEx final {
		std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
	public:
		MapFileEx(const std::string& pathname, const std::string& shared = "") :
			m_FilePathname(pathname),
			m_Shared(shared) {
			Init();
		}
		MapFileEx(const std::string& pathname, const std::uint64_t& initdatasize, const std::string& shared = "") :
			m_FilePathname(pathname),
			m_Shared(shared),
			m_InitDataSize(initdatasize) {
			Init();
		}
		~MapFileEx() {
			UnInit();
		}
	private:
		bool Init() {
			if (m_FilePathname.empty()) return false;
			if (m_Ready.load()) return true;
			::GetSystemInfo(&m_SystemInfo);
			bool result = true;
			do {
				m_hFileObj = ::CreateFileA(m_FilePathname.c_str(),
					GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
				if (INVALID_HANDLE_VALUE == m_hFileObj || NULL == m_hFileObj) {
					result = false;
					break;
				}
				if (::SetFilePointer(m_hFileObj, 0, NULL, FILE_BEGIN) == HFILE_ERROR) {
					result = false;
					break;
				}
				DWORD dwReadFileSize = 0;
				MAPFILEHEAD head;
				if (!::ReadFile(m_hFileObj, &head, sizeof(MAPFILEHEAD), &dwReadFileSize, NULL)) {
					result = false;
					break;
				}
				shared::MD5 md5;
				if (!head.Verify())
					head.Default();
				head.size_data = static_cast<decltype(head.size_data)>(m_InitDataSize);
				head.size_total += head.size_data;
				if (!head.shared[0]) {
					if (m_Shared.empty()) {
						m_Shared = md5.Encode(std::string((char*)&head.time_create, sizeof(head.time_create)));
					}
					::memcpy(head.shared, m_Shared.c_str(), __min(sizeof(MAPFILEHEAD), m_Shared.size()));
				}

				m_Identify = md5.Encode(m_FilePathname);
				if (head.size_total <= m_SystemInfo.dwAllocationGranularity)
					m_MappingFileSize = m_SystemInfo.dwAllocationGranularity;
				else
					m_MappingFileSize = (static_cast<size_t>(head.size_total / m_SystemInfo.dwAllocationGranularity) + 1) * m_SystemInfo.dwAllocationGranularity;
				m_hFileMappingObj = ::CreateFileMappingA(m_hFileObj, NULL,
					PAGE_READWRITE, 0, static_cast<DWORD>(m_MappingFileSize), (LPCSTR)head.shared);
				if (INVALID_HANDLE_VALUE == m_hFileMappingObj || NULL == m_hFileMappingObj) {
					result = false;
					break;
				}
				m_pMappingBaseAddress = ::MapViewOfFile(
					m_hFileMappingObj,
					FILE_MAP_ALL_ACCESS,
					static_cast<DWORD>((static_cast<long long>(head.size_total) >> 32)),
					/*(DWORD)(m_MappingFileSize & 0xFFFFFFFF)*/0,
					static_cast<DWORD>(m_MappingFileSize));
				if (NULL == m_pMappingBaseAddress || INVALID_HANDLE_VALUE == m_pMappingBaseAddress) {
					result = false;
					break;
				}
				::memcpy(m_pMappingBaseAddress, &head, LENMAPFILEHEAD);
				::FlushViewOfFile(m_pMappingBaseAddress, static_cast<DWORD>(head.size_total));
				m_FileObjSize = ::GetFileSize(m_hFileObj, NULL);
			} while (0);
			if (!result) {
				SK_CLOSE_HANDLE(m_pMappingBaseAddress);
				SK_CLOSE_HANDLE(m_hFileMappingObj);
				SK_CLOSE_HANDLE(m_hFileObj);
			}
			m_Ready.store(result);
			return m_Ready.load();
		}
		void UnInit() {
			if (!m_Ready.load()) return;
			bool result = true;
			HANDLE hTempObjectFile = nullptr;
			HANDLE hTempObjectFileMapping = nullptr;
			do {
				if (!m_pMappingBaseAddress)
					break;
				MAPFILEHEAD head;
				::memcpy(&head, m_pMappingBaseAddress, LENMAPFILEHEAD);
				::FlushViewOfFile(m_pMappingBaseAddress, static_cast<DWORD>(head.size_total));
				if (!::UnmapViewOfFile(m_pMappingBaseAddress)) {
					result = false;
					break;
				}
				SK_CLOSE_HANDLE(m_hFileMappingObj);
				SK_CLOSE_HANDLE(m_hFileObj);

				hTempObjectFile = ::CreateFileA(
					m_FilePathname.c_str(), GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
					FILE_FLAG_SEQUENTIAL_SCAN, NULL);
				if (INVALID_HANDLE_VALUE == hTempObjectFile || NULL == hTempObjectFile)
					break;
				hTempObjectFileMapping = ::CreateFileMappingA(
					hTempObjectFile,
					NULL,
					PAGE_READWRITE,
					0,
					static_cast<DWORD>((static_cast<long long>(head.size_total) & 0xFFFFFFFF)),
					NULL);
				if (INVALID_HANDLE_VALUE == hTempObjectFileMapping || NULL == hTempObjectFileMapping) {
					SK_CLOSE_HANDLE(hTempObjectFile);
					break;
				}
				SK_CLOSE_HANDLE(hTempObjectFile);
				LPVOID pTempMappingBaseAddress = ::MapViewOfFile(
					hTempObjectFileMapping,
					FILE_MAP_ALL_ACCESS,
					0,
					0,
					(SIZE_T)head.size_total);

				if (NULL == pTempMappingBaseAddress) {
					SK_CLOSE_HANDLE(hTempObjectFileMapping);
					break;
				}
				if (!::UnmapViewOfFile(pTempMappingBaseAddress)) {
					SK_CLOSE_HANDLE(hTempObjectFileMapping);
					break;
				}
				SK_CLOSE_HANDLE(hTempObjectFileMapping);
			} while (0);
			SK_CLOSE_HANDLE(hTempObjectFileMapping);
			SK_CLOSE_HANDLE(hTempObjectFile);
			m_Ready.store(false);
		}
		bool AllocationGranularity(const size_t& current_data_total_size) {
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return false;
			const MAPFILEHEAD* pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			size_t AllocationGranularitySize = 0;
			if (current_data_total_size <= m_SystemInfo.dwAllocationGranularity)
				AllocationGranularitySize = m_SystemInfo.dwAllocationGranularity;
			else
				AllocationGranularitySize = (static_cast<size_t>(current_data_total_size / m_SystemInfo.dwAllocationGranularity) + 1) * m_SystemInfo.dwAllocationGranularity;
			::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total);
			SK_CLOSE_HANDLE(m_hFileMappingObj);
			::UnmapViewOfFile(m_pMappingBaseAddress);
			m_pMappingBaseAddress = nullptr;
			SK_CLOSE_HANDLE(m_hFileObj);
			m_hFileObj = ::CreateFileA(
				m_FilePathname.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_FLAG_SEQUENTIAL_SCAN,
				NULL);
			if (INVALID_HANDLE_VALUE == m_hFileObj || NULL == m_hFileObj)
				return false;
			m_MappingFileSize += static_cast<decltype(m_MappingFileSize)>(AllocationGranularitySize);
			m_hFileMappingObj = ::CreateFileMappingA(
				m_hFileObj,
				NULL,
				PAGE_READWRITE,
				0,
				m_MappingFileSize,
				(LPCSTR)m_Shared.c_str());
			if (INVALID_HANDLE_VALUE == m_hFileMappingObj || NULL == m_hFileMappingObj) {
				SK_CLOSE_HANDLE(m_hFileObj);
				return false;
			}
			m_pMappingBaseAddress = ::MapViewOfFile(
				m_hFileMappingObj,
				FILE_MAP_ALL_ACCESS,
				static_cast<DWORD>((static_cast<long long>(current_data_total_size) >> 32)),
				/*(DWORD)(m_MappingFileSize & 0xFFFFFFFF)*/0,
				static_cast<DWORD>(m_MappingFileSize));
			if (!m_pMappingBaseAddress) {
				SK_CLOSE_HANDLE(m_hFileObj);
				SK_CLOSE_HANDLE(m_hFileMappingObj);
				return false;
			}
			m_FileObjSize = ::GetFileSize(m_hFileObj, NULL);
			return true;
		}
	public:
		bool Ready() const {
			return m_Ready.load();
		}
		const std::string& FilePathname() const {
			return m_FilePathname;
		}
		bool Write(const std::string& data, const bool& flush = true) {
			if (data.empty())
				return false;
			return Write(data.data(), data.size());
		}
		bool Write(const std::string& data, const size_t& offset, const bool& flush = true) {
			if (data.empty())
				return false;
			return Write(data.data(), data.size(), offset);
		}
		bool Write(const char* data, const size_t& size, const bool& flush = true) {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_Ready.load()) return false;
			if (!data || size <= 0) return false;
			if (!m_pMappingBaseAddress)
				return false;

			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (m_MappingFileSize < (size + pHead->size_total))
				if (!AllocationGranularity(size + pHead->size_total))
					return false;
			pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			::memcpy((char*)m_pMappingBaseAddress + pHead->size_total, data, size);
			pHead->size_total += static_cast<decltype(pHead->size_total)>(size);
			pHead->size_data += static_cast<decltype(pHead->size_data)>(size);
			pHead->time_update = ::time(NULL);

			if (flush)
				return ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total) != 0;
			return true;
		}
		bool Write(const char* data, const size_t& size, const size_t& offset, const bool& flush = true) {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_Ready.load()) return false;
			if (!data || size <= 0) return false;
			if (!m_pMappingBaseAddress)
				return false;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (m_MappingFileSize < (size + offset + LENMAPFILEHEAD))
				if (!AllocationGranularity(size + offset + LENMAPFILEHEAD))
					return false;
			pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			::memcpy((char*)m_pMappingBaseAddress + LENMAPFILEHEAD + offset, data, size);
			pHead->size_total = __max(pHead->size_total, static_cast<decltype(pHead->size_total)>(LENMAPFILEHEAD + offset + size));
			pHead->size_data = __max(pHead->size_data, static_cast<decltype(pHead->size_data)>(offset + size));
			pHead->time_update = ::time(NULL);
			if (flush)
				return ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total) != 0;
			return true;
		}
		const char* Base() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return nullptr;
			return reinterpret_cast<const char*>(m_pMappingBaseAddress);
		}
		bool MoveHeadToEOF(const bool& flush = true) {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return false;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (!pHead->Verify())
				return false;
			if (pHead->size_data <= 0)
				return false;
			MAPFILEHEAD head;
			::memcpy(&head, m_pMappingBaseAddress, LENMAPFILEHEAD);
			::memmove((char*)m_pMappingBaseAddress, (char*)m_pMappingBaseAddress + LENMAPFILEHEAD, head.size_data);
			::memcpy((char*)m_pMappingBaseAddress + head.size_data, &head, LENMAPFILEHEAD);
			m_RememberDataSize = head.size_data;
			if (flush)
				return ::FlushViewOfFile(m_pMappingBaseAddress, pHead->size_total + LENMAPFILEHEAD) != 0;
			return true;
		}
		MAPFILEHEAD* HeadFromEOF() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (m_RememberDataSize <= 0 || m_FileObjSize <= 0)
				return nullptr;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>((char*)m_pMappingBaseAddress + m_RememberDataSize);
			if (!pHead->Verify())
				return nullptr;
			return pHead;
		}
		std::string PathFixed(const std::string& path) const {
			std::string result{ path };
			if (path.empty())
				return result;
			for (auto it = result.begin(); it != result.end(); ++it) {
				if (*it == '/')
					*it = '\\';
			}
			do {
				auto found = result.find("\\\\");
				if (found == std::string::npos)
					break;
				result.replace(found, ::strlen("\\\\"), "\\");
			} while (1);
			return result;
		}
		std::string FinishGenerate(const std::string& generate_filename = "", const bool& flush = true) {
			std::string result;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return "";
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (!pHead->Verify())
				return "";
			if (pHead->size_data <= 0)
				return "";
			MAPFILEHEAD head;
			::memcpy(&head, m_pMappingBaseAddress, LENMAPFILEHEAD);
			m_RememberDataSize = head.size_data;
			::memmove((char*)m_pMappingBaseAddress, (char*)m_pMappingBaseAddress + LENMAPFILEHEAD, head.size_data);
			if (flush)
				::FlushViewOfFile(m_pMappingBaseAddress, m_RememberDataSize);
			UnInit();
			bool success = false;
			HANDLE hFile = NULL;
			do {
				hFile = ::CreateFileA(m_FilePathname.c_str(),
					GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
				if (hFile == INVALID_HANDLE_VALUE || hFile == NULL)
					break;
				LARGE_INTEGER liDistanceToMove = { 0 };
				if (!::GetFileSizeEx(hFile, &liDistanceToMove))
					break;
				if (liDistanceToMove.QuadPart < m_RememberDataSize)
					break;
				::memset(&liDistanceToMove, 0x00, sizeof(liDistanceToMove));
				liDistanceToMove.QuadPart = m_RememberDataSize;
				if (!::SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_BEGIN))
					break;
				if (!::SetEndOfFile(hFile))
					break;
				success = true;
			} while (0);
			SK_CLOSE_HANDLE(hFile);
			if (success) {
				if (generate_filename.empty())
					result = m_FilePathname + (generate_filename.empty() ? "mem.finish" : generate_filename);
				else {
					result = m_FilePathname;
					for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
						if (*rit == '\\' || *rit == '/') {
							*rit = 0;
							result.resize(std::distance(result.begin(), rit.base()) - 1);
							break;
						}
					}
					result = PathFixed(result + "\\" + generate_filename);
				}
				::remove(result.c_str());
				success = ::rename(PathFixed(m_FilePathname).c_str(), result.c_str()) == 0;
			}
			return success ? result : "";
		}
#if 0
		bool Generate(const std::string& filename) {
			if (filename.empty())
				return false;
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load() || !m_hFileObj)
				return false;
			MAPFILEHEAD head = *reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (!head.Verify())
				return false;
			if (head.size_data <= 0)
				return false;
			::memset(m_pMappingBaseAddress, 0x00, LENMAPFILEHEAD);
			::memmove(m_pMappingBaseAddress, (char*)m_pMappingBaseAddress + LENMAPFILEHEAD, head.size_data);
			LARGE_INTEGER liDistanceToMove = { 0 };
			liDistanceToMove.QuadPart = head.size_data;
			if (!::SetFilePointerEx(m_hFileObj, liDistanceToMove, NULL, FILE_BEGIN))
				return false;
			if (!::SetEndOfFile(m_hFileObj))
				return false;
			return true;
		}
#endif
		const char* Read(const size_t& offset = 0) const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return nullptr;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (pHead->size_total <= (offset + LENMAPFILEHEAD))
				return nullptr;
			return reinterpret_cast<char*>(m_pMappingBaseAddress) + LENMAPFILEHEAD + offset;
		}
		const MAPFILEHEAD* Head() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return nullptr;
			return reinterpret_cast<const MAPFILEHEAD*>(m_pMappingBaseAddress);
		}
		bool Head(const std::function<void(MAPFILEHEAD*)>& set_head_cb) {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!set_head_cb)
				return false;
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return false;
			set_head_cb(reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress));
			return ::FlushViewOfFile(m_pMappingBaseAddress, LENMAPFILEHEAD) != 0;
		}
		size_t DataSize() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return 0;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			return pHead->size_data;
		}
		size_t TotalSize() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return 0;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			return pHead->size_total;
		}
		time_t TimeUpdate() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return 0;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			return pHead->time_update;
		}
		time_t TimeCreate() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return 0;
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			return pHead->time_create;
		}
		std::string Shared() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			if (!m_pMappingBaseAddress || !m_Ready.load())
				return "";
			auto pHead = reinterpret_cast<MAPFILEHEAD*>(m_pMappingBaseAddress);
			if (!pHead->shared[0])
				return "";
			return std::string(pHead->shared, __min(sizeof(pHead->shared), strlen(pHead->shared)));
		}
		const std::string& Identify() const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			return m_Identify;
		}
	private:
		std::uint64_t m_InitDataSize = 0;
		std::string m_Shared;
		std::string m_Identify;
		const std::string m_FilePathname;
		std::atomic_bool m_Ready = false;
		HANDLE m_hFileObj = nullptr;
		HANDLE m_hFileMappingObj = nullptr;
		DWORD m_MappingFileSize = 0;
		DWORD m_FileObjSize = 0;
		SYSTEM_INFO m_SystemInfo = { 0 };
		LPVOID m_pMappingBaseAddress = nullptr;
	private:
		unsigned long m_RememberDataSize = 0;
	};































#pragma pack(push,1)
	//! Memory file standard header
	typedef struct tagMapFileHeader {
		std::uint64_t		IdentifyHead;
		std::int64_t		length;						//	The length of the data(Inclusion head structure)
		std::int64_t		length_total;				//	File total size
		std::int64_t		create_timestamp;
		std::int64_t		update_timestamp;
		char shared[64];		//	Shared memory name
		char reserve1[64];
		char reserve2[80];
		std::uint64_t		IdentifyTail;
		tagMapFileHeader() {
			memset(this, 0x00, sizeof(*this));
			IdentifyHead = 0xFAC9C2D0;
			IdentifyTail = 0xB4B4AAC1;
			create_timestamp = ::time(0);
			update_timestamp = create_timestamp;
		}
		bool Verify() const {
			return IdentifyHead == 0xFAC9C2D0 && IdentifyTail == 0xB4B4AAC1;
		}
		void operator=(const tagMapFileHeader& obj) {
			::memcpy(this, &obj, sizeof(*this));
		}
	}MapFileHeader;
	const size_t MAPFILEHEADERLEN = sizeof(MapFileHeader);
#pragma pack(pop)
	class MapFile final
	{
	public:
		MapFile(const char* _FilePath, const char* _SharedName, bool _IsClearOpen = false);
		~MapFile();
	public:
		inline void Clear();
		inline const char* SharedName() const;
		//! 复位/修复
		inline void Reset(size_t DataLength = 0);
		inline bool Open();
		inline void Close();
		//! 从头部读取，不包含文件信息头
		inline const char* Read();
		inline long long GetDataLength();
		//! 从指定的偏移量读取，不算头部的偏移量|从数据开始计算的偏移量
		inline const char* Read(unsigned long _Offset);
		//! *追加写法
		//! 如果翻页 则返回一个内存映射新的基址，
		//! 如果未翻页，则返回nullptr
		//! 出错返回-1
		inline bool Write(const char* _pData, unsigned long _nData);
		//! *偏移写法
		//! *该方法只允许将数据写在文件现有数据大小的范围以内
		//! *使用该方法后，文件有效数据大小将不会改变
		inline bool Write(unsigned long _Offset, const char* _pData, unsigned long _nData);
	private:
		std::string m_strFilePath;
		tagMapFileHeader m_FileInfo;
		bool m_IsClearOpen = false;
		bool m_IsOpen = false;
		INT64		m_qwFileOffset = 0;						//	内存映像文件偏移，尾部位置（大小），约定始终设置为EOF
		HANDLE	m_hObjectFile = nullptr;						//	磁盘文件句柄
		HANDLE	m_hObjectFileMapping = nullptr;			//	映像文件句柄
		LPVOID	m_lpMappingBaseAddress = nullptr;		//	映像文件基址
		DWORD	m_dwMappingFileSize = 0;			//	当前内存文件映射大小 AllocationGranularity 的整数倍
		DWORD	m_dwContentFileSize = 0;				//	实际文件内容大小
		std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		std::condition_variable m_condition_variable;
	};

	MapFile::MapFile(const char* _FilePath, const char* _SharedName, bool _IsClearOpen /*= false*/) :
		m_IsOpen(false),
		m_hObjectFile(nullptr),
		m_IsClearOpen(_IsClearOpen),
		m_strFilePath(_FilePath) {
		::memcpy(m_FileInfo.shared, _SharedName, __min(::strlen(_SharedName), sizeof(m_FileInfo.shared)));
	}
	MapFile::~MapFile() {

	}
	inline const char* MapFile::SharedName() const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		if (!m_FileInfo.Verify())
			return nullptr;
		return m_FileInfo.shared;
	}
	inline void MapFile::Clear() {
		if (m_IsOpen) {
			Close();
			auto m_IsClearOpenOld = m_IsClearOpen;
			m_IsClearOpen = true;
			Open();
			m_IsClearOpen = m_IsClearOpenOld;
		}
	}
	//! 复位/修复
	inline void MapFile::Reset(size_t DataLength /*= 0*/) {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		if (0 == DataLength) {
			m_qwFileOffset = MAPFILEHEADERLEN;
			//! 更新文件头
			auto thisHeader = (MapFileHeader*)m_lpMappingBaseAddress;
			thisHeader->update_timestamp = ::time(0);
			thisHeader->length = 0;
			thisHeader->length_total = m_dwMappingFileSize;
			::memcpy(&m_FileInfo, thisHeader, MAPFILEHEADERLEN);
			::memcpy((BYTE*)m_lpMappingBaseAddress, &m_FileInfo, sizeof(m_FileInfo));
			::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset);
		}
		else {
			m_qwFileOffset = MAPFILEHEADERLEN + DataLength;
			//! 更新文件头
			auto thisHeader = (MapFileHeader*)m_lpMappingBaseAddress;
			thisHeader->update_timestamp = ::time(0);
			thisHeader->length = DataLength;
			thisHeader->length_total = m_dwMappingFileSize;
			::memcpy(&m_FileInfo, thisHeader, MAPFILEHEADERLEN);
			::memcpy((BYTE*)m_lpMappingBaseAddress, &m_FileInfo, sizeof(m_FileInfo));
			::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset);
		}
	}
	inline bool MapFile::Open() {
		if (m_IsOpen)
			return true;
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		DWORD dwOpenType = OPEN_ALWAYS;
		if (m_IsClearOpen) {
			if (::_access(m_strFilePath.c_str(), 0) != -1) {//! 如果该文件存在
				dwOpenType = OPEN_ALWAYS | TRUNCATE_EXISTING;
			}
		}
		m_hObjectFile = ::CreateFileA(
			m_strFilePath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			dwOpenType,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);
		if (INVALID_HANDLE_VALUE == m_hObjectFile || NULL == m_hObjectFile) {
			return false;
		}

		bool bProcessStageResult = false;
		//! 文件大小
		auto CurrentFileLen = ::GetFileSize(m_hObjectFile, NULL);

		if (m_IsClearOpen) {
			m_FileInfo.length = 0;
			m_FileInfo.length_total = MAPFILEHEADERLEN;
			m_qwFileOffset = MAPFILEHEADERLEN;
			bProcessStageResult = true;
		}
		else {
			//! 如果文件大小于与标准文件头，则清空该文件重新利用
			if (CurrentFileLen < MAPFILEHEADERLEN) {
				m_FileInfo.length = 0;
				m_FileInfo.length_total = MAPFILEHEADERLEN;
				m_qwFileOffset = MAPFILEHEADERLEN;
				bProcessStageResult = true;
			}
			else {
				if (::SetFilePointer(m_hObjectFile, 0, NULL, FILE_BEGIN) != HFILE_ERROR) {
					DWORD dwRead = 0;
					MapFileHeader Head;
					if (::ReadFile(m_hObjectFile, &Head, MAPFILEHEADERLEN, &dwRead, NULL)) {
						if (Head.Verify()) {
							::memcpy(&m_FileInfo, &Head, MAPFILEHEADERLEN);
							::SetFilePointer(m_hObjectFile, 0, NULL, FILE_END);
							::SetEndOfFile(m_hObjectFile);
							m_qwFileOffset = MAPFILEHEADERLEN + Head.length;
							bProcessStageResult = true;
						}
						else {
							auto headerror = 0;
						}
					}
				}
			}
		}

		if (!bProcessStageResult) {
			if (m_hObjectFile) {
				::CloseHandle(m_hObjectFile);
				m_hObjectFile = nullptr;
			}
			return false;
		}///if (!bProcessStageResult) 
#if 0
					//! 取得系统默认的单页面大小
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		if (CurrentFileLen <= sysInfo.dwAllocationGranularity) {
			m_dwMappingFileSize = sysInfo.dwAllocationGranularity;
		}
		else {
			m_dwMappingFileSize = ((CurrentFileLen / sysInfo.dwAllocationGranularity) + 1) * sysInfo.dwAllocationGranularity;
		}
#else
		m_dwMappingFileSize = CurrentFileLen;

		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		if (m_dwMappingFileSize < sysInfo.dwAllocationGranularity) {
			m_dwMappingFileSize = sysInfo.dwAllocationGranularity;
		}
#endif
		//! 预映射文件到内存
		m_hObjectFileMapping = ::CreateFileMappingA(
			m_hObjectFile,
			NULL,
			PAGE_READWRITE,
			0,
			m_dwMappingFileSize,
			(LPCSTR)m_FileInfo.shared);

		if (INVALID_HANDLE_VALUE == m_hObjectFileMapping || NULL == m_hObjectFileMapping) {
			if (m_hObjectFile) {
				::CloseHandle(m_hObjectFile);
				m_hObjectFile = nullptr;
			}
			return false;
		}///if (INVALID_HANDLE_VALUE == m_hObjectFileMapping || NULL == m_hObjectFileMapping)

		//! 映射文件到内存
		m_lpMappingBaseAddress = ::MapViewOfFile(
			m_hObjectFileMapping,
			FILE_MAP_ALL_ACCESS,
			(DWORD)(m_qwFileOffset >> 32),
			0/*(DWORD)(m_qwFileOffset & 0xFFFFFFFF)*/,
			m_dwMappingFileSize);
		if (NULL == m_lpMappingBaseAddress) {
			if (m_hObjectFileMapping) {
				::CloseHandle(m_hObjectFileMapping);
				m_hObjectFileMapping = nullptr;
			}
			if (m_hObjectFile) {
				::CloseHandle(m_hObjectFile);
				m_hObjectFile = nullptr;
			}
			return false;
		}
		::memcpy((BYTE*)m_lpMappingBaseAddress, &m_FileInfo, sizeof(m_FileInfo));
		if (!::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset)) {
			auto sk = 0;
		}
		m_IsOpen = true;
		return true;
	}///Open()
	inline void MapFile::Close() {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		if (!m_IsOpen) {
			return;
		}
		if (!::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset)) {

		}

		if (!::UnmapViewOfFile(m_lpMappingBaseAddress)) {

		}
		m_IsOpen = false;
		if (m_hObjectFileMapping) {
			::CloseHandle(m_hObjectFileMapping);
			m_hObjectFileMapping = nullptr;
		}
		if (m_hObjectFile) {
			::CloseHandle(m_hObjectFile);
			m_hObjectFile = nullptr;
		}
		auto hTempObjectFile = ::CreateFileA(
			m_strFilePath.c_str(),
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);
		if (INVALID_HANDLE_VALUE == hTempObjectFile || NULL == hTempObjectFile) {
			return;
		}
		auto hTempObjectFileMapping = ::CreateFileMappingA(
			hTempObjectFile,
			NULL,
			PAGE_READWRITE,
			0,
			(DWORD)(m_qwFileOffset & 0xFFFFFFFF),
			NULL);
		if (INVALID_HANDLE_VALUE == hTempObjectFileMapping || NULL == hTempObjectFileMapping) {
			if (hTempObjectFile) {
				::CloseHandle(hTempObjectFile);
				hTempObjectFile = NULL;
			}
			return;
		}
		if (hTempObjectFile) {
			::CloseHandle(hTempObjectFile);
			hTempObjectFile = nullptr;
		}
		auto pTempMappingBaseAddress = ::MapViewOfFile(
			hTempObjectFileMapping,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			(UINT)m_qwFileOffset);

		if (NULL == pTempMappingBaseAddress) {
			if (hTempObjectFileMapping) {
				::CloseHandle(hTempObjectFileMapping);
				hTempObjectFileMapping = nullptr;
			}
			return;
		}
		if (!::UnmapViewOfFile(pTempMappingBaseAddress)) {

		}
		if (hTempObjectFileMapping) {
			::CloseHandle(hTempObjectFileMapping);
			hTempObjectFileMapping = nullptr;
		}
	}///Close()
	//! 从头部读取，不包含文件信息头
	inline const char* MapFile::Read() {
		if (!m_IsOpen) {
			return nullptr;
		}
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		if (!m_hObjectFileMapping) {
			return nullptr;
		}
		return (const char*)m_lpMappingBaseAddress + MAPFILEHEADERLEN;
	}
	long long MapFile::GetDataLength() {
		if (!m_IsOpen) {
			return 0;
		}
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		auto thisHeader = (tagMapFileHeader*)m_lpMappingBaseAddress;
		return thisHeader->length;
	}
	//! 从指定的偏移量读取，不算头部的偏移量|从数据开始计算的偏移量
	const char* MapFile::Read(unsigned long _Offset) {
		if (!m_IsOpen) {
			return nullptr;
		}
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		if (!m_hObjectFileMapping) {
			return nullptr;
		}
		if (m_qwFileOffset < _Offset) {
			return nullptr;
		}
		return (const char*)m_lpMappingBaseAddress + MAPFILEHEADERLEN + _Offset;
	}
	//! *追加写法
	//! 如果翻页 则返回一个内存映射新的基址，
	//! 如果未翻页，则返回nullptr
	//! 出错返回-1
	inline bool MapFile::Write(const char* _pData, unsigned long _nData) {
		if (!m_IsOpen) {
			return false;
		}
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		//! 如果写入新的数据内容将超过当前映射文件的大小的60%
		//! 那么该映射文件扩容
		bool IsNewPage = false;	//! 翻页标记
		if ((m_dwMappingFileSize * 5 / 10) <= (m_qwFileOffset + _nData))
		{
			::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset);
			if (m_hObjectFileMapping) {
				::CloseHandle(m_hObjectFileMapping);
				m_hObjectFileMapping = nullptr;
			}
			::UnmapViewOfFile(m_lpMappingBaseAddress);
			m_lpMappingBaseAddress = nullptr;
			if (m_hObjectFile) {
				::CloseHandle(m_hObjectFile);
				m_hObjectFile = nullptr;
			}
			m_hObjectFile = ::CreateFileA(
				m_strFilePath.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_FLAG_SEQUENTIAL_SCAN,
				NULL);
			if (INVALID_HANDLE_VALUE == m_hObjectFile || NULL == m_hObjectFile) {
				return false;
			}
			::SetFilePointer(m_hObjectFile, 0, NULL, FILE_END);
			::SetEndOfFile(m_hObjectFile);

			//! 取得系统默认的单页面大小
			SYSTEM_INFO sysInfo;
			::GetSystemInfo(&sysInfo);

			auto diff = (m_dwMappingFileSize * 5 / 10) - (m_qwFileOffset + _nData);
			if (diff < 0) {
				diff = 0 - diff;
			}
			m_dwMappingFileSize += sysInfo.dwAllocationGranularity;
			m_dwMappingFileSize += static_cast<DWORD>(diff);

			m_hObjectFileMapping = ::CreateFileMappingA(
				m_hObjectFile,
				NULL,
				PAGE_READWRITE,
				0,
				m_dwMappingFileSize,
				(LPCSTR)m_FileInfo.shared);

			if (INVALID_HANDLE_VALUE == m_hObjectFileMapping || NULL == m_hObjectFileMapping) {
				if (m_hObjectFile) {
					::CloseHandle(m_hObjectFile);
					m_hObjectFile = NULL;
					return false;
				}
			}

			m_lpMappingBaseAddress = ::MapViewOfFile(
				m_hObjectFileMapping,
				FILE_MAP_ALL_ACCESS,
				(DWORD)(m_qwFileOffset >> 32),
				0/*(DWORD)(m_qwFileOffset & 0xFFFFFFFF)*/,
				m_dwMappingFileSize
			);

			if (NULL == m_lpMappingBaseAddress) {
				if (m_hObjectFileMapping) {
					::CloseHandle(m_hObjectFileMapping);
					m_hObjectFileMapping = NULL;
				}
				if (m_hObjectFile) {
					::CloseHandle(m_hObjectFile);
					m_hObjectFile = NULL;
				}
				return false;
			}///if (NULL == m_lpMappingBaseAddress)
			IsNewPage = true;
		}///if ((m_dwMappingFileSize * 6 / 10) < (m_qwFileOffset + _nData))
		::memcpy((char*)m_lpMappingBaseAddress + m_qwFileOffset, _pData, _nData);
		m_qwFileOffset += _nData;
		auto thisHeader = (tagMapFileHeader*)m_lpMappingBaseAddress;
		thisHeader->length = m_qwFileOffset - MAPFILEHEADERLEN;
		thisHeader->length_total = m_dwMappingFileSize;
		thisHeader->update_timestamp = ::time(0);
		::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset);
		if (IsNewPage) {
			/*return reinterpret_cast<long long>(m_lpMappingBaseAddress);*/
		}
		::memcpy(&m_FileInfo, thisHeader, sizeof(m_FileInfo));
		return true;
	}///Write()
	//! *偏移写法
	//! *该方法只允许将数据写在文件现有数据大小的范围以内
	//! *使用该方法后，文件有效数据大小将不会改变
	inline bool MapFile::Write(unsigned long _Offset, const char* _pData, unsigned long _nData) {
		if (!m_IsOpen) {
			return false;
		}
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		if (!m_hObjectFileMapping) {
			return false;
		}
		//! 指定的偏移位置不在有效数据范围内
		auto PosBegin = _Offset + MAPFILEHEADERLEN;
		auto PosEnd = _Offset + MAPFILEHEADERLEN + _nData;

		if (PosBegin < MAPFILEHEADERLEN || PosEnd > m_dwMappingFileSize) {
			return false;
		}
		::memcpy((char*)m_lpMappingBaseAddress + MAPFILEHEADERLEN + _Offset, _pData, _nData);
		//! 更新文件头
		auto thisHeader = (tagMapFileHeader*)m_lpMappingBaseAddress;
		thisHeader->update_timestamp = ::time(0);
		::memcpy(&m_FileInfo, thisHeader, MAPFILEHEADERLEN);
		::FlushViewOfFile(m_lpMappingBaseAddress, (DWORD)m_qwFileOffset);
		return true;
	}///Write()




}///namespace shared

/*!@ 新生联创®（上海）*/
/*!@ Fri Aug 13 09:15:41 UTC+0800 2021*/
/*!@ ___www.skstu.com___*/
#endif /*AFX_SK_H__22B9E599_9685_40F1_8EFE_FA517F90C840__HEAD__*/