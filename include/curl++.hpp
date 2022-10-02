#if !defined(INC_H___5DE3681B_08A3_407C_A49D_F2294AD0EC91__HEAD__)
#define INC_H___5DE3681B_08A3_407C_A49D_F2294AD0EC91__HEAD__

#define CURL_STATICLIB
#define CURL_DISABLE_LDAP
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Normaliz.lib")
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")
#include <curl/curl.h>

#define CURLPP_STATICLIB
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Multi.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

#pragma comment(lib,"curlpp")
#pragma comment(lib,"libcurl")

namespace shared {
	namespace http {

#pragma pack(push,1)
		enum struct EnRequestType : std::uint64_t {
			EN_REQUEST_TYPE_HEAD = 0,
			EN_REQUEST_TYPE_GET = 1,
			EN_REQUEST_TYPE_POST = 2,
			EN_REQUEST_TYPE_DOWN = 3,
			EN_REQUEST_TYPE_PUT = 4,
		};
		enum struct EnDownProgressStatus : int {
			EN_DOWN_PROGRESS_STATUS_ON = 0,
			EN_DOWN_PROGRESS_STATUS_OFF = 1,
		};

		typedef struct tagCacheFileHead final {
			long head;
			char url[256];
			long long total;
			long long current;
			long tail;
			char data[1];
			tagCacheFileHead();
			bool Verify() const;
			void operator=(const tagCacheFileHead& obj);
			bool operator<(const tagCacheFileHead& obj) const;
		}CacheFileHead;
		const size_t LENCAHCEFILEHEAD = sizeof(CacheFileHead);
#pragma pack(pop)


		class ReadData final {
			std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		public:
			ReadData();
			virtual ~ReadData();
			std::string pop();
			void push(const char* ind, const size_t& inn);
			void operator=(const ReadData& obj);
		private:
			void release();
		private:
			size_t size = 0;
			char* data = nullptr;
		};

		class CacheFile final {
			friend class Http;
			friend class Route;
			friend class ResponseObj;
			std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		public:
			CacheFile(const std::string& pathname, const std::string& url, const size_t& data_size);
			virtual ~CacheFile();
		private:
			void Init();
			void UnInit();
			bool Ready() const;
			bool Finish() const;
			const std::string& GenerateFilePathname() const;
			const long long& Current() const;
		public:
			bool Write(const char*, const size_t&);
		private:
			std::string Generate();
		private:
			CacheFileHead m_FileHead;
			HANDLE m_hFileObj = nullptr;
			const std::string m_Pathname;
			const std::string m_Url;
			size_t m_DataSize;
			std::atomic_bool m_Ready = false;
			std::atomic_bool m_Finish = false;
			std::string m_GenerateFilePathname;
		};

		using tfProgressCallback = std::function<int(const __int64&, const __int64&, const __int64&, const __int64&, void*)>;

