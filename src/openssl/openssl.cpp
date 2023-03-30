#include "stdafx.h"

namespace shared {
	bool Openssl::MD5(const std::string input, std::string& output, const std::vector<std::uint8_t> seed /*= {}*/, const bool& hex_output /*= true*/, const bool& base64_output /*= false*/) {
		bool result = false;
		output.clear();
#if ENABLE_OPENSSL_3
		EVP_MD_CTX* ctx = nullptr;
		do {
			if ((ctx = EVP_MD_CTX_new()) == nullptr)
				break;
			if (1 != EVP_DigestInit_ex(ctx, EVP_md5(), nullptr))
				break;
			if (1 != EVP_DigestUpdate(ctx, input.empty() ? nullptr : &input[0], input.size()))
				break;
			output.resize(4096/*MD5_DIGEST_LENGTH*/, 0x00);
			unsigned int len = 0;
			if (1 != EVP_DigestFinal_ex(ctx, (std::uint8_t*)&output[0], &len)) {
				output.clear();
				break;
			}
			output.resize(len);
			result = true;
		} while (0);
		if (ctx)
			EVP_MD_CTX_free(ctx);
#else
		do {
			MD5_CTX md5Obj;
			if (!MD5_Init(&md5Obj))
				break;
			if (!seed.empty() && seed.size() == 16) {
				/*const std::uint8_t md5_init_seed[] = { 0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10 };*/
				size_t md5_calc_pos = 0;
				::memcpy(&md5Obj.A, &seed[md5_calc_pos], sizeof(unsigned int));
				md5_calc_pos += sizeof(unsigned int);
				::memcpy(&md5Obj.B, &seed[md5_calc_pos], sizeof(unsigned int));
				md5_calc_pos += sizeof(unsigned int);
				::memcpy(&md5Obj.C, &seed[md5_calc_pos], sizeof(unsigned int));
				md5_calc_pos += sizeof(unsigned int);
				::memcpy(&md5Obj.D, &seed[md5_calc_pos], sizeof(unsigned int));
			}
			output.resize(MD5_DIGEST_LENGTH, 0x00);
			if (!MD5_Update(&md5Obj, input.empty() ? nullptr : reinterpret_cast<const void*>(input.data()), input.size()))
				break;
			if (!MD5_Final((std::uint8_t*)&output[0], &md5Obj))
				break;
			result = true;
		} while (0);
#endif
		if (!result)
			output.clear();
		else {
			if (hex_output)
				output = shared::Win::BinToHex(output);
			else if (base64_output) {
				std::string base64;
				Base64Encode(output, base64);
				output = base64;
			}
		}
		return result;
	}
	bool Openssl::SHA256(const std::string& inData, const bool& isHex, std::string& outData) {
		bool result = false;
		outData.clear();
		std::uint8_t hash[SHA256_DIGEST_LENGTH] = { 0 };
		SHA256_CTX sha256 = { 0 };
		int sslret = 0;
		do {
			sslret = SHA256_Init(&sha256);
			if (1 != sslret)
				break;
			sslret = SHA256_Update(&sha256, inData.c_str(), inData.size());
			if (1 != sslret)
				break;
			sslret = SHA256_Final(hash, &sha256);
			if (1 != sslret)
				break;
			if (isHex) {
				std::stringstream ss;
				outData = shared::Win::BinToHex(std::string((char*)&hash[0], SHA256_DIGEST_LENGTH));
			}
			else {
				outData.append((char*)&hash[0], SHA256_DIGEST_LENGTH);
			}
			result = !outData.empty();
		} while (0);
		return result;
	}
	bool Openssl::HMAC_SHA1(const std::string& input, std::string& output, const std::string& seed, const bool& hex_output /*= true*/, const bool& base64_output /*= false*/) {
		bool result = false;
		output.clear();
#if ENABLE_OPENSSL_3
		do {

			result = true;
	} while (0);
#else
		do {
			if (seed.empty())
				break;
			HMAC_CTX hmacCtx = { 0 };
			if (!HMAC_Init_ex(&hmacCtx, (std::uint8_t*)&seed[0], static_cast<int>(seed.size()), EVP_sha1(), nullptr))
				break;
			if (!HMAC_Update(&hmacCtx, (const std::uint8_t*)input.data(), input.size()))
				break;
			std::uint8_t md[SHA_DIGEST_LENGTH] = { 0 };
			unsigned int md_size = 0;
			if (!HMAC_Final(&hmacCtx, md, &md_size) || md_size <= 0)
				break;
			HMAC_CTX_cleanup(&hmacCtx);
			if (md_size <= 0)
				break;
			output.append((char*)&md[0], md_size);
			result = true;
		} while (0);
#endif
		if (!result)
			output.clear();
		else {
			if (hex_output)
				output = shared::Win::BinToHex(output);
			else if (base64_output) {
				std::string base64;
				Base64Encode(output, base64);
				output = base64;
			}
		}
		return result;
}
	bool Openssl::HMAC_SHA256(const std::string& input, std::string& output, const std::string& seed, const bool& hex_output /*= true*/, const bool& base64_output /*= false*/) {
		bool result = false;
		output.clear();
#if ENABLE_OPENSSL_3
		do {

			result = true;
	} while (0);
#else
		do {
			if (seed.empty())
				break;
			HMAC_CTX hmacCtx = { 0 };
			if (!HMAC_Init_ex(&hmacCtx, (std::uint8_t*)&seed[0], static_cast<int>(seed.size()), EVP_sha256(), nullptr))
				break;
			if (!HMAC_Update(&hmacCtx, (const std::uint8_t*)input.data(), input.size()))
				break;
			std::uint8_t md[SHA256_DIGEST_LENGTH] = { 0 };
			unsigned int md_size = 0;
			if (!HMAC_Final(&hmacCtx, md, &md_size) || md_size <= 0)
				break;
			HMAC_CTX_cleanup(&hmacCtx);
			if (md_size <= 0)
				break;
			output.append((char*)md, md_size);
			result = true;
		} while (0);
#endif
		if (!result)
			output.clear();
		else {
			if (hex_output)
				output = shared::Win::BinToHex(output);
			else if (base64_output) {
				std::string base64;
				Base64Encode(output, base64);
				output = base64;
			}
		}
		return result;
	}
	bool Openssl::Base64Encode(const std::string& input, std::string& output, const bool& multiline /*= false*/) {
		bool result = false;
		output.clear();
		BIO* bmem = nullptr;
		BIO* b64 = nullptr;
		BUF_MEM* bptr = nullptr;
		do {
			if (input.empty())
				break;
			b64 = BIO_new(BIO_f_base64());
			if (!b64)
				break;
			if (!multiline)
				BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
			bmem = BIO_new(BIO_s_mem());
			if (!bmem)
				break;
			b64 = BIO_push(b64, bmem);
			if (!b64)
				break;
			long bio_write_length = BIO_write(b64, input.data(), static_cast<int>(input.size()));
			if (bio_write_length <= 0)
				break;
			auto sslret = BIO_flush(b64);
			if (sslret != 1)
				break;
			sslret = BIO_get_mem_ptr(b64, &bptr);
			if (sslret != 1)
				break;
			sslret = BIO_set_close(b64, BIO_NOCLOSE);
			if (sslret != 1)
				break;
			output.append(bptr->data, bptr->length);
			result = true;
		} while (0);
		if (b64) {
			BIO_free_all(b64);
			b64 = nullptr;
		}
		if (bptr) {
			BUF_MEM_free(bptr);
			bptr = nullptr;
		}
		return result;
	}
	bool Openssl::Base64Decode(const std::string& input, std::string& output, const bool& multiline /*= false*/) {
		bool result = false;
		output.clear();
		BIO* b64 = nullptr;
		BIO* bmem = nullptr;
		do {
			if (input.empty())
				break;
			b64 = BIO_new(BIO_f_base64());
			if (!b64)
				break;
			if (!multiline)
				BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
			bmem = BIO_new_mem_buf(input.data(), static_cast<int>(input.size()));
			if (!bmem)
				break;
			bmem = BIO_push(b64, bmem);
			if (!bmem)
				break;
			output.resize(input.size());
			long sslret = BIO_read(bmem, &output[0], static_cast<int>(output.size()));
			if (sslret <= 0) {
				output.clear();
				break;
			}
			output.resize(sslret);
			result = true;
		} while (0);
		if (b64) {
			BIO_free_all(b64);
			b64 = nullptr;
		}
		return result;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Openssl::Rsa::Encode(const EnType& type, const std::string& pwd, const std::string& keyPublic, const std::string& keyPrivate, const std::string& inData, std::string& outData) {
		outData.clear();
		bool result = false;
		if (inData.empty())
			return false;
		if (keyPublic.empty() && keyPrivate.empty())
			return false;
		BIO* bioKey = nullptr;
		RSA* rsaObj = nullptr;
		if (!keyPrivate.empty()) {//!@ Private Encode
			do {
				bioKey = BIO_new_mem_buf((std::uint8_t*)keyPrivate.c_str(), -1);
				rsaObj = RSA_new();
				rsaObj = PEM_read_bio_RSAPrivateKey(bioKey, &rsaObj, NULL, pwd.empty() ? nullptr : (void*)pwd.data());
				if (!rsaObj)
					break;
				const int once_encrypt_max = RSA_size(rsaObj);
				//!@ https://www.openssl.org/docs/man1.1.0/man3/RSA_public_encrypt.html -11 why??
				const int once_encrypt_handle = RSA_size(rsaObj) - 11;
				for (size_t pos = 0; pos < inData.size(); pos += once_encrypt_handle) {
					std::string sub_source = inData.substr(pos, once_encrypt_handle);
					if (sub_source.empty())
						break;
					std::string sub_result;
					sub_result.resize(once_encrypt_max);
					int result_len = RSA_private_encrypt(static_cast<int>(sub_source.size()), (const std::uint8_t*)sub_source.c_str(), (std::uint8_t*)&sub_result[0], rsaObj, RSA_PKCS1_PADDING);
					if (result_len <= 0)
						break;
					outData.append(std::string(sub_result.data(), result_len));
				}
				result = !outData.empty();
			} while (0);
		}
		else {//!@ Public Encode
			do {
				bioKey = BIO_new_mem_buf((std::uint8_t*)keyPublic.c_str(), -1);
				rsaObj = RSA_new();
				switch (type) {
				case EnType::EN_TYPE_PKCS8: {
					rsaObj = PEM_read_bio_RSA_PUBKEY(bioKey, &rsaObj, NULL, NULL);
				}break;
				default: {
					rsaObj = PEM_read_bio_RSAPublicKey(bioKey, &rsaObj, NULL, NULL);
				}break;
				}
				if (!rsaObj)
					break;
				const size_t once_encrypt_max = RSA_size(rsaObj);
				const size_t once_handle_size = once_encrypt_max - 11;
				for (size_t pos = 0; pos < inData.size(); pos += once_handle_size) {
					std::string sub_source = inData.substr(pos, once_handle_size);
					if (sub_source.empty())
						break;
					std::string sub_result;
					sub_result.resize(once_encrypt_max);
					auto size_result = RSA_public_encrypt(static_cast<int>(sub_source.size()), (const std::uint8_t*)sub_source.c_str(), (std::uint8_t*)&sub_result[0], rsaObj, RSA_PKCS1_PADDING);
					if (size_result <= 0)
						break;
					outData.append(std::string(sub_result.data(), size_result));
				}
				result = !outData.empty();
			} while (0);
		}
		if (rsaObj) {
			RSA_free(rsaObj);
			rsaObj = nullptr;
		}
		if (bioKey) {
			BIO_free_all(bioKey);
			bioKey = nullptr;
		}
		return result;
	}

	bool Openssl::Rsa::Decode(const EnType& type, const std::string& pwd, const std::string& keyPublic, const std::string& keyPrivate, const std::string& inData, std::string& outData) {
		outData.clear();
		bool result = false;
		if (inData.empty())
			return false;
		if (keyPrivate.empty() && keyPublic.empty())
			return false;
		outData.clear();
		BIO* bioKey = nullptr;
		RSA* rsaObj = nullptr;
		if (!keyPrivate.empty()) {
			do {
				rsaObj = RSA_new();
				bioKey = BIO_new_mem_buf((unsigned char*)keyPrivate.c_str(), -1);
				rsaObj = PEM_read_bio_RSAPrivateKey(bioKey, &rsaObj, NULL, NULL);
				if (!rsaObj)
					break;
				const size_t once_decrypt_max = RSA_size(rsaObj);
				for (size_t pos = 0; pos < inData.size(); pos += once_decrypt_max) {
					std::string sub_source = inData.substr(pos, once_decrypt_max);
					if (sub_source.empty())
						break;
					std::string sub_result;
					sub_result.resize(once_decrypt_max);
					int size_result = RSA_private_decrypt(static_cast<int>(sub_source.size()), (const std::uint8_t*)sub_source.c_str(), (std::uint8_t*)&sub_result[0], rsaObj, RSA_PKCS1_PADDING);
					if (size_result <= 0)
						break;
					outData.append(std::string(sub_result.data(), size_result));
				}
				result = !outData.empty();
			} while (0);
		}
		else {
			do {
				bioKey = BIO_new_mem_buf((unsigned char*)keyPublic.c_str(), -1);
				rsaObj = RSA_new();
				switch (type) {
				case EnType::EN_TYPE_PKCS8: {
					rsaObj = PEM_read_bio_RSA_PUBKEY(bioKey, &rsaObj, NULL, NULL);
				}break;
				default: {
					rsaObj = PEM_read_bio_RSAPublicKey(bioKey, &rsaObj, NULL, NULL);
				}break;
				}
				if (!rsaObj)
					break;
				const int once_decrypt_size = RSA_size(rsaObj);
				for (size_t pos = 0; pos < inData.size(); pos += once_decrypt_size) {
					std::string sub_source = inData.substr(pos, once_decrypt_size);
					if (sub_source.empty())
						break;
					std::string sub_result;
					sub_result.resize(once_decrypt_size);
					int decrypt_size = RSA_public_decrypt(static_cast<int>(sub_source.size()), (const std::uint8_t*)sub_source.c_str(), (std::uint8_t*)&sub_result[0], rsaObj, RSA_PKCS1_PADDING);
					if (decrypt_size <= 0)
						break;
					outData.append(std::string((char*)&sub_result[0], decrypt_size));
				}
				result = !outData.empty();
			} while (0);
		}
		if (bioKey) {
			BIO_free_all(bioKey);
			bioKey = nullptr;
		}
		if (rsaObj) {
			RSA_free(rsaObj);
			rsaObj = nullptr;
		}
		return result;
	}

	bool Openssl::Rsa::DERPublic(const EnType& type, const std::string& keyPublic, const std::string& pwd, std::string& outDER) {
		bool result = false;
		outDER.clear();
		RSA* rsaObj = nullptr;
		BIO* bioPublic = nullptr;
		do {
			if (keyPublic.empty())
				break;
			rsaObj = RSA_new();
			if (!rsaObj)
				break;
			bioPublic = BIO_new_mem_buf((unsigned char*)keyPublic.c_str(), -1);
			if (!bioPublic)
				break;
			switch (type) {
			case EnType::EN_TYPE_PKCS8: {
				rsaObj = PEM_read_bio_RSA_PUBKEY(bioPublic, &rsaObj, NULL, NULL);
			}break;
			default: {
				rsaObj = PEM_read_bio_RSAPublicKey(bioPublic, &rsaObj, NULL, NULL);
			}break;
			}
			if (!rsaObj)
				break;
#if 0
			if (1 != RSA_check_key(rsaObj))
				break;
#endif
			// Convert the RSA PublicKey to DER format to include in the header
			std::uint8_t* derkey = nullptr;
			/*int derlen = i2d_RSA_PUBKEY(rsaObj, 0);*/
			auto derlen = i2d_RSA_PUBKEY(rsaObj, &derkey);
			if (!derkey || derlen <= 0)
				break;
			outDER.append((char*)derkey, derlen);
			result = !outDER.empty();
		} while (0);
		if (rsaObj) {
			RSA_free(rsaObj);
			rsaObj = nullptr;
		}
		if (bioPublic) {
			BIO_free(bioPublic);
			bioPublic = nullptr;
		}
		return result;
	}
	bool Openssl::Rsa::PrivateToPublic(const EnType& type, const std::string& pwd, const std::string& inKeyPrivate, std::string& outKeyPublic) {
		bool result = false;
		outKeyPublic.clear();
		BIO* bioPrivate = nullptr;
		BIO* bioPublic = nullptr;
		RSA* rsaObj = nullptr;
		do {
			if (inKeyPrivate.empty())
				break;
			bioPrivate = BIO_new_mem_buf((std::uint8_t*)inKeyPrivate.c_str(), -1);
			rsaObj = RSA_new();
			rsaObj = PEM_read_bio_RSAPrivateKey(bioPrivate, &rsaObj, NULL, pwd.empty() ? nullptr : (void*)pwd.data());
			if (!rsaObj)
				break;
			bioPublic = BIO_new(BIO_s_mem());
			if (!bioPublic)
				break;
			int sslret = 0;
			switch (type) {
			case EnType::EN_TYPE_PKCS8: {
				sslret = PEM_write_bio_RSA_PUBKEY(bioPublic, rsaObj);
			}break;
			default: {
				sslret = PEM_write_bio_RSAPublicKey(bioPublic, rsaObj);
			}break;
			}
			if (sslret != 1)
				break;
			size_t public_key_length = BIO_pending(bioPublic);
			if (public_key_length <= 0)
				break;
			outKeyPublic.resize(public_key_length);
			public_key_length = BIO_read(bioPublic, &outKeyPublic[0], static_cast<int>(outKeyPublic.size()));
			if (public_key_length <= 0) {
				outKeyPublic.clear();
				break;
			}
			result = !outKeyPublic.empty();
		} while (0);
		if (rsaObj) {
			RSA_free(rsaObj);
			rsaObj = nullptr;
		}
		if (bioPrivate) {
			BIO_free(bioPrivate);
			bioPrivate = nullptr;
		}
		if (bioPublic) {
			BIO_free(bioPublic);
			bioPublic = nullptr;
		}
		return result;
	}
	bool Openssl::Rsa::Generate(const EnType& type, const std::string& pwd, std::string& outKeyPublic, std::string& outKeyPrivate) {
		bool result = false;
		outKeyPublic.clear();
		outKeyPrivate.clear();
		RSA* rsaObj = nullptr;
		BIO* bioPrivate = nullptr;
		BIO* bioPublic = nullptr;
		BIGNUM* e = nullptr;
		EVP_PKEY* pEVPKey = nullptr;
		switch (type) {
		case EnType::EN_TYPE_PKCS8: {
			do {
#if 0
				rsaObj = RSA_generate_key(2048, RSA_F4, NULL, NULL);
				if (!rsaObj)
					break;
#else
				e = BN_new();
				if (!e)
					break;
				if (1 != BN_set_word(e, RSA_F4))
					break;
				rsaObj = RSA_new();
				if (!rsaObj)
					break;
				if (1 != RSA_generate_key_ex(rsaObj, 2048, e, 0))
					break;
				if (!rsaObj)
					break;
#endif
				pEVPKey = EVP_PKEY_new();
				if (!pEVPKey)
					break;
				int sslret = EVP_PKEY_assign_RSA(pEVPKey, rsaObj);
				if (1 != sslret)
					break;
				bioPrivate = BIO_new(BIO_s_mem());
				if (!bioPrivate)
					break;
				sslret = PEM_write_bio_PKCS8PrivateKey(bioPrivate, pEVPKey, nullptr/*EVP_des_ede3_cbc()*/, NULL, 0, NULL, pwd.empty() ? nullptr : (void*)pwd.data());
				if (1 != sslret)
					break;
				bioPublic = BIO_new(BIO_s_mem());
				if (!bioPublic)
					break;
				sslret = PEM_write_bio_RSA_PUBKEY(bioPublic, rsaObj);
				if (1 != sslret)
					break;
				size_t private_key_length = BIO_pending(bioPrivate);
				size_t public_key_length = BIO_pending(bioPublic);
				if (private_key_length <= 0 || public_key_length <= 0)
					break;
				outKeyPublic.resize(public_key_length);
				outKeyPrivate.resize(private_key_length);
				private_key_length = BIO_read(bioPrivate, &outKeyPrivate[0], static_cast<int>(outKeyPrivate.size()));
				if (private_key_length <= 0) {
					outKeyPublic.clear();
					outKeyPrivate.clear();
					break;
				}
				public_key_length = BIO_read(bioPublic, &outKeyPublic[0], static_cast<int>(outKeyPublic.size()));
				if (public_key_length <= 0) {
					outKeyPublic.clear();
					outKeyPrivate.clear();
					break;
				}
				result = !outKeyPublic.empty() && !outKeyPrivate.empty();
		} while (0);
	}break;
		default: {
			do {
#if 0
				rsaObj = RSA_generate_key(2048, RSA_F4, NULL, NULL);
				if (!rsaObj)
					break;
#else
				e = BN_new();
				if (!e)
					break;
				if (1 != BN_set_word(e, RSA_F4))
					break;
				rsaObj = RSA_new();
				if (!rsaObj)
					break;
				if (1 != RSA_generate_key_ex(rsaObj, 2048, e, 0))
					break;
				if (!rsaObj)
					break;
#endif
				bioPrivate = BIO_new(BIO_s_mem());
				if (!bioPrivate)
					break;
				bioPublic = BIO_new(BIO_s_mem());
				if (!bioPublic)
					break;
				int sslret = PEM_write_bio_RSAPrivateKey(bioPrivate, rsaObj, NULL, NULL, 0, NULL, pwd.empty() ? nullptr : (void*)pwd.data());
				if (sslret != 1)
					break;
				sslret = PEM_write_bio_RSAPublicKey(bioPublic, rsaObj);
				if (sslret != 1)
					break;
				size_t private_key_length = BIO_pending(bioPrivate);
				size_t public_key_length = BIO_pending(bioPublic);
				if (private_key_length <= 0 || public_key_length <= 0)
					break;
				outKeyPublic.resize(public_key_length);
				outKeyPrivate.resize(private_key_length);
				private_key_length = BIO_read(bioPrivate, &outKeyPrivate[0], static_cast<int>(outKeyPrivate.size()));
				if (private_key_length <= 0) {
					outKeyPublic.clear();
					outKeyPrivate.clear();
					break;
				}
				public_key_length = BIO_read(bioPublic, &outKeyPublic[0], static_cast<int>(outKeyPublic.size()));
				if (public_key_length <= 0) {
					outKeyPublic.clear();
					outKeyPrivate.clear();
					break;
				}
				result = !outKeyPublic.empty() && !outKeyPrivate.empty();
			} while (0);
		}break;
		}
#if 0
		if (pEVPKey) {
			EVP_PKEY_free(pEVPKey);
			pEVPKey = nullptr;
		}
#endif
		if (rsaObj) {
			RSA_free(rsaObj);
			rsaObj = nullptr;
		}
		if (bioPublic) {
			BIO_free_all(bioPublic);
			bioPublic = nullptr;
		}
		if (bioPrivate) {
			BIO_free_all(bioPrivate);
			bioPrivate = nullptr;
		}
		if (e) {
			BN_free(e);
			e = nullptr;
		}
		return result;
	}

}///namespace shared