#include "stdafx.h"
#if defined _M_IX86
#elif defined _M_X64
#else
#endif

#pragma warning(disable : 4267)

#define CHUNK 16384
#define windowBits 15
#define GZIP_ENCODING 16
#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
#define Z_BLOCK         5
#define Z_TREES         6
/* Allowed flush values; see deflate() and inflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
		/* Return codes for the compression/decompression functions. Negative values
		* are errors, positive values are used for special but normal events.
		*/
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
		/* compression levels */
#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_RLE                 3
#define Z_FIXED               4
#define Z_DEFAULT_STRATEGY    0
		/* compression strategy; see deflateInit2() below for details */
#define Z_BINARY   0
#define Z_TEXT     1
#define Z_ASCII    Z_TEXT   /* for compatibility with 1.2.2 and earlier */
#define Z_UNKNOWN  2
		/* Possible values of the data_type field for deflate() */
#define Z_DEFLATED   8
		/* The deflate compression method (the only one supported in this version) */
#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */
#ifndef MAX_WBITS
#  define MAX_WBITS   15 /* 32K LZ77 window */
#endif

namespace shared {

	static bool CreateDirectoryA(const std::string& create_directory_path) {
		if (create_directory_path.empty())
			return false;
		std::string temp_directory_path{ create_directory_path };
		do {
			if (::_access(temp_directory_path.c_str(), 0) == 0) {
				break;
			}
			for (auto it = temp_directory_path.begin(); it != temp_directory_path.end(); ++it) {
				if (*it == '/')
					*it = '\\';
			}
			do {
				auto found = temp_directory_path.find("\\\\");
				if (found == std::string::npos)
					break;
				temp_directory_path.replace(found, ::strlen("\\\\"), "\\");
			} while (1);

			std::vector<std::string> levels;
			do {
				if (::_access(temp_directory_path.c_str(), 0) == 0)
					break;
				if (temp_directory_path.empty())
					break;
				levels.emplace_back(temp_directory_path);
				temp_directory_path.pop_back();
				temp_directory_path = temp_directory_path.substr(0, temp_directory_path.rfind("\\"));
			} while (1);

			std::sort(levels.begin(), levels.end(),
				[&](const auto& obj1, const auto& obj2)->bool {
					return obj1.size() < obj2.size();
				});

			for (auto& level : levels) {
				if (_mkdir(level.c_str()) != 0)
					break;
			}
		} while (0);
		return ::_access(temp_directory_path.c_str(), 0) == 0;
	}

	bool Zip::IsZipCompress(const std::string& buffer) {
		if (buffer.empty())
			return false;
		if (buffer.size() < sizeof(__int64))
			return false;
		__int64 head = 0;
		::memcpy(&head, buffer.data(), sizeof(__int64));
		return  (head << (8 * 4)) >> (8 * 4) == 0x04034b50/*0x0000001404034b50*/;
	}

	int Zip::zipCompress(__in const std::string& src, __out std::string& dest) noexcept {
		dest.clear();
		uLongf nCompress = compressBound(static_cast<uLongf>(src.size()));
		Bytef* pCompress = new Bytef[nCompress];
		int ret = compress(pCompress, &nCompress, (Bytef*)src.data(), static_cast<uLong>(src.size()));
		switch (ret) {
		case Z_OK: {
			dest.append((char*)pCompress, nCompress);
		}break;
		case Z_MEM_ERROR: {//!如果内存不够
		}break;
		case Z_BUF_ERROR: {//!目标缓冲区太小
		}break;
		}
		delete[] pCompress;
		pCompress = nullptr;
		return ret;
	}

