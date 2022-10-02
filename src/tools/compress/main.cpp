#include <win.h>
#include <zip++.h>

class ZipTaskNode final {
public:
	WIN32_FILE_ATTRIBUTE_DATA m_TargetFileAttributeData = { 0 };
	_finddata_t m_TargetInfo = { 0 };
	std::string m_TargetFullPathname;
	std::string m_TargetIdentify;
	std::string m_TargetPathname;
};

int main(int argc, char** argv) {
#if 1
	if (argc < 3) {
		std::cout << "Params invalid." << std::endl;
		return -1;
	}

	const std::string MadeObjectDir = argv[1];
	const std::string GenerateTargetFilePathname = argv[2];

	if (MadeObjectDir.empty() || GenerateTargetFilePathname.empty()) {
		std::cout << "Pathname invalid." << std::endl;
		return -1;
	}
#else
	const std::string MadeObjectDir = R"(E:\www.skstu.com\Project.KS\Malware.DownTool\res\KitTip.dll)";
	const std::string GenerateTargetFilePathname = R"(E:\www.skstu.com\Project.KS\Malware.DownTool\res\KitTip.dll.res)";
#endif

	if (::_access(MadeObjectDir.c_str(), 0) != 0) {
		std::cout << "Target file not found." << std::endl;
		return -1;
	}

	std::vector<ZipTaskNode> update_target_s;

	WIN32_FIND_DATAA win32FindData = { 0 };
	HANDLE findFile = ::FindFirstFileA(MadeObjectDir.c_str(), &win32FindData);

	/*shared::Windows::CreateDirectoryA(GenerateTargetFilePathname);*/
	if (win32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		std::map<std::string, std::string> dirs, files;
		shared::Win::EnumFoldersAndFiles(MadeObjectDir, dirs, files, "*.*", true,
			[&](const auto& target_path, const auto& target_identify, const _finddata_t& target_file_data) {
				if (!(target_file_data.attrib & _A_SUBDIR))
				{
					ZipTaskNode target;
					target.m_TargetPathname = target_path;
					target.m_TargetFullPathname = MadeObjectDir + target_path;
					target.m_TargetIdentify = shared::Win::EnumCorrectPath(target_path, '_', false);
					memcpy(&target.m_TargetInfo, &target_file_data, sizeof(target.m_TargetInfo));
					update_target_s.emplace_back(target);
				}
			});

		if (!update_target_s.empty()) {
			::remove(GenerateTargetFilePathname.c_str());
			libzippp::ZipArchive zipWrite(GenerateTargetFilePathname);
			if (zipWrite.open(libzippp::ZipArchive::Write)) {
				for (auto& target : update_target_s) {
					do {
						auto ZipPath = shared::Win::EnumCorrectPath(shared::Win::GetPathByPathnameA(target.m_TargetPathname), '/', true);
						if (ZipPath.empty())
							break;
						ZipPath.erase(ZipPath.begin());
						std::string entryName = ZipPath + target.m_TargetInfo.name;
						if (!zipWrite.addFile(entryName, target.m_TargetFullPathname))
							break;
						if (!zipWrite.addEntry(ZipPath))
							break;
					} while (0);
				}
				zipWrite.close();
			}
		}

	}
	else {
		::remove(GenerateTargetFilePathname.c_str());
		libzippp::ZipArchive zipWrite(GenerateTargetFilePathname);
		if (zipWrite.open(libzippp::ZipArchive::Write)) {
			do {
				std::string entryName = shared::Win::GetNameByPathnameA(MadeObjectDir);
				if (!zipWrite.addFile(entryName, MadeObjectDir))
					break;
			} while (0);
			zipWrite.close();
		}
	}

	std::cout << "Compress " << std::endl;
	std::cout << " From " << MadeObjectDir << std::endl;
	std::cout << " To " << GenerateTargetFilePathname << std::endl;
	std::cout << "Process success." << std::endl;
	return 0;
}
