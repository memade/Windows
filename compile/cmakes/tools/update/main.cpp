#include <win.h>
#include <rapidjson.h>
#include <zip++.h>
#include <openssl.hpp>
#include <encryption.hpp>

const char key_cmd_input_dir[] = R"(--input-update-dir)";
const char key_cmd_output_config[] = R"(--output-update-config)";

class UpdateObj final : public shared::FileUpdateObj {
public:
	UpdateObj(const std::string& identify, const std::string& basePath) {
		do {
			Identify = identify;
			FilePathname = shared::Win::PathFixedA(basePath + "\\" + Identify);
			if (!shared::Win::AccessA(FilePathname))
				break;
			FileBuffer = shared::Win::File::Read(FilePathname);
			if (FileBuffer.empty())
				break;
			DiskFileSize = static_cast<DWORD>(FileBuffer.size());
			if (DiskFileSize <= 0)
				break;
			if (!shared::Openssl::MD5(FileBuffer, MD5) || MD5.empty())
				break;
			shared::Win::File::Attribute::GetFileVersion(FilePathname, Version);
		} while (0);
	}
	bool operator>>(std::string& jsonObjstr) const {
		bool result = false;
		jsonObjstr.clear();
		do {
			rapidjson::Document doc{ rapidjson::Type::kObjectType };
			if (Identify.empty())
				break;
			doc.AddMember(rapidjson::Value().SetString("Identify", doc.GetAllocator()).Move(),
				rapidjson::Value().SetString(Identify.c_str(), doc.GetAllocator()).Move(),
				doc.GetAllocator()
			);
			doc.AddMember(rapidjson::Value().SetString("DiskFileSize", doc.GetAllocator()).Move(),
				rapidjson::Value().SetUint(DiskFileSize).Move(),
				doc.GetAllocator()
			);
			doc.AddMember(rapidjson::Value().SetString("MD5", doc.GetAllocator()).Move(),
				rapidjson::Value().SetString(MD5.c_str(), doc.GetAllocator()).Move(),
				doc.GetAllocator()
			);
			doc.AddMember(rapidjson::Value().SetString("Version", doc.GetAllocator()).Move(),
				rapidjson::Value().SetString(Version.c_str(), doc.GetAllocator()).Move(),
				doc.GetAllocator()
			);
			jsonObjstr = shared::Json::toString(doc);
			result = true;
		} while (0);
		return result;
	}
};

int main(int argc, char** argv) {
	std::map<std::string, std::string> input_parameters;
	if (!shared::Win::ParseCommandLineParameters(argc, argv, input_parameters)) {
		std::cout << "Invalid command line parameters." << std::endl;
		return -1;
	}

	std::set<UpdateObj> updateObjs;
	std::map<std::string, std::string> update_dir_folders, update_dir_files;
	shared::Win::EnumFoldersAndFiles(input_parameters[key_cmd_input_dir], update_dir_folders, update_dir_files, "*.*", true);
	for (const auto& file : update_dir_files) {
		UpdateObj obj(file.first, input_parameters[key_cmd_input_dir]);
		if (!obj.Verify())
			continue;
		updateObjs.emplace(obj);
	}
	std::string update_config;
	for (const auto& obj : updateObjs) {
		std::string node_config;
		if (!(obj >> node_config))
			continue;
		update_config.append(node_config).append(",");
	}
	if (!update_config.empty()) {
		update_config.pop_back();
		update_config.insert(0, "[");
		update_config.push_back(']');
	}
	std::string output_config = input_parameters[key_cmd_output_config];
	if (output_config.empty())
		output_config = shared::Win::PathFixedA(shared::Win::GetSpecialFolderLocationA(CSIDL_DESKTOPDIRECTORY) + "\\update_config.json");

	shared::Win::File::Write(output_config, update_config);

	std::cout << std::format("Update config output to file({}).", output_config);

	return 0;
}