	int Zip::zipUnCompress(__in const std::string& src, __in const size_t& nraw, __out std::string& dest) noexcept {
		dest.clear();
		Bytef* pUnCompress = new Bytef[nraw];
		int ret = uncompress(pUnCompress, (uLongf*)&nraw, (Bytef*)src.data(), static_cast<uLong>(src.size()));
		switch (ret) {
		case Z_OK: {
			dest.append((char*)pUnCompress, nraw);
		}break;
		case Z_STREAM_ERROR: {
		}break;
		case Z_MEM_ERROR: {//!如果内存不够
		}break;
		case Z_BUF_ERROR: {//!目标缓冲区太小
		}break;
		}
		delete[] pUnCompress;
		pUnCompress = nullptr;
		return ret;
	}

	bool Zip::gzipCompress(const std::string& data, std::string& compressedData, int level /*= -1*/) {
		auto out = new unsigned char[CHUNK];
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		if (deflateInit2_(&strm, level, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, 0, ZLIB_VERSION, (int)sizeof(z_stream)/*Z_DEFAULT_STRATEGY*/) != Z_OK)
		{
			delete[] out;
			out = nullptr;
			return false;
		}
		strm.next_in = (unsigned char*)data.c_str();
		strm.avail_in = static_cast<unsigned int>(data.size());
		do {
			int have;
			strm.avail_out = CHUNK;
			strm.next_out = out;
			if (deflate(&strm, Z_FINISH) == Z_STREAM_ERROR)
			{
				delete[] out;
				out = nullptr;
				return false;
			}
			have = CHUNK - strm.avail_out;
			compressedData.append((char*)out, have);
		} while (strm.avail_out == 0);
		if (deflateEnd(&strm) != Z_OK)
		{
			delete[] out;
			out = nullptr;
			return false;
		}
		delete[] out;
		out = nullptr;
		return true;
	}
	bool Zip::gzipUnCompress(const std::string& compressedData, std::string& data) {
		int ret;
		unsigned have;
		z_stream strm;
		auto out = new unsigned char[CHUNK];
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		if (inflateInit2_(&strm, 16 + MAX_WBITS, ZLIB_VERSION, (int)sizeof(z_stream)) != Z_OK)
		{
			delete[] out;
			out = nullptr;
			return false;
		}
		strm.avail_in = static_cast<unsigned int>(compressedData.size());
		strm.next_in = (unsigned char*)compressedData.c_str();
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			switch (ret) {
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				delete[] out;
				out = nullptr;
				return false;
			}
			have = CHUNK - strm.avail_out;
			data.append((char*)out, have);
		} while (strm.avail_out == 0);

