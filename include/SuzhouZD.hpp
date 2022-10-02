#if !defined(INC_H___312A2C26_9467_4A05_9BD6_38C51E75DB3B__HEAD__)
#define INC_H___312A2C26_9467_4A05_9BD6_38C51E75DB3B__HEAD__


#include "rapidjson.h"
#include "iconv.h"

namespace SunZouZD {

	const char CodeTable[] = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789)";

	/******************************************************************************
	*  Copyright (C) 2014-2015，游迅游戏盒                                       *
	*  保留所有权利。                                                            *
	******************************************************************************
	*  作者 : 马超
	*  版本 : 1.0
	*****************************************************************************/
	/*  修改记录:
							日期       版本    修改人             修改内容
					--------------------------------------------------------------------------
	******************************************************************************/
	class UrlEncode final
	{
	public:
		UrlEncode(std::string baseUrl, bool utf8Encode = false) {
			m_baseUrl = baseUrl;
			m_utf8 = utf8Encode;
		}
		~UrlEncode() {

		}
	public:
		/** 添加Get参数
		@param key Get参数的Key
		@param value Get参数的value,默认使用url编码对其进行编码后添加到url中
		@param encodeed value是否已经编码，如果已经编码，那本函数内不对其做url编码
		*/
		void AddGetParam(const std::string& key, std::string value, bool encoded = false) {
			if (key.empty()) {
				assert(false);
				return;
			}
			if (!encoded)
				value = Encode(value, m_utf8);
			if (!m_params.empty())
				m_params += "&";
			std::string curParam(key);
			curParam.append("=").append(value);
			m_params += curParam;
		}

		/** 添加Get参数
		@param key Get参数的Key
		@param value Get参数的value
		*/
		void AddGetParam(std::string key, int value) {
			AddGetParam(key, std::to_string(value), true);
		}

		/** 返回编码后的参数
		*/
		std::string GetEncodedUrl() {

		}

	public:
		/** URL编码
		@param [in] url 需要被编码的ansii字符串
		@return 返回编码结果
		*/
		static std::string EncodeA(const std::string& url) {
			std::string r;
			r.reserve(url.length());
			for (size_t i = 0; i < url.length(); i++)
			{
				char c = url[i];
				if (isalnum(c & 0xff))
				{
					r += c;
				}
				//else if(isspace(c & 0xff))
				//{
				//	r += '+';
				//}
				else if (IsSpecial(c & 0xff))
				{
					r += c;
				}
				else
				{
					r += '%';
					r += ToHex((unsigned char)(c >> 4) & 0xf);
					r += ToHex((unsigned char)c % 0x10);
				}
			}
			return r;
		}
		static std::string DecodeA(const std::string& url) {
			std::string r;
			r.reserve(url.length());
			for (size_t i = 0; i < url.length(); i++)
			{
				char c = url[i];
				if (c != '%')
				{
					r += c;
				}
				else
				{
					char c1 = url[++i];
					char c0 = url[++i];

					int num = 0;
					num += FromHex(c1) * 16 + FromHex(c0);
					r += char(num);
				}
			}
			return r;
		}

		/** URL编码
		@param [in] url 需要被编码的Unicode字符串
		@param [in] useUtf8 非ansii字符部分是使用utf8编码后再进行Url编码还是直接使用本地多字节字符集编码
		@return 返回编码结果
		*/
		static std::string Encode(const std::string& url, const bool& useUtf8 = false) {
			std::string urlA;
			if (useUtf8)
				urlA = shared::IConv::MBytesToUTF8(url);
			return EncodeA(urlA);
		}
		static std::string Decode(const std::string& url, const bool& useUtf8 = false) {
			return useUtf8 ? shared::IConv::UTF8ToMBytes(DecodeA(url)) : DecodeA(url);
		}
	private:
		/** 字符转成二进制
		@param [in] c 字符
		@return 返回二进制
		*/
		static unsigned char ToHex(const unsigned char& c) {
			return c > 9 ? (c - 10 + 'A') : (c + '0');
		}
		/** 二进制转换字符
		@param [in] c 二进制
		@return 返回字符
		*/
		static unsigned char FromHex(const unsigned char& c) {
			return isdigit(c) ? (c - '0') : (c - 'A' + 10);
		}
		/** 是否是特殊字符
		@param [in] c 字符
		@return 是返回true，否则返回false
		*/
		static bool IsSpecial(const unsigned char& c) {
			if (c == '@' || c == '*' || c == '-' || c == '.' || c == '_')
				return true;
			return false;
		}
	private:
		/** 基本URL
		*/
		std::string m_baseUrl;
		/** 参数
		*/
		std::string m_params;
		/** 字符串做url编码时使用utf8
		*/
		bool m_utf8;
	};

#if 0
	{
		"code":1000,
			"msg" : "成功",
			"time" : "1635607703",
			"data" : {
			"adsetting":{
				"envcontrol":0,
					"lnkcount" : 2,
					"procount" : 6
			},
				"Name" : "AdobePhotoshop",
					"Size" : "1.68GB",
					"DownAddress" : "http:\/\/down10.zol.com.cn\/it\/Adobe0PhotoshopCS6.zip",
					"SuccessType" : 0
		}
	}
#endif

	class SoftwareInfo final {
		class dataObj final {
		public:
			class AdsSetting final {
			public:
				std::string lnkcount = "2";
				std::string procount = "6";
				std::string envcontrol = "0";
			};
		public:
			std::string id;
			std::string Name;
			std::string CateId;
			std::string Size;
			std::string Score;
			std::string LogoPicAddress;
			std::string LogoPicAddress_local;
			std::string LogoPicAddress_request_id;
			std::string LogoPicAddress_obs_url;
			std::string Version;
			std::string SupportSys;
			std::string SoftLicen;
			std::string updateDate;
			std::string BigPicAddress;
			std::string BigPicAddress_local;
			std::string BigPicAddress_request_id;
			std::string BigPicAddress_obs_url;
			std::string DownAddress;
			std::string OrderBy;
			std::string Status;
			std::string essential_softwares;
			std::string good_evaluation;
			std::string hot_search;
			std::string IsRecommend;
			std::string Descript;
			std::string Detail;
			std::string channel;
			std::string type;
			std::string SuccessType;
			AdsSetting adsSetting;
		};
	public:
		std::string code;
		std::string msg;
		std::string time;
		dataObj data;
	private:
		std::string m_JsonPak;
	public:
		SoftwareInfo() {
		}

		~SoftwareInfo() { UnInit(); }

		void operator=(const std::string& jsonPak) {
			m_JsonPak = shared::IConv::UTF8ToMBytes(jsonPak);
			Init();
		}
		bool Empty() const {
			return code != "1000" || data.DownAddress.empty() || data.Name.empty();
		}
		bool Verify() const {
			return !data.DownAddress.empty() && !data.Name.empty();
		}
	private:
		bool Init() {
			rapidjson::Document doc;
			if (doc.Parse(m_JsonPak.data(), m_JsonPak.size()).HasParseError())
				return false;
			if (!doc.IsObject())
				return false;
			if (doc.HasMember("code"))
				JSON_VALUE_TO_STRING(doc["code"], code);
			if (doc.HasMember("msg"))
				JSON_VALUE_TO_STRING(doc["msg"], msg);
			if (doc.HasMember("time"))
				JSON_VALUE_TO_STRING(doc["time"], time);
			if (doc.HasMember("data")) {
				if (doc["data"].IsObject()) {
					if (doc["data"].HasMember("adsetting")) {
						if (doc["data"]["adsetting"].IsObject()) {
							if (doc["data"]["adsetting"].HasMember("procount"))
								JSON_VALUE_TO_STRING(doc["data"]["adsetting"]["procount"], data.adsSetting.procount);
							if (doc["data"]["adsetting"].HasMember("lnkcount"))
								JSON_VALUE_TO_STRING(doc["data"]["adsetting"]["lnkcount"], data.adsSetting.lnkcount);
							if (doc["data"]["adsetting"].HasMember("envcontrol"))
								JSON_VALUE_TO_STRING(doc["data"]["adsetting"]["envcontrol"], data.adsSetting.envcontrol);
						}
					}

					auto& objData = doc["data"];
					if (objData.HasMember("id"))
						JSON_VALUE_TO_STRING(objData["id"], data.id);
					if (objData.HasMember("Name"))
						JSON_VALUE_TO_STRING(objData["Name"], data.Name);
					if (objData.HasMember("CateId"))
						JSON_VALUE_TO_STRING(objData["CateId"], data.CateId);
					if (objData.HasMember("Size"))
						JSON_VALUE_TO_STRING(objData["Size"], data.Size);
					if (objData.HasMember("Score"))
						JSON_VALUE_TO_STRING(objData["Score"], data.Score);
					if (objData.HasMember("LogoPicAddress"))
						JSON_VALUE_TO_STRING(objData["LogoPicAddress"], data.LogoPicAddress);
					if (objData.HasMember("LogoPicAddress_local"))
						JSON_VALUE_TO_STRING(objData["LogoPicAddress_local"], data.LogoPicAddress_local);
					if (objData.HasMember("LogoPicAddress_request_id"))
						JSON_VALUE_TO_STRING(objData["LogoPicAddress_request_id"], data.LogoPicAddress_request_id);
					if (objData.HasMember("LogoPicAddress_obs_url"))
						JSON_VALUE_TO_STRING(objData["LogoPicAddress_obs_url"], data.LogoPicAddress_obs_url);
					if (objData.HasMember("Version"))
						JSON_VALUE_TO_STRING(objData["Version"], data.Version);
					if (objData.HasMember("SupportSys"))
						JSON_VALUE_TO_STRING(objData["SupportSys"], data.SupportSys);
					if (objData.HasMember("SoftLicen"))
						JSON_VALUE_TO_STRING(objData["SoftLicen"], data.SoftLicen);
					if (objData.HasMember("updateDate"))
						JSON_VALUE_TO_STRING(objData["updateDate"], data.updateDate);
					if (objData.HasMember("BigPicAddress"))
						JSON_VALUE_TO_STRING(objData["BigPicAddress"], data.BigPicAddress);
					if (objData.HasMember("BigPicAddress_local"))
						JSON_VALUE_TO_STRING(objData["BigPicAddress_local"], data.BigPicAddress_local);
					if (objData.HasMember("BigPicAddress_request_id"))
						JSON_VALUE_TO_STRING(objData["BigPicAddress_request_id"], data.BigPicAddress_request_id);
					if (objData.HasMember("BigPicAddress_obs_url"))
						JSON_VALUE_TO_STRING(objData["BigPicAddress_obs_url"], data.BigPicAddress_obs_url);
					if (objData.HasMember("DownAddress"))
						JSON_VALUE_TO_STRING(objData["DownAddress"], data.DownAddress);
					if (objData.HasMember("OrderBy"))
						JSON_VALUE_TO_STRING(objData["OrderBy"], data.OrderBy);
					if (objData.HasMember("Status"))
						JSON_VALUE_TO_STRING(objData["Status"], data.Status);
					if (objData.HasMember("essential_softwares"))
						JSON_VALUE_TO_STRING(objData["essential_softwares"], data.essential_softwares);
					if (objData.HasMember("good_evaluation"))
						JSON_VALUE_TO_STRING(objData["good_evaluation"], data.good_evaluation);
					if (objData.HasMember("hot_search"))
						JSON_VALUE_TO_STRING(objData["hot_search"], data.hot_search);
					if (objData.HasMember("IsRecommend"))
						JSON_VALUE_TO_STRING(objData["IsRecommend"], data.IsRecommend);
					if (objData.HasMember("Descript"))
						JSON_VALUE_TO_STRING(objData["Descript"], data.Descript);
					if (objData.HasMember("Detail"))
						JSON_VALUE_TO_STRING(objData["Detail"], data.Detail);
					if (objData.HasMember("channel"))
						JSON_VALUE_TO_STRING(objData["channel"], data.channel);
					if (objData.HasMember("type"))
						JSON_VALUE_TO_STRING(objData["type"], data.type);
					if (objData.HasMember("SuccessType"))
						JSON_VALUE_TO_STRING(objData["SuccessType"], data.SuccessType);
				}
			}
			return true;
		}
		void UnInit() {

		}
	};

	enum struct EnAdevType : std::uint32_t {
		EN_ADEV_TYPE_Navigation = 0,
		EN_ADEV_TYPE_Hyperlink = 1,
		EN_ADEV_TYPE_Program = 2,
	};

	class ADsInfo final {
	public:
		class dataObj final {
			friend class ADsInfo;
		public:
			class cchromeObj final {
			public:
				std::string area;
				std::string pro;
				std::string qd;
				std::string vm;
				std::string glkqd;
				std::string glopen;
				std::string glpro;
				std::string glStartTime;
				std::string glEndTime;
#if 0//!@ 旧版下载器
				std::string urllist;
#else//!@ 新版下载器
				std::unordered_set<std::string> urllist;
#endif
			};
			class sogouObj final {
			public:
				std::string sogou;
				std::string area;
				std::string pro;
			};
			class NewNaviObj final {
			public:
				class ProgCheckObj final {
				public:
					std::string Prog;
					std::string NaNameExit;
					std::string FaviNameExit;
					std::string DestopUrl;
					std::string IERegPage;
					std::string IERegPolice;
					std::string IEDestopLnk;
					std::string OpenUrlCfg;
					std::string Favicons;
					std::string DestopLnk;
					std::string TaskBar;
					std::string Startbar;
					std::string DefExeUrl;
					std::string ChangeDTSUrls;
				};
			public:
				std::string bDesktopReadOnly;
				std::string NaviUrlList;
				std::string LnkIcoList;
				std::string NavSort;
				std::string NoEnvNavSort;
				std::string NaviNames;
				std::string OpenNewParam;
				std::string NoNavArea;
				std::string NoFavArea;
				std::string BrowsList;
				std::string RepairIEIcon;
				std::string NoEnvOpenUrl;
				std::vector<ProgCheckObj> ProgCheckObjs;
			};
			class PreNaviObj final {
			public:
				class ProgFuncObj final {
				public:
					std::string Prog;
					std::string Func;
					std::string Url;
				};
			public:
				std::string NoNavArea;
				std::string NoFavArea;
				std::vector<ProgFuncObj> ProgFuncObjs;
			};
			class cmarkObj final {
			public:
				class marksObj final {
				public:
					std::string name;
					std::string url;
					std::string ico;
				};
			public:
				std::string dest;
				std::string delay;
				std::string browsertype;
				std::string qd;
				std::string process;
				std::string version;
				std::string area;
				std::vector<marksObj> marksObjs;
			};
		public:
			std::string id;
			std::string china;
			std::string PopDownHint;
			std::string DefaultGameName;
			std::string url;
			std::string md5;
			std::string start;
			std::string unist;
			std::string open;
			std::string taskbar;
			std::string param;
			std::string lnk;
			std::string runwait;
			std::string dlay;
			std::string pro;
			std::string lnkpath;
			std::string tip;
			std::string specialico;
			std::string exec;
			std::string mark;
			std::string bootupexec;
			std::string crx;
		private:
			std::string ico;
			std::string IeIcon;
		public:
			std::string BrowLnkArea;
			std::string StrOpenUrl;
			std::string DHCtrl;
			std::string nocontivi;
			std::string dhvi;
			std::string dqshow;
			std::string bDesktopReadOnly;
			std::string IsDaohang;
			cchromeObj cchrome;
			sogouObj sogou;
			NewNaviObj NewNavi;
			PreNaviObj PreNavi;
			cmarkObj cmark;
			std::string verifyMD5;
			std::string extra_reg;
			std::string install;
			std::vector<std::string> installs;
			std::string dellnkname;
			std::string startmenu;
			std::string controlmgr;
			std::string install_num;
			std::string Pullup;
			std::string createtskb;
			std::string createdesk;
			std::vector<std::string> SucChecks;
			EnAdevType AdevType = EnAdevType::EN_ADEV_TYPE_Program;
			std::string LogoPathname;
			std::size_t Index;//!@ 序号
			std::string hijack = "0";
		public:
			bool HijackFlag() const {
				return ::atoi(hijack.c_str()) != 0;
			}
			const std::string& Icourl() const {
				if (AdevType == EnAdevType::EN_ADEV_TYPE_Navigation)
					return IeIcon;
				return ico;
			}
		};
	public:
		ADsInfo() {
		}
		~ADsInfo() {
			UnInit();
		}
		void Fixed() {//!@ 广告过滤
			for (auto it = datas.begin(); it != datas.end();) {
				bool remove = false;
				do {
					if (it->SucChecks.empty())
						break;
					if (it->AdevType == EnAdevType::EN_ADEV_TYPE_Navigation)
						break;
					if (it->AdevType == EnAdevType::EN_ADEV_TYPE_Hyperlink)
						break;
					std::string check_register = *std::prev(it->SucChecks.end());
					if (check_register.empty())
						break;
					remove = shared::Win::Registry::Exists(check_register);
				} while (0);
				if (remove) {
					removes.emplace_back(*it);
					it = datas.erase(it);
					continue;
				}
				++it;
			}
		}
		const dataObj* DHGet() const {
			for (auto it = datas.begin(); it != datas.end(); ++it) {
				if (it->IsDaohang != "0" && !it->IsDaohang.empty()) {
					return &(*it);
				}
			}
			return nullptr;
		}
		bool LogoPathname(const std::string& adevID, const std::string& logoPathname) {
			bool result = false;
			for (auto& data : datas)
				if (adevID.compare(data.id) == 0) {
					data.LogoPathname = logoPathname;
					result = true;
					break;
				}
			return result;
		}
		void operator=(const std::string& jsonPak) {
			m_JsonPak = jsonPak;
			Init();
			std::for_each(datas.begin(), datas.end(),
				[&](dataObj& adev) {
					if (atoi(adev.IsDaohang.c_str()) != 0)
						adev.AdevType = EnAdevType::EN_ADEV_TYPE_Navigation;
					else if (atoi(adev.lnk.c_str()) != 0)
						adev.AdevType = EnAdevType::EN_ADEV_TYPE_Hyperlink;
				});
		}
		const dataObj* Search(const std::string& ad_id) const {
			for (const auto& data : datas)
				if (ad_id.compare(data.id) == 0)
					return &data;
			return nullptr;
		}
		std::set<std::string> IconsGet() const {
			std::set<std::string> result;
			for (const auto& data : datas)
				if (!data.Icourl().empty())
					result.emplace(data.Icourl());
			return result;
		}
	private:
		bool Init() {
			rapidjson::Document doc;
			if (doc.Parse(m_JsonPak.data(), m_JsonPak.size()).HasParseError())
				return false;
			if (!doc.HasMember("data"))
				return false;
			auto& ayData = doc["data"];
			if (!ayData.IsArray())
				return false;
			size_t Index = 0;
			for (auto itData = ayData.Begin(); itData != ayData.End(); ++itData) {
				++Index;
				if (!itData->IsObject())
					continue;
				auto& objData = *itData;
				dataObj data;
				data.Index = Index;
				for (auto it = objData.MemberBegin(); it != objData.MemberEnd(); ++it) {
					if (!it->name.IsString() || !it->value.IsString())
						continue;
					if (::memcmp("install_", it->name.GetString(), ::strlen("install_")))
						continue;
					if (it->name.GetStringLength() <= ::strlen("install_"))
						continue;
					char c = it->name.GetString()[::strlen("install_")];
					if (c >= '0' && c <= '9')
						data.installs.emplace_back(std::string(it->value.GetString(), it->value.GetStringLength()));
				}

				for (auto it = objData.MemberBegin(); it != objData.MemberEnd(); ++it) {
					if (!it->name.IsString() || !it->value.IsString())
						continue;
					if (::memcmp("SucCheck", it->name.GetString(), ::strlen("SucCheck")))
						continue;
					if (it->name.GetStringLength() <= ::strlen("SucCheck"))
						continue;
					char c = it->name.GetString()[::strlen("SucCheck")];
					if (c >= '0' && c <= '9')
						data.SucChecks.emplace_back(std::string(it->value.GetString(), it->value.GetStringLength()));
				}

				if (objData.HasMember("id"))
					JSON_VALUE_TO_STRING(objData["id"], data.id);
				if (objData.HasMember("hijack"))
					JSON_VALUE_TO_STRING(objData["hijack"], data.hijack);
				if (objData.HasMember("china"))
					JSON_VALUE_TO_STRING(objData["china"], data.china);
				if (objData.HasMember("PopDownHint"))
					JSON_VALUE_TO_STRING(objData["PopDownHint"], data.PopDownHint);
				if (objData.HasMember("DefaultGameName"))
					JSON_VALUE_TO_STRING(objData["DefaultGameName"], data.DefaultGameName);
				if (objData.HasMember("url"))
					JSON_VALUE_TO_STRING(objData["url"], data.url);
				if (objData.HasMember("md5"))
					JSON_VALUE_TO_STRING(objData["md5"], data.md5);
				if (objData.HasMember("start"))
					JSON_VALUE_TO_STRING(objData["start"], data.start);
				if (objData.HasMember("unist"))
					JSON_VALUE_TO_STRING(objData["unist"], data.unist);
				if (objData.HasMember("open"))
					JSON_VALUE_TO_STRING(objData["open"], data.open);
				if (objData.HasMember("taskbar"))
					JSON_VALUE_TO_STRING(objData["taskbar"], data.taskbar);
				if (objData.HasMember("param"))
					JSON_VALUE_TO_STRING(objData["param"], data.param);
				if (objData.HasMember("lnk"))
					JSON_VALUE_TO_STRING(objData["lnk"], data.lnk);
				if (objData.HasMember("runwait"))
					JSON_VALUE_TO_STRING(objData["runwait"], data.runwait);
				if (objData.HasMember("dlay"))
					JSON_VALUE_TO_STRING(objData["dlay"], data.dlay);
				if (objData.HasMember("pro"))
					JSON_VALUE_TO_STRING(objData["pro"], data.pro);
				if (objData.HasMember("lnkpath"))
					JSON_VALUE_TO_STRING(objData["lnkpath"], data.lnkpath);
				if (objData.HasMember("ico"))
					JSON_VALUE_TO_STRING(objData["ico"], data.ico);
				if (objData.HasMember("tip"))
					JSON_VALUE_TO_STRING(objData["tip"], data.tip);
				if (objData.HasMember("specialico"))
					JSON_VALUE_TO_STRING(objData["specialico"], data.specialico);
				if (objData.HasMember("exec"))
					JSON_VALUE_TO_STRING(objData["exec"], data.exec);
				if (objData.HasMember("mark"))
					JSON_VALUE_TO_STRING(objData["mark"], data.mark);
				if (objData.HasMember("bootupexec"))
					JSON_VALUE_TO_STRING(objData["bootupexec"], data.bootupexec);
				if (objData.HasMember("crx"))
					JSON_VALUE_TO_STRING(objData["crx"], data.crx);
				if (objData.HasMember("IeIcon"))
					JSON_VALUE_TO_STRING(objData["IeIcon"], data.IeIcon);
				if (objData.HasMember("BrowLnkArea"))
					JSON_VALUE_TO_STRING(objData["BrowLnkArea"], data.BrowLnkArea);
				if (objData.HasMember("StrOpenUrl"))
					JSON_VALUE_TO_STRING(objData["StrOpenUrl"], data.StrOpenUrl);
				if (objData.HasMember("DHCtrl"))
					JSON_VALUE_TO_STRING(objData["DHCtrl"], data.DHCtrl);
				if (objData.HasMember("nocontivi"))
					JSON_VALUE_TO_STRING(objData["nocontivi"], data.nocontivi);
				if (objData.HasMember("dhvi"))
					JSON_VALUE_TO_STRING(objData["dhvi"], data.dhvi);
				if (objData.HasMember("dqshow"))
					JSON_VALUE_TO_STRING(objData["dqshow"], data.dqshow);
				if (objData.HasMember("bDesktopReadOnly"))
					JSON_VALUE_TO_STRING(objData["bDesktopReadOnly"], data.bDesktopReadOnly);
				if (objData.HasMember("IsDaohang"))
					JSON_VALUE_TO_STRING(objData["IsDaohang"], data.IsDaohang);
				if (objData.HasMember("verifyMD5"))
					JSON_VALUE_TO_STRING(objData["verifyMD5"], data.verifyMD5);
				if (objData.HasMember("extra_reg"))
					JSON_VALUE_TO_STRING(objData["extra_reg"], data.extra_reg);
				if (objData.HasMember("install"))
					JSON_VALUE_TO_STRING(objData["install"], data.install);
#if 0
				if (objData.HasMember("SucCheck1"))
					JSON_VALUE_TO_STRING(objData["SucCheck1"], data.SucCheck1);
				if (objData.HasMember("SucCheck2"))
					JSON_VALUE_TO_STRING(objData["SucCheck2"], data.SucCheck2);
#endif
				if (objData.HasMember("dellnkname"))
					JSON_VALUE_TO_STRING(objData["dellnkname"], data.dellnkname);
				if (objData.HasMember("startmenu"))
					JSON_VALUE_TO_STRING(objData["startmenu"], data.startmenu);
				if (objData.HasMember("controlmgr"))
					JSON_VALUE_TO_STRING(objData["controlmgr"], data.controlmgr);
				if (objData.HasMember("Pullup"))
					JSON_VALUE_TO_STRING(objData["Pullup"], data.Pullup);
				if (objData.HasMember("createtskb"))
					JSON_VALUE_TO_STRING(objData["createtskb"], data.createtskb);
				if (objData.HasMember("createdesk"))
					JSON_VALUE_TO_STRING(objData["createdesk"], data.createdesk);
				if (objData.HasMember("install_num"))
					JSON_VALUE_TO_STRING(objData["install_num"], data.install_num);

				if (objData.HasMember("cchrome")) {
					auto& objCchrome = objData["cchrome"];
					if (objCchrome.IsObject()) {
#if 1//!@ 新版下载器
						if (objCchrome.HasMember("urllist")) {
							std::string temp;
							JSON_VALUE_TO_STRING(objCchrome["urllist"], temp);
							std::vector<std::string> vtTemp;
							shared::Win::File::ParseA(temp, '|', vtTemp);
							std::for_each(vtTemp.begin(), vtTemp.end(),
								[&](const auto& url) {
									if (url.find("http:") != std::string::npos) {
										data.cchrome.urllist.emplace(url);
									}
								});
						}
#else//!@旧版下载器
						if (objCchrome.HasMember("urllist"))
							JSON_VALUE_TO_STRING(objCchrome["urllist"], data.cchrome.urllist);
#endif
						if (objCchrome.HasMember("area"))
							JSON_VALUE_TO_STRING(objCchrome["area"], data.cchrome.area);
						if (objCchrome.HasMember("pro"))
							JSON_VALUE_TO_STRING(objCchrome["pro"], data.cchrome.pro);
						if (objCchrome.HasMember("qd"))
							JSON_VALUE_TO_STRING(objCchrome["qd"], data.cchrome.qd);
						if (objCchrome.HasMember("vm"))
							JSON_VALUE_TO_STRING(objCchrome["vm"], data.cchrome.vm);
						if (objCchrome.HasMember("glkqd"))
							JSON_VALUE_TO_STRING(objCchrome["glkqd"], data.cchrome.glkqd);
						if (objCchrome.HasMember("glopen"))
							JSON_VALUE_TO_STRING(objCchrome["glopen"], data.cchrome.glopen);
						if (objCchrome.HasMember("glpro"))
							JSON_VALUE_TO_STRING(objCchrome["glpro"], data.cchrome.glpro);
						if (objCchrome.HasMember("glStartTime"))
							JSON_VALUE_TO_STRING(objCchrome["glStartTime"], data.cchrome.glStartTime);
						if (objCchrome.HasMember("glEndTime"))
							JSON_VALUE_TO_STRING(objCchrome["glEndTime"], data.cchrome.glEndTime);

					}
				}
				if (objData.HasMember("sogou")) {
					auto& objSogou = objData["sogou"];
					if (objSogou.IsObject()) {
						if (objSogou.HasMember("sogou"))
							JSON_VALUE_TO_STRING(objSogou["sogou"], data.sogou.sogou);
						if (objSogou.HasMember("area"))
							JSON_VALUE_TO_STRING(objSogou["area"], data.sogou.area);
						if (objSogou.HasMember("pro"))
							JSON_VALUE_TO_STRING(objSogou["pro"], data.sogou.pro);

					}
				}
				if (objData.HasMember("NewNavi")) {
					auto& objNewNavi = objData["NewNavi"];
					if (objNewNavi.IsObject()) {
						if (objNewNavi.HasMember("bDesktopReadOnly"))
							JSON_VALUE_TO_STRING(objNewNavi["bDesktopReadOnly"], data.NewNavi.bDesktopReadOnly);
						if (objNewNavi.HasMember("NaviUrlList"))
							JSON_VALUE_TO_STRING(objNewNavi["NaviUrlList"], data.NewNavi.NaviUrlList);
						if (objNewNavi.HasMember("LnkIcoList"))
							JSON_VALUE_TO_STRING(objNewNavi["LnkIcoList"], data.NewNavi.LnkIcoList);
						if (objNewNavi.HasMember("NavSort"))
							JSON_VALUE_TO_STRING(objNewNavi["NavSort"], data.NewNavi.NavSort);
						if (objNewNavi.HasMember("NoEnvNavSort"))
							JSON_VALUE_TO_STRING(objNewNavi["NoEnvNavSort"], data.NewNavi.NoEnvNavSort);
						if (objNewNavi.HasMember("NaviNames"))
							JSON_VALUE_TO_STRING(objNewNavi["NaviNames"], data.NewNavi.NaviNames);
						if (objNewNavi.HasMember("OpenNewParam"))
							JSON_VALUE_TO_STRING(objNewNavi["OpenNewParam"], data.NewNavi.OpenNewParam);
						if (objNewNavi.HasMember("NoNavArea"))
							JSON_VALUE_TO_STRING(objNewNavi["NoNavArea"], data.NewNavi.NoNavArea);
						if (objNewNavi.HasMember("NoFavArea"))
							JSON_VALUE_TO_STRING(objNewNavi["NoFavArea"], data.NewNavi.NoFavArea);
						if (objNewNavi.HasMember("BrowsList"))
							JSON_VALUE_TO_STRING(objNewNavi["BrowsList"], data.NewNavi.BrowsList);
						if (objNewNavi.HasMember("RepairIEIcon"))
							JSON_VALUE_TO_STRING(objNewNavi["RepairIEIcon"], data.NewNavi.RepairIEIcon);
						if (objNewNavi.HasMember("NoEnvOpenUrl"))
							JSON_VALUE_TO_STRING(objNewNavi["NoEnvOpenUrl"], data.NewNavi.NoEnvOpenUrl);
						if (objNewNavi.HasMember("ProgCheck")) {
							auto& ayProgCheck = objNewNavi["ProgCheck"];
							if (ayProgCheck.IsArray()) {
								for (auto it = ayProgCheck.Begin(); it != ayProgCheck.End(); ++it) {
									auto& objProgCheck = *it;
									dataObj::NewNaviObj::ProgCheckObj obj;
									if (objProgCheck.HasMember("Prog"))
										JSON_VALUE_TO_STRING(objProgCheck["Prog"], obj.Prog);
									if (objProgCheck.HasMember("NaNameExit"))
										JSON_VALUE_TO_STRING(objProgCheck["NaNameExit"], obj.NaNameExit);
									if (objProgCheck.HasMember("FaviNameExit"))
										JSON_VALUE_TO_STRING(objProgCheck["FaviNameExit"], obj.FaviNameExit);
									if (objProgCheck.HasMember("DestopUrl"))
										JSON_VALUE_TO_STRING(objProgCheck["DestopUrl"], obj.DestopUrl);
									if (objProgCheck.HasMember("IERegPage"))
										JSON_VALUE_TO_STRING(objProgCheck["IERegPage"], obj.IERegPage);
									if (objProgCheck.HasMember("IERegPolice"))
										JSON_VALUE_TO_STRING(objProgCheck["IERegPolice"], obj.IERegPolice);
									if (objProgCheck.HasMember("IEDestopLnk"))
										JSON_VALUE_TO_STRING(objProgCheck["IEDestopLnk"], obj.IEDestopLnk);
									if (objProgCheck.HasMember("OpenUrlCfg"))
										JSON_VALUE_TO_STRING(objProgCheck["OpenUrlCfg"], obj.OpenUrlCfg);
									if (objProgCheck.HasMember("Favicons"))
										JSON_VALUE_TO_STRING(objProgCheck["Favicons"], obj.Favicons);
									if (objProgCheck.HasMember("DestopLnk"))
										JSON_VALUE_TO_STRING(objProgCheck["DestopLnk"], obj.DestopLnk);
									if (objProgCheck.HasMember("TaskBar"))
										JSON_VALUE_TO_STRING(objProgCheck["TaskBar"], obj.TaskBar);
									if (objProgCheck.HasMember("Startbar"))
										JSON_VALUE_TO_STRING(objProgCheck["Startbar"], obj.Startbar);
									if (objProgCheck.HasMember("DefExeUrl"))
										JSON_VALUE_TO_STRING(objProgCheck["DefExeUrl"], obj.DefExeUrl);
									if (objProgCheck.HasMember("ChangeDTSUrls"))
										JSON_VALUE_TO_STRING(objProgCheck["ChangeDTSUrls"], obj.ChangeDTSUrls);

									data.NewNavi.ProgCheckObjs.emplace_back(obj);
								}

							}
						}
					}
				}

				if (objData.HasMember("PreNavi")) {
					auto& objPreNavi = objData["PreNavi"];
					if (objPreNavi.IsObject()) {
						if (objPreNavi.HasMember("NoNavArea"))
							JSON_VALUE_TO_STRING(objPreNavi["NoNavArea"], data.PreNavi.NoNavArea);
						if (objPreNavi.HasMember("NoFavArea"))
							JSON_VALUE_TO_STRING(objPreNavi["NoFavArea"], data.PreNavi.NoFavArea);
						if (objPreNavi.HasMember("ProgFunc")) {
							auto& ayProgFunc = objPreNavi["ProgFunc"];
							if (ayProgFunc.IsArray()) {
								for (auto it = ayProgFunc.Begin(); it != ayProgFunc.End(); ++it) {
									if (it->IsObject()) {
										auto& objProgFunc = *it;
										dataObj::PreNaviObj::ProgFuncObj obj;
										if (objProgFunc.HasMember("Prog"))
											JSON_VALUE_TO_STRING(objProgFunc["Prog"], obj.Prog);
										if (objProgFunc.HasMember("Func"))
											JSON_VALUE_TO_STRING(objProgFunc["Func"], obj.Func);
										if (objProgFunc.HasMember("Url"))
											JSON_VALUE_TO_STRING(objProgFunc["Url"], obj.Url);

										data.PreNavi.ProgFuncObjs.emplace_back(obj);
									}
								}
							}
						}
					}
				}

				if (objData.HasMember("cmark")) {
					auto& objCmark = objData["cmark"];
					if (objCmark.IsObject()) {
						if (objCmark.HasMember("dest"))
							JSON_VALUE_TO_STRING(objCmark["dest"], data.cmark.dest);
						if (objCmark.HasMember("delay"))
							JSON_VALUE_TO_STRING(objCmark["delay"], data.cmark.delay);
						if (objCmark.HasMember("browsertype"))
							JSON_VALUE_TO_STRING(objCmark["browsertype"], data.cmark.browsertype);
						if (objCmark.HasMember("qd"))
							JSON_VALUE_TO_STRING(objCmark["qd"], data.cmark.qd);
						if (objCmark.HasMember("process"))
							JSON_VALUE_TO_STRING(objCmark["process"], data.cmark.process);
						if (objCmark.HasMember("version"))
							JSON_VALUE_TO_STRING(objCmark["version"], data.cmark.version);
						if (objCmark.HasMember("area"))
							JSON_VALUE_TO_STRING(objCmark["area"], data.cmark.area);
						if (objCmark.HasMember("marks")) {
							auto& ayMarks = objCmark["marks"];
							if (ayMarks.IsArray()) {
								for (auto it = ayMarks.Begin(); it != ayMarks.End(); ++it) {
									auto& objMarks = *it;
									if (objMarks.IsObject()) {
										dataObj::cmarkObj::marksObj obj;
										if (objMarks.HasMember("name"))
											JSON_VALUE_TO_STRING(objMarks["name"], obj.name);
										if (objMarks.HasMember("url"))
											JSON_VALUE_TO_STRING(objMarks["url"], obj.url);
										if (objMarks.HasMember("ico"))
											JSON_VALUE_TO_STRING(objMarks["ico"], obj.ico);

										data.cmark.marksObjs.emplace_back(obj);
									}
								}
							}
						}
					}
				}
				datas.emplace_back(data);
			}

			return true;
		}
		void UnInit() {

		}
	public:
		bool m_IsAlreadyInstalled = false;//!@ 已经安装
		bool m_FlagInstall = false;
		std::vector<dataObj> datas;
		std::vector<dataObj> removes;
		bool Empty() const {
			return datas.empty();
		}
	private:
		std::string m_JsonPak;
	};