		class RequestObj {
			friend class RouteObj;
			friend class ResponseObj;
			friend class Http;
		public:
			RequestObj(const std::uint64_t&, const std::uint64_t&);
			virtual ~RequestObj();
		public:
			void Url(const std::string&);
			void Verbose(const bool& flag);
			void NoSignal(const bool& flag);
			void SslVerifyHost(const int&);
			void SslVerifyPeer(const int&);
			void ConnectTimeout(const long&);
			void Timeout(const long&);
			void NoBody(const bool&);
			void Header(const bool&);
			void HttpGet(const bool&);
			void Post(const bool&);
			void PostFields(const std::string&);
			void PostFieldSize(const long&);
			void MaxRecvSpeedLarge(const __int64&);
			void ResumeFromLarge(const __int64&);
			void NoProgress(const bool&);
			void Type(const EnRequestType&);
			void ProgressCb(const tfProgressCallback&);
			void ResultCb(const std::function<void(const RequestObj*, const ResponseObj*)>&);
			void PrivatePtr(void*);
			void PrivateData(const std::string&);
			void Default();
			void HttpHeader(const std::map<std::string, std::string>&);
			void HttpHeaderAppend(const std::map<std::string, std::string>&);
			void TimeoutResponseMS(const std::uint64_t&);
			void DownCacheFilePathname(const std::string&);
		public:
			std::string Url() const;
		private:
			void Release() const;
			std::string m_Url;
			bool m_Verbose = false;
			bool m_NoSignal = true;
			int m_SslVerifyHost = 0;
			int m_SslVerifyPeer = 0;
			long m_ConnectTimeout = 10;
			long m_Timeout = 30;
			bool m_NoBody = false;
			bool m_Header = true;
			bool m_HttpGet = false;
			bool m_Post = false;
			std::string m_PostFields;
			long m_PostFieldSize = 0;
			__int64 m_MaxRecvSpeedLarge = 0;
			__int64 m_ResumeFromLarge = 0;
			bool m_NoProgress = false;
			void* m_PrivatePtr = nullptr;
			std::string m_PrivateData;
			EnRequestType m_Type = EnRequestType::EN_REQUEST_TYPE_GET;
			tfProgressCallback m_ProgressCb = nullptr;
			std::function<void(const RequestObj*, const ResponseObj*)> m_ResultCb = nullptr;
			std::uint64_t m_TimeoutResponseMS = 0;
			std::map<std::string, std::string> m_Headers;
			std::string m_DownCacheFilePathname;
		private:
			const std::uint64_t m_Identify;
			const std::uint64_t m_RouteIdentify;
			void operator>>(curlpp::Easy*);
			void operator>>(std::string&);
			void ResultCb(const ResponseObj*) const;
			const std::ostringstream* Stream() const;
		protected:
			std::ostringstream m_StreamBuffer;
		};