		if (inflateEnd(&strm) != Z_OK) {
			delete[] out;
			out = nullptr;
			return false;
		}
		delete[] out;
		out = nullptr;
		return true;
	}
	size_t Zip::GZGetBound(const size_t& sourceLen)
	{
		double len = (double)sourceLen;
		if (!len)
			return 0;
		if (len < 0)
			return 0;
		if (len > 250.0)
			len *= 1.5;
		else
			len *= 2.0;
		return (size_t)len;
	}

	int Zip::GZCompress(unsigned char* data, unsigned long ndata, unsigned char* zdata, unsigned long* nzdata)
	{
		z_stream c_stream;
		int err = 0;
		if (data && ndata > 0) {
			c_stream.zalloc = NULL;
			c_stream.zfree = NULL;
			c_stream.opaque = NULL;
			//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
			if (deflateInit2_(&c_stream, -1, 8, 15 + 16, 8, 0, ZLIB_VERSION, (int)sizeof(z_stream)) != 0) return -1;
			c_stream.next_in = data;
			c_stream.avail_in = ndata;
			c_stream.next_out = zdata;
			c_stream.avail_out = *nzdata;
			while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
				if (inflate(&c_stream, 0) != 0) return -1;
			}
			if (c_stream.avail_in != 0) return c_stream.avail_in;
			for (;;) {
				if ((err = inflate(&c_stream, 4)) == 1) break;
				if (err != 0) return -1;
			}
			if (deflateEnd(&c_stream) != 0) return -1;
			*nzdata = c_stream.total_out;
			return 0;
		}
		return -1;
	}
	int Zip::GZUnCompress(unsigned char* zdata, unsigned long nzdata, unsigned char* data, unsigned long* ndata)
	{
		int err = 0;
		z_stream d_stream = { 0 }; /* decompression stream */
		static char dummy_head[2] = {
			0x8 + 0x7 * 0x10,
			(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
		};
		d_stream.zalloc = NULL;
		d_stream.zfree = NULL;
		d_stream.opaque = NULL;
		d_stream.next_in = zdata;
		d_stream.avail_in = 0;
		d_stream.next_out = data;
		//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
		if (inflateInit2_(&d_stream, 15 + 16, ZLIB_VERSION, (int)sizeof(z_stream)) != 0) return -1;
		//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
		while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
			d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
			if ((err = inflate(&d_stream, 0)) == 1) break;
			if (err != 0) {
				if (err == -3) {
					d_stream.next_in = (unsigned char*)dummy_head;
					d_stream.avail_in = sizeof(dummy_head);
					if ((err = inflate(&d_stream, 0)) != 0) {
						return -1;
					}
				}
				else return -1;
			}
		}
		if (inflateEnd(&d_stream) != 0) return -1;
		*ndata = d_stream.total_out;
		return 0;
	}
	static std::string GetNameByPathname(const std::string& pathname) {
		std::string result;
		for (auto rit = pathname.rbegin(); rit != pathname.rend(); ++rit) {
			result.append(sizeof(char), *rit);
			if (*rit == '/' || *rit == '\\') {
				if (!result.empty())
					result.pop_back();
				break;
			}
		}

		std::reverse(result.begin(), result.end());
		return result;
	}
	static std::string GetPathByPathame(const std::string& pathname)
	{
		if (pathname.empty())
			return pathname;
		if (*std::prev(pathname.end()) == '\\' || *std::prev(pathname.end()) == '/')
			return pathname;
		std::string result(pathname);
		for (auto rit = result.rbegin(); rit != result.rend(); ++rit) {
			if (*rit == '/' || *rit == '\\') {
				result.resize(rit.base() - result.begin());
				break;
			}
		}
		if (!result.empty())
			result.push_back('/');
		return result;
	}
	bool Zip::zipPakUnCompressToBuffer(const std::string& zipBuffer, const std::string& identify, std::string& out_buffer) {
		bool result = false;
		out_buffer.clear();
		libzippp::ZipArchive* pZipObj = nullptr;
		do {
			if (zipBuffer.empty())
				break;
			pZipObj = libzippp::ZipArchive::fromBuffer(zipBuffer.data(), static_cast<libzippp_uint32>(zipBuffer.size()));
			if (!pZipObj || !pZipObj->isOpen())
				break;
			auto entries = pZipObj->getEntries();
			if (entries.size() <= 0)
				break;
			libzippp::ZipEntry target = pZipObj->getEntry(identify.c_str());
			if (target.isNull())
				break;
			if (!target.isFile())
				break;
			auto sizeTarget = target.getSize();
			if (sizeTarget <= 0)
				break;
			auto bin = target.readAsBinary();
			if (!bin)
				break;
			out_buffer.append((char*)bin, (size_t)target.getSize());
			::free(bin);
			bin = nullptr;
			result = !out_buffer.empty();
		} while (0);
		if (pZipObj) {
			pZipObj->close();
			delete pZipObj;
			pZipObj = nullptr;
		}
		return result;
	}
	bool Zip::zipPakUnCompressFirstToBuffer(const std::string& zipBuffer, std::string& out_buffer) {
		bool result = false;
		out_buffer.clear();
		libzippp::ZipArchive* pZipObj = nullptr;
		do {
			if (zipBuffer.empty())
				break;
			pZipObj = libzippp::ZipArchive::fromBuffer(zipBuffer.data(), static_cast<libzippp_uint32>(zipBuffer.size()));
			if (!pZipObj || !pZipObj->isOpen())
				break;
			auto entries = pZipObj->getEntries();
			if (entries.size() <= 0)
				break;
			libzippp::ZipEntry target = entries[0];
			if (target.isNull())
				break;
			if (!target.isFile())
				break;
			auto sizeTarget = target.getSize();
			if (sizeTarget <= 0)
				break;
			auto bin = target.readAsBinary();
			if (!bin)
				break;
			out_buffer.append((char*)bin, (size_t)target.getSize());
			::free(bin);
			bin = nullptr;
			result = !out_buffer.empty();
		} while (0);
		if (pZipObj) {
			pZipObj->close();
			delete pZipObj;
			pZipObj = nullptr;
		}
		return result;
	}
	bool Zip::zipPakUnCompress(const std::string& zipBuffer, const std::string& outPathanme) {
		bool result = false;
		libzippp::ZipArchive* pZipObj = nullptr;
		std::string strOutName{ GetNameByPathname(outPathanme) };
		std::string strOutPath{ GetPathByPathame(outPathanme) };
		do {
			if (strOutPath.empty())
				break;
			if (strOutName.empty())
				break;
			if (zipBuffer.empty())
				break;
			if (*std::prev(strOutPath.end()) != '\\' || *std::prev(strOutPath.end()) != '/')
				strOutPath.push_back('\\');
			CreateDirectoryA(strOutPath);
			::remove(outPathanme.c_str());
			pZipObj = libzippp::ZipArchive::fromBuffer(zipBuffer.data(), static_cast<libzippp_uint32>(zipBuffer.size()));
			if (!pZipObj || !pZipObj->isOpen())
				break;
			auto entries = pZipObj->getEntries();
			if (entries.size() <= 0)
				break;
			libzippp::ZipEntry target = pZipObj->getEntry(strOutName.c_str());
			if (target.isNull())
				break;
			if (!target.isFile())
				break;
			auto sizeTarget = target.getSize();
			if (sizeTarget <= 0)
				break;

			std::ofstream ofUnzippedFile(outPathanme, std::ios::out | std::ios::trunc | std::ios::binary);
			if (!ofUnzippedFile.is_open())
				continue;
			if (target.readContent(ofUnzippedFile, libzippp::ZipArchive::Current, 0) != LIBZIPPP_OK) {
			}
			ofUnzippedFile.close();
			result = _access(outPathanme.c_str(), 0) == 0;
		} while (0);
		if (pZipObj) {
			pZipObj->close();
			delete pZipObj;
			pZipObj = nullptr;
		}
		return result;
	}
	bool Zip::zipBufferCompress(const std::string& zipBuffer, std::string& zipFinish) {
		bool result = false;
		zipFinish.clear();
		do {
			if (zipBuffer.empty())
				break;
			libzippp::ZipArchive z1("identify.zip");
			z1.open(libzippp::ZipArchive::Write);
			z1.addData("session", zipBuffer.data(), zipBuffer.size());
			z1.close();
			std::ifstream ifs("identify.zip", std::ios::binary);
			ifs.seekg(0, std::ifstream::end);
			libzippp_uint32 bufferSize = (libzippp_uint32)ifs.tellg();
			zipFinish.resize(bufferSize, 0x00);
			ifs.seekg(std::ifstream::beg);
			ifs.read(&zipFinish[0], bufferSize);
			ifs.close();
			z1.unlink();
			result = true;
		} while (0);
		return result;
	}
	bool Zip::zipBufferUnCompress(const std::string& zipBuffer, const std::function<bool(const std::string&, const std::string&, bool&)>& uncompress_cb, const std::string& outputDir /*= ""*/) {
		bool result = false;
		libzippp::ZipArchive* pZipObj = nullptr;
		do {
			if (zipBuffer.empty() || !uncompress_cb)
				break;
			std::string strOutPath{ outputDir };
			if (!strOutPath.empty()) {
				if (*std::prev(strOutPath.end()) != '\\' || *std::prev(strOutPath.end()) != '/')
					strOutPath.push_back('\\');
				CreateDirectoryA(strOutPath);
				if (::_access(strOutPath.c_str(), 0) != 0)
					break;
			}
			pZipObj = libzippp::ZipArchive::fromBuffer(zipBuffer.data(), static_cast<libzippp_uint32>(zipBuffer.size()));
			if (!pZipObj || !pZipObj->isOpen())
				break;
			auto entries = pZipObj->getEntries();
			if (entries.size() <= 0)
				break;
			for (const libzippp::ZipEntry& entrie : entries) {
				std::string theTargetPathname;
				if (!strOutPath.empty()) {
					theTargetPathname = strOutPath + entrie.getName();
					if (entrie.isDirectory()) {
						CreateDirectoryA(GetPathByPathame(theTargetPathname));
						continue;
					}
				}
				std::uint64_t theTargetBuffeSize = entrie.getSize();
				if (theTargetBuffeSize <= 0)
					continue;
				void* bin = entrie.readAsBinary();
				if (!bin)
					continue;
				bool __break = false;
				bool release = uncompress_cb(entrie.getName(), std::string((char*)bin, (size_t)theTargetBuffeSize), __break);
				::free(bin);
				bin = nullptr;
				if (!theTargetPathname.empty() && release) {
					std::ofstream ofUnzippedFile(theTargetPathname, std::ios::out | std::ios::trunc | std::ios::binary);
					if (!ofUnzippedFile.is_open())
						continue;
					if (entrie.readContent(ofUnzippedFile, libzippp::ZipArchive::Current, 0) != LIBZIPPP_OK) {
					}
					ofUnzippedFile.close();
				}
				if (__break)
					break;
			}///for
			result = true;
		} while (0);
		if (pZipObj) {
			pZipObj->close();
			delete pZipObj;
			pZipObj = nullptr;
		}
		return result;
	}
	bool Zip::zipFileUnCompress(const std::string& zipBuffer, const std::string& outPath) {
		bool result = false;
		libzippp::ZipArchive* pZipObj = nullptr;
		std::string strOutPath{ outPath };
		do {
			if (strOutPath.empty())
				break;
			if (zipBuffer.empty())
				break;
			if (*std::prev(strOutPath.end()) != '\\' || *std::prev(strOutPath.end()) != '/')
				strOutPath.push_back('\\');
			CreateDirectoryA(strOutPath);

			pZipObj = libzippp::ZipArchive::fromBuffer(zipBuffer.data(), static_cast<libzippp_uint32>(zipBuffer.size()));
			if (!pZipObj || !pZipObj->isOpen())
				break;
			auto entries = pZipObj->getEntries();
			if (entries.size() <= 0)
				break;
			for (const libzippp::ZipEntry& entrie : entries) {
				auto theTargetPathname = strOutPath + entrie.getName();
				if (entrie.isDirectory()) {
					CreateDirectoryA(GetPathByPathame(theTargetPathname));
					continue;
				}
				auto theTargetBuffeSize = entrie.getSize();
				if (theTargetBuffeSize <= 0) {
					continue;
				}
				std::ofstream ofUnzippedFile(theTargetPathname, std::ios::out | std::ios::trunc | std::ios::binary);
				if (!ofUnzippedFile.is_open())
					continue;
				if (entrie.readContent(ofUnzippedFile, libzippp::ZipArchive::Current, 0) != LIBZIPPP_OK) {
				}
				ofUnzippedFile.close();
			}///for
			result = true;
		} while (0);
		if (pZipObj) {
			pZipObj->close();
			delete pZipObj;
			pZipObj = nullptr;
		}
		return result;
	}

}///namespace shared