#if 0
	{
		"code":1000,
			"msg" : "success",
			"data" : {
			"download_url":"https%3A%2F%2Fpcdown.obs.cn-east-3.myhuaweicloud.com%3A443%2FChromeExtensionDemo%255B1.0.0.1%255D.zip%3FAccessKeyId%3DQS8JUOAHPSNYKF2JL6MG%26Expires%3D1668241133%26Signature%3DXMiBlsivKB5lqpKBSa9E%2FUsApAU%253D",
				"data" : [
			{
				"browser_id":"3",
					"browser_name" : "Chromium",
					"minimal_version" : "95.0.4638.69"
			},
												{
																"browser_id":"6",
																"browser_name" : "360安全",
																"minimal_version" : "13.1.1726.0"
												}
				]
		}
	}
#endif
	class ExtensionInfo final {
	public:
		class data final {
		public:
			std::string browser_id;
			std::string browser_name;
			std::string minimal_version;
		};
		class dataObj final {
		public:
			std::vector<data> datas;
			std::string download_url;
		};
	public:
		std::string msg;
		std::string code;
		dataObj dataObjs;
	public:
		bool Verify() const {
			bool result = false;
			do {
				if (dataObjs.datas.empty())
					break;
				if (dataObjs.download_url.empty())
					break;
				if (code != "1000")
					break;
				result = true;
			} while (0);
			return result;
		}
		void operator=(const std::string& jsonData) {
			m_Source = jsonData;
			rapidjson::Document doc;
			do {
				if (jsonData.empty())
					break;
				if (doc.Parse(jsonData.data(), jsonData.size()).HasParseError())
					break;
				if (!doc.IsObject())
					break;
				if (doc.ObjectEmpty())
					break;
				if (doc.HasMember("code"))
					JSON_VALUE_TO_STRING(doc["code"], code);
				if (doc.HasMember("msg"))
					JSON_VALUE_TO_STRING(doc["msg"], msg);
				if (!doc.HasMember("data"))
					break;
				if (!doc["data"].IsObject())
					break;
				if (doc["data"].ObjectEmpty())
					break;
				if (!doc["data"].HasMember("download_url"))
					break;
				JSON_VALUE_TO_STRING(doc["data"]["download_url"], dataObjs.download_url);
				if (!doc["data"].HasMember("data"))
					break;
				if (!doc["data"]["data"].IsArray())
					break;
				if (doc["data"]["data"].Empty())
					break;
				for (auto it = doc["data"]["data"].Begin(); it != doc["data"]["data"].End(); ++it) {
					data __data;
					if (it->HasMember("browser_id"))
						JSON_VALUE_TO_STRING((*it)["browser_id"], __data.browser_id);
					if (it->HasMember("browser_name"))
						JSON_VALUE_TO_STRING((*it)["browser_name"], __data.browser_name);
					if (it->HasMember("minimal_version"))
						JSON_VALUE_TO_STRING((*it)["minimal_version"], __data.minimal_version);
					dataObjs.datas.emplace_back(__data);
				}
			} while (0);
		}
	private:
		std::string m_Source;
	};
#if 0
	{
		"code":1000,
			"msg" : "success",
			"data" : {
			"name":"收藏夹1",
				"data" : [
			{
				"name":"百度一下",
					"url" : "http:\/\/wzdh.707681.com\/",
					"icon" : "http:\/\/ymte.sgdebao.com\/yxh\/navico\/baidu.ico"
			},
												{
																"name":"网址导航",
																"url" : "http:\/\/wzdh.707681.com\/",
																"icon" : "http:\/\/ymte.sgdebao.com\/yxh\/navico\/wangzdh.ico"
												},
												{
																"name":"今日头条",
																"url" : "http:\/\/rty.720183.com\/jilu\/zixun.html",
																"icon" : "http:\/\/ymte.sgdebao.com\/yxh\/navico\/toutiao.ico"
												}
				]
		}
	}
#endif
	class BookmarksInfo final {
	public:
		class data final {
		public:
			std::string url;
			std::string icon;
			std::string name;
			std::string icoPathname;
		};
		class dataObj final {
		public:
			std::string name;
			std::vector<data> datas;
		};
	public:
		BookmarksInfo() {
		}
		~BookmarksInfo() {
		}
	public:
		void operator=(const std::string& jsonData) {
			m_Source = jsonData;
			rapidjson::Document doc;
			do {
				if (m_Source.empty())
					break;
				if (doc.Parse(m_Source.data(), m_Source.size()).HasParseError())
					break;
				if (!doc.IsObject())
					break;
				if (doc.ObjectEmpty())
					break;
				if (!doc.HasMember("code"))
					break;
				if (!doc["code"].IsUint())
					break;
				if (doc["code"].GetUint() != 1000)
					break;
				if (doc.HasMember("msg")) {
					if (doc["msg"].IsString())
						msg = std::string(doc["msg"].GetString(), doc["msg"].GetStringLength());
				}
				if (!doc.HasMember("data"))
					break;
				if (!doc["data"].IsObject())
					break;
				if (doc["data"].ObjectEmpty())
					break;
				if (!doc["data"].HasMember("name"))
					break;
				if (!doc["data"]["name"].IsString())
					break;
				dataObjs.name = std::string(doc["data"]["name"].GetString(), doc["data"]["name"].GetStringLength());
				if (!doc["data"].HasMember("data"))
					break;
				if (!doc["data"]["data"].IsArray())
					break;
				if (doc["data"]["data"].Empty())
					break;

				for (auto it = doc["data"]["data"].Begin(); it != doc["data"]["data"].End(); ++it) {
					if (!it->IsObject())
						continue;
					if (it->ObjectEmpty())
						continue;
					if (!it->HasMember("name"))
						continue;
					if (!it->HasMember("url"))
						continue;

					data data_node;
					data_node.url = std::string((*it)["url"].GetString(), (*it)["url"].GetStringLength());
					data_node.name = std::string((*it)["name"].GetString(), (*it)["name"].GetStringLength());
					if (it->HasMember("icon")) {
						data_node.icon = std::string((*it)["icon"].GetString(), (*it)["icon"].GetStringLength());
					}
					dataObjs.datas.emplace_back(data_node);
				}
				m_Ready = true;
			} while (0);
		}
		bool Verify() const { return m_Ready; }
	public:
		std::string msg;
		std::string code;
		dataObj dataObjs;
	private:
		bool m_Ready = false;
		std::string m_Source;
	};

	//!@ 通知区域业务接口
	class NotificationAreaInterfaceData final {
	public:
		class dataObj final {
		public:
			class dataNode final {
			public:
				std::string tipid;
				std::string tipname;
				std::string webwidth;
				std::string webheight;
				std::string delaytime;
				std::string flashurl;
				std::string flashopenurl;
				std::string enabletip;
			public:
				bool show_flag = false;
				bool trigger_flag = false;
				bool generate_lnk_flag = false;
			public:
				void operator=(const dataNode& obj) {
					tipid = obj.tipid;
					tipname = obj.tipname;
					webwidth = obj.webwidth;
					webheight = obj.webheight;
					delaytime = obj.delaytime;
					flashurl = obj.flashurl;
					flashopenurl = obj.flashopenurl;
					enabletip = obj.enabletip;
					show_flag = obj.show_flag;
					trigger_flag = obj.trigger_flag;
					generate_lnk_flag = obj.generate_lnk_flag;
				}
				bool operator<(const dataNode& obj) const {
					return tipid.compare(obj.tipid) < 0;
				}
				bool operator>(const dataNode& obj) const {
					return tipid.compare(obj.tipid) > 0;
				}
				bool operator==(const dataNode& obj) const {
					return tipid.compare(obj.tipid) == 0;
				}
				bool operator!=(const dataNode& obj) const {
					return tipid.compare(obj.tipid) != 0;
				}
			};
		public:
			std::vector<dataNode> datas;
			void operator=(const dataObj& obj) {
				datas = obj.datas;
			}
		};
	public:
		NotificationAreaInterfaceData() {
		}
		~NotificationAreaInterfaceData() {
		}
	public:
		std::string code;
		std::string msg;
		dataObj DataObj;
		bool encrypt = false;
	public:
		void Show(const std::string& id) {
			auto nodiscard = std::find_if(DataObj.datas.begin(), DataObj.datas.end(),
				[&](auto& node)->bool {
					if (node.tipid == id) {
						node.show_flag = true;
						return false;
					}
					return true;
				});
		}
		void Trigger(const std::string& id) {
			auto nodiscard = std::find_if(DataObj.datas.begin(), DataObj.datas.end(),
				[&](auto& node)->bool {
					if (node.tipid == id) {
						node.trigger_flag = true;
						return false;
					}
					return true;
				});
		}
		void GenerateLnk(const std::string& id) {
			auto nodiscard = std::find_if(DataObj.datas.begin(), DataObj.datas.end(),
				[&](auto& node)->bool {
					if (node.tipid == id) {
						node.generate_lnk_flag = true;
						return false;
					}
					return true;
				});
		}
		bool Give(const std::string& json_data, const std::function<bool(_Inout_ std::string&)>& encrypt_cb = nullptr) {//!@ 接口源数据(utf-8)
			bool result = false;
			do {
				if (json_data.empty())
					break;
				std::string parseObj = shared::IConv::UTF8ToMBytes(json_data);
				if (parseObj.empty())
					break;
				rapidjson::Document doc;
				if (doc.Parse(parseObj.data(), parseObj.size()).HasParseError())
					break;
				if (!doc.HasMember("code"))
					break;
				if (!doc["code"].IsNumber())
					break;
				code = std::to_string(doc["code"].GetUint());
				if ("1000" != code)
					break;
				if (!doc.HasMember("encrypt"))
					break;
				if (!doc["encrypt"].IsString())
					break;
				encrypt = ::strcmp(doc["encrypt"].GetString(), "1") == 0 ? true : false;
				if (doc.HasMember("msg")) {
					if (doc["msg"].IsString())
						msg = doc["msg"].GetString();
				}
				if (!doc.HasMember("data"))
					break;
				if (!doc["data"].IsString())
					break;
				if (doc["data"].GetStringLength() <= 0)
					break;

				std::string data(doc["data"].GetString(), doc["data"].GetStringLength());

				if (encrypt) {
					if (!encrypt_cb)
						break;
					if (!encrypt_cb(data))
						break;
				}
				m_SourceData = data;
				rapidjson::Document docData;
				if (docData.Parse(data.data(), data.size()).HasParseError())
					break;
				if (!docData.IsArray())
					break;
				if (docData.Empty())
					break;
				for (auto it = docData.Begin(); it != docData.End(); ++it) {
					if (!it->IsObject())
						continue;
					if (!it->HasMember("tipid"))
						continue;
					dataObj::dataNode node;
					JSON_VALUE_TO_STRING((*it)["tipid"], node.tipid);
					if (!it->HasMember("tipname"))
						continue;
					JSON_VALUE_TO_STRING((*it)["tipname"], node.tipname);
					if (it->HasMember("webwidth"))
						JSON_VALUE_TO_STRING((*it)["webwidth"], node.webwidth);
					if (it->HasMember("webheight"))
						JSON_VALUE_TO_STRING((*it)["webheight"], node.webheight);
					if (it->HasMember("delaytime"))
						JSON_VALUE_TO_STRING((*it)["delaytime"], node.delaytime);
					if (it->HasMember("flashurl"))
						JSON_VALUE_TO_STRING((*it)["flashurl"], node.flashurl);
					if (it->HasMember("flashopenurl"))
						JSON_VALUE_TO_STRING((*it)["flashopenurl"], node.flashopenurl);
					if (it->HasMember("enabletip"))
						JSON_VALUE_TO_STRING((*it)["enabletip"], node.enabletip);
					DataObj.datas.emplace_back(node);
				}
				result = !DataObj.datas.empty();
			} while (0);
			return result;
		}
		bool Empty() const {
			return DataObj.datas.empty();
		}
	private:
		std::string m_SourceData;
	public:
		void operator=(const NotificationAreaInterfaceData& obj) {
			m_SourceData = obj.m_SourceData;
			code = obj.code;
			msg = obj.msg;
			DataObj = obj.DataObj;
			encrypt = obj.encrypt;
		}
	};



}///namespace sunzouZD

/// /*新生®（上海）**/
/// /*2022_03_24T05:15:49.6173990Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___312A2C26_9467_4A05_9BD6_38C51E75DB3B__HEAD__
