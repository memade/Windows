#if !defined(INC_H___5DEA8633_C188_4F24_9635_29C3BD7C3683__HEAD__)
#define INC_H___5DEA8633_C188_4F24_9635_29C3BD7C3683__HEAD__

#include <string>
#include <memory>
#include <io.h>
#include <direct.h>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <zlib-1.2.11/zlib.h>
#include <zlib-1.2.11/build/zconf.h>
#include <libzippp/src/libzippp.h>

namespace shared {

	class ZipWriteNode final {
	public:
		std::string Filename;
		std::string FilePathname;
		std::string Filebuffer;
		std::string ReleaseRelativePathname;
		void operator=(const ZipWriteNode& obj) {
			Filename = obj.Filename;
			Filebuffer = obj.Filebuffer;
			FilePathname = obj.FilePathname;
			ReleaseRelativePathname = obj.ReleaseRelativePathname;
		}
	};

	class Zip final {
	public:
		Zip() {}
		~Zip() = delete;
	public:
		static bool IsZipCompress(const std::string& buffer);
		static int zipCompress(__in const std::string& src, __out std::string& dest) noexcept;
		static int zipUnCompress(__in const std::string& src, __in const size_t& nraw, __out std::string& dest) noexcept;
	public:
		static bool gzipCompress(const std::string& src, std::string& dest, int level = -1);
		static bool gzipUnCompress(const std::string& src, std::string& dest);
		static bool zipFileUnCompress(const std::string& buffer, const std::string& out_path);
		static bool zipPakUnCompress(const std::string& zipBuffer, const std::string& out_pathname);
		static bool zipPakUnCompressFirstToBuffer(const std::string& zipBuffer, std::string& out_buffer);
		static bool zipPakUnCompressToBuffer(const std::string& zipBuffer, const std::string& identify, std::string& out_buffer);
		static bool zipBufferCompress(const std::string& zipBuffer, std::string& zipFinish);
		static bool zipBufferUnCompress(const std::string& zipBuffer, const std::function<bool(const std::string&, const std::string&, bool&)>& uncompress_cb, const std::string& outputDir = "");
	private://!@ GZip
		static size_t GZGetBound(const size_t& sourceLen);
		static int GZCompress(unsigned char* pSrc, unsigned long nSrc, unsigned char* pDest, unsigned long* nDest);
		static int GZUnCompress(unsigned char* pSrc, unsigned long nSrc, unsigned char* pDest, unsigned long* nDest);
	};

}///namespace shared


#pragma comment(lib,"libzip-1.7.3.lib")
#pragma comment(lib,"libzippp.lib")
#pragma comment(lib,"zlib-1.2.11.lib")
#pragma comment(lib,"zip++.lib")
/// /*新生®（上海）**/
/// /*2022_08_14T22:52:15.7271653Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___5DEA8633_C188_4F24_9635_29C3BD7C3683__HEAD__
