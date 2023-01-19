#include <win.h>
#include <zip++.h>
#include <openssl.hpp>
#include <encryption.hpp>

enum class ActionType : std::uint64_t {
	UNDEFINED = 0,
	MEMADE = 1,
	RSA = MEMADE << 1,
	BASE64 = MEMADE << 2,
};

static bool operator&(const ActionType& type1, const ActionType& type2) {
	return static_cast<std::uint64_t>(type1) & static_cast<std::uint64_t>(type2);
}

const char CMD_PARAM_TYPE[] = R"(--type)";
const char CMD_PARAM_C_BUFFER[] = R"(--cbuffer)";
const char CMD_PARAM_INPUT[] = R"(--input)";
const char CMD_PARAM_PWD[] = R"(--pwd)";
const char CMD_PARAM_KEY_PUBLIC[] = R"(--key-public)";
const char CMD_PARAM_KEY_PRIVATE[] = R"(--key-private)";
const char CMD_PARAM_OUTPUT[] = R"(--output)";

int main(int argc, char** argv) {
	std::map<std::string, std::string> InputParameters;
	shared::Win::ParseCommandLineParameters(argc, argv, InputParameters);

	ActionType type = static_cast<ActionType>(::strtoull(InputParameters[CMD_PARAM_TYPE].c_str(), nullptr, 10));
	switch (type) {
	case ActionType::MEMADE: {
	}break;
	case ActionType::BASE64: {
	}break;
	case ActionType::RSA: {
		//!@ 如果存在公钥就用公钥加密 如果不存在公钥则用私密加密
		//!@ PKCS8
		std::string key_private_buffer, key_public_buffer, pwd_buffer, output_buffer, input_buffer;
		key_private_buffer = shared::Win::File::Read(InputParameters[CMD_PARAM_KEY_PRIVATE]);
		key_public_buffer = shared::Win::File::Read(InputParameters[CMD_PARAM_KEY_PUBLIC]);
		pwd_buffer = InputParameters[CMD_PARAM_PWD];
		input_buffer = shared::Win::File::Read(InputParameters[CMD_PARAM_INPUT]);
		if (input_buffer.empty())
			break;
		if (key_private_buffer.empty() && key_public_buffer.empty())
			break;	
		shared::Openssl::Rsa::Encode(shared::Openssl::Rsa::EnType::EN_TYPE_PKCS8, pwd_buffer, key_public_buffer, key_private_buffer, input_buffer, output_buffer);
		std::string output_pathname = InputParameters[CMD_PARAM_OUTPUT];
		if (output_pathname.empty())
			break;
		std::string output_path = shared::Win::GetPathByPathnameA(output_pathname);
		if (!shared::Win::CreateDirectoryA(output_path))
			break;
		shared::Win::File::Write(output_pathname, output_buffer);

#ifdef DEBUG
		//!@ 只能用公钥解密
		std::string read_buffer = shared::Win::File::Read(output_pathname);
		std::string decode_buffer;
		shared::Openssl::Rsa::Decode(shared::Openssl::Rsa::EnType::EN_TYPE_PKCS8, pwd_buffer, key_public_buffer, "", read_buffer, decode_buffer);
		auto __break = 0;
#endif // DEBUG
	}break;
	default:
		break;
	}
#if 0
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
#endif
	return 0;
}