		class ResponseObj {
			friend class RouteObj;
			friend class RequestObj;
			friend class Http;
			std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		public:
			ResponseObj(const std::uint64_t&, const std::uint64_t&);
			virtual ~ResponseObj();
		public:
			std::uint64_t Identify() const;
			std::uint64_t RouteIdentify() const;
			void Reason(const std::string&);
			const std::string& Reason() const;
			void Reason(const std::function<void(const std::string&)>&) const;
			int ReasonCode() const;
			void ReasonCode(const int&);
			void EffectiveUrl(const std::function<void(const std::string&)>&) const;
			long ResponseCode() const;
			long HttpCode() const;
			long HttpConnectCode() const;
			long FileTime() const;
			double TotalTime() const;
			double NameLookupTime() const;
			double ConnectTime() const;
			double PreTransferTime() const;
			double StartTransferTime() const;
			double RedirectTime() const;
			long RedirectCount() const;
			double SizeUpload() const;
			double SizeDownload() const;
			double SpeedDownload() const;
			double SpeedUpload() const;
			long HeaderSize() const;
			long RequestSize() const;
			long SslVerifyResult() const;
			void SslEngines(const std::function<void(const std::list<std::string>&)>&) const;
			double ContentLengthDownload() const;
			double ContentLengthUpload() const;
			void ContentType(const std::function<void(const std::string&)>&) const;
			long HttpAuthAvail() const;
			long ProxyAuthAvail() const;
			long OsErrno() const;
			long NumConnects() const;
			void CookieList(const std::function<void(const std::list<std::string>&)>&) const;
			void Headers(const std::function<void(const std::unordered_map<std::string, std::string>&)>&) const;
			const __int64& ContentLength() const;
			bool ContentBodyIsUtf8() const;
			std::string ContentBody() const;
			std::string ContentHead() const;
			bool LocationNextUrl(const std::function<void(const std::string&)>&) const;
			std::string DownCacheFilePathname() const;
			const std::uint64_t& TimeoutResponseMS() const;
			const void* PrivatePtr() const;
			const std::string& PrivateData() const;
		private:
			void Release() const;
			const __int64& ResumeFromLarge() const;
			void HeaderPush(const std::string&);
			void operator<<(const curlpp::Easy&);
			void operator<<(RequestObj*);
			void operator<<(const std::ostringstream*);
			bool TimeoutActive(const std::uint64_t& current_size);
			ReadData* ReadDataPtr();
			int Write(const char*, const size_t&);
			void TimeoutResponseMS(const std::uint64_t&);
		protected:
			std::string m_RequestUrl;
			void* m_RequestRoutePtr = nullptr;
			std::string m_RequestRouteData;
			bool CacheFileInit(const std::string& url);
			bool DownCacheFilePathname(const std::string&);
		private:
			const std::uint64_t m_Identify;
			const std::uint64_t m_RouteIdentify;
			std::string m_Reason = R"(Complete all missions)";
			int m_ReasonCode = CURLcode::CURLE_OK;
			std::unordered_map<std::string, std::string> m_Headers;
		private:
			__int64 m_ResumeFromLarge = 0;
			std::uint64_t m_TimeoutResponseMS = 0;
			std::string m_EffectiveUrl;
			long m_ResponseCode = 0;
			long m_HttpConnectCode = 0;
			long m_FileTime = 0;
			double m_TotalTime = 0;
			double m_NameLookupTime = 0;
			double m_ConnectTime = 0;
			double m_PreTransferTime = 0;
			double m_StartTransferTime = 0;
			double m_RedirectTime = 0;
			long m_RedirectCount = 0;
			double m_SizeUpload = 0;
			double m_SizeDownload = 0;
			double m_SpeedDownload = 0;
			double m_SpeedUpload = 0;
			long m_HeaderSize = 0;
			long m_RequestSize = 0;
			long m_SslVerifyResult = 0;
			std::list<std::string> m_SslEngines;
			double m_ContentLengthDownload = 0;
			double m_ContentLengthUpload = 0;
			std::string m_ContentType;
			long m_HttpAuthAvail = 0;
			long m_ProxyAuthAvail = 0;
			long m_OsErrno = 0;
			long m_NumConnects = 0;
			std::list<std::string> m_CookieList;
			//!@ extern
			__int64 m_ContentLength = 0;
			std::string m_ContentBody;
			std::string m_ContentHead;
			std::string m_DownCacheFilePathname;
		private:
			ReadData* m_pReadBuffer = nullptr;
			//!@ 前一次下载大小
			std::atomic_uint64_t m_PrevDownSize = 0;
			std::atomic_uint64_t m_DownActiveTimeMS = 0;
		private:
			std::string m_CachePath;
			std::string m_CachePathname;
			std::string m_CacheFinishFilePathname;
			std::string m_CacheTempFilePathname;
			std::string m_CacheFinishFilename;
			std::string m_CacheTempFilename;
			std::shared_ptr<CacheFile> m_pCacheFile;
			std::ostringstream m_StreamRead;
		};

		using tfRouteTask = std::tuple<RequestObj*, ResponseObj*>;
		class RouteObj final {
			friend class Http;
			std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		public:
			RouteObj(const std::uint64_t&);
		private:
			virtual ~RouteObj();
		public:
			RequestObj* Push(const std::function<bool(RequestObj*)>&);
			const std::uint64_t& Identify() const;
			void MultiWait(const std::uint64_t& timewait_ms);
			void Release() const;
			std::uint64_t MultiWait() const;
		public:
			std::vector<std::tuple<RequestObj*, ResponseObj*>> Tasks() const;
			bool Empty() const;
		private:
			const std::uint64_t m_Identify = 0;
			std::atomic_bool m_Perform = false;
			std::string m_DownCacheFilePathname;
			std::atomic_uint64_t m_MultiWait = 30 * 1000;
		private:
			std::map<RequestObj*, ResponseObj*> m_TaskQ;
		};

		class Http final {
			std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		public:
			Http(const bool& Verbose = false);
			virtual ~Http();
		private:
			bool Init();
			void UnInit();
		public:
			bool Ready() const;
			bool Perform(RouteObj*);
			RouteObj* RouteCreate();
		private:
			std::atomic_bool m_Verbose = false;
			std::atomic_bool m_Ready = false;
		};


	}///namespace http
}///namespace shared

#pragma comment(lib,"curl++")
/// /*新生®（上海）**/
/// /*2022_03_03T05:34:06.5936519Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___5DE3681B_08A3_407C_A49D_F2294AD0EC91__HEAD__