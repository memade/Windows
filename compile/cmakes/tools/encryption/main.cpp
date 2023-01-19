#include <win.h>
#include <encryption.hpp>

enum class EnArgvType : int {
	EN_ARGVTYPE_UNDEFINED = 0,
	EN_ARGVTYPE_ENCRYPTTYPE = 1,
	EN_ARGVTYPE_SRC_PATHNAME = 2,
	EN_ARGVTYPE_DEST_PATHNAME = 3,
};
enum class EncryptType : std::uint64_t {
	ENCRYPT_TYPE_UNDEFINED = 0,
	ENCRYPT_TYPE_MEMADE = 1,
	ENCRYPT_TYPE_RSA = ENCRYPT_TYPE_MEMADE << 1,
	ENCRYPT_TYPE_POWERSHELL_BASE64 = ENCRYPT_TYPE_MEMADE << 2,//@ in Ascii out Unicode and base64
	ENCRYPT_TYPE_C = ENCRYPT_TYPE_MEMADE << 3,
};

static bool operator&(const EncryptType& type1, const EncryptType& type2) {
	return static_cast<std::uint64_t>(type1) & static_cast<std::uint64_t>(type2);
}


int main(int argc, char** argv) {

	std::string srcPathname, destPathname;
	EncryptType encryptType = EncryptType::ENCRYPT_TYPE_UNDEFINED;

	for (int i = 0; i < argc; ++i) {
		switch (EnArgvType(i)) {
		case EnArgvType::EN_ARGVTYPE_UNDEFINED: {

		}break;
		case EnArgvType::EN_ARGVTYPE_ENCRYPTTYPE: {
			encryptType = EncryptType(::atoll(argv[i]));
		}break;
		case EnArgvType::EN_ARGVTYPE_SRC_PATHNAME: {
			srcPathname = argv[i];
		}break;
		case EnArgvType::EN_ARGVTYPE_DEST_PATHNAME: {
			destPathname = argv[i];
		}break;
		default:
			break;
		}
	}

	std::string FinalData = shared::Win::File::Read(srcPathname);
	if (FinalData.empty()) {
		std::cout << "load(" << srcPathname << ") error." << std::endl;
		return -1;
	}
	//!@ 需要加密
	if (encryptType & EncryptType::ENCRYPT_TYPE_MEMADE) {
		do {
			if (srcPathname.empty() || destPathname.empty()) {
				std::cout << std::format("Tool(Encrypt) type({}) parameter error.", int(encryptType)) << std::endl;
				break;
			}
			::remove(destPathname.c_str());
			std::cout << "remove(" << destPathname << ")" << std::endl;
			std::string enFileData = shared::Encryption::WemadeEncode(FinalData);
			FinalData = enFileData;
			shared::Win::CreateDirectoryA(shared::Win::GetPathByPathnameA(destPathname));
			if (!shared::Win::File::Write(destPathname, FinalData)) {
				std::cout << "Encrypt failed." << std::endl;
				break;
			}
		} while (0);
	}

	if (encryptType & EncryptType::ENCRYPT_TYPE_RSA) {



	}
	


	//!@ 生成C头文件
	if ((encryptType & EncryptType::ENCRYPT_TYPE_C) && !FinalData.empty()) {
		std::string filename = shared::Win::GetNameByPathnameA(destPathname);
		for (auto it = filename.begin(); it != filename.end(); ++it) {
			char& ch = *it;
			if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')))
				ch = '_';
		}
		if (!filename.empty()) {
			if (filename[0] >= '0' && filename[0] <= '9')
				filename.insert(0, "_");
		}
		std::string final_buffer = shared::Win::GenerateHexBuffer(FinalData, filename, ",");
		if (!final_buffer.empty()) {
			shared::Win::File::Write(destPathname + ".h", final_buffer);
		}
	}


	std::cout << "Encrypt " << std::endl;
	std::cout << " Form " << srcPathname << std::endl;
	std::cout << " To " << destPathname << std::endl;
	std::cout << "Process success." << std::endl;

	return 0;
}
