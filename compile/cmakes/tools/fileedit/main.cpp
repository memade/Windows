#include <win.h>

const char key_cmd_input[] = R"(--input)";
const char key_cmd_output[] = R"(--output)";

#if 0
#include "stdafx.h"
namespace protocol {
	const char SYSTEM_INTERNAL_VERSION[] = "{}";
}
#endif
//!@ 本项目当前专用与编辑/更新 项目中 'version.cpp' 文件
int main(int argc, char** argv) {
	std::map<std::string, std::string> input_parameters;
	if (!shared::Win::ParseCommandLineParameters(argc, argv, input_parameters)) {
		std::cout << "Invalid command line parameters." << std::endl;
		return -1;
	}
	do {
		if (input_parameters.size() != 2)
			break;
		std::string temp = shared::Win::File::Read(input_parameters[key_cmd_input]);
		if (temp.find("[$version]") == std::string::npos)
			break;
		SYSTEMTIME systime;
		::GetLocalTime(&systime);
		auto version = std::format("{}.{}.{}{:02}.{}{:02}",
			1,
			systime.wYear % 100,
			systime.wMonth,
			systime.wDay,
			systime.wHour,
			systime.wMinute
		);
		temp.replace(temp.find("[$version]"), strlen("[$version]"), version);
		shared::Win::File::Write(input_parameters[key_cmd_output], temp);
	} while (0);
	return 0;
}