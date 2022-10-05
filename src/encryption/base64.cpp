#include "stdafx.h"
namespace shared {

	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	static inline bool is_base64(unsigned char c)
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	Encryption::base64::base64()
	{

	}

	Encryption::base64::~base64()
	{

	}

	std::string Encryption::base64::base64_encode(const std::string& src)
	{
		const unsigned char* bytes_to_encode = reinterpret_cast<const unsigned char*>(src.data());
		unsigned int in_len = static_cast<unsigned int>(src.size());
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;
				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}
		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;
			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];
			while ((i++ < 3))
				ret += '=';
		}
		return ret;
	}
	bool Encryption::base64::base64_decode(const unsigned char* in_data, const size_t& in_data_size, unsigned char* out_buffer, const size_t& out_buffer_size) {
		if (!in_data || !out_buffer || out_buffer_size <= 0 || in_data_size <= 0)
			return false;
		::memset(out_buffer, 0x00, out_buffer_size);

		int in_len = static_cast<int>(in_data_size);
		if (in_len <= 0)
			return false;
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		while (in_len-- && (in_data[in_] != '=') && is_base64(in_data[in_])) {
			char_array_4[i++] = in_data[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
				for (i = 0; (i < 3); i++)
					out_buffer += char_array_3[i];
				i = 0;
			}
		}
		if (i) {
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;
			for (j = 0; j < 4; j++)
				char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
			for (j = 0; (j < i - 1); j++) out_buffer += char_array_3[j];
		}











		//std::string inData(in_data, in_data_size);
		//auto outData = base64_decode(inData);
		//if (outData.size() > out_buffer_size)
		//	return false;
		//::memcpy(out_buffer, outData.data(), __min(out_buffer_size, outData.size()));
		return true;
	}
	bool Encryption::base64::base64_decode(const std::string& encoded_string, std::string& decoded_string) {
		decoded_string.clear();
		if (encoded_string.empty())
			return false;
		int in_len = static_cast<int>(encoded_string.size());
		if (in_len <= 0)
			return false;
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
				for (i = 0; (i < 3); i++)
					decoded_string += char_array_3[i];
				i = 0;
			}
		}
		if (i) {
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;
			for (j = 0; j < 4; j++)
				char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
			for (j = 0; (j < i - 1); j++) decoded_string += char_array_3[j];
		}
		return true;
	}
	std::string Encryption::base64::base64_decode(const std::string& encoded_string)
	{
		int in_len = static_cast<int>(encoded_string.size());
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;
		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}
		if (i) {
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;
			for (j = 0; j < 4; j++)
				char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}
		return ret;
	}

}///namespace shared