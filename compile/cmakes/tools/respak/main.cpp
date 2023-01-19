#include <win.h>
#include <zip++.h>

//!@ 将任意文件转换为"C"数组
//! --input [输入目标文件路径]
//! --outfilename [输出文件名字]
//! --output [输入文件路径]
//! --variable [数组变量名字]
//! --zip=1 [是否压缩 0不压缩 1压缩]

static bool ParseParameters(const int& argc, char**, tfCommandLineNode& parameters);

int main(int argc, char** argv) {
 tfCommandLineNode param_map;
 if (!ParseParameters(argc, argv, param_map)) {
  std::cout << std::format("Input parameters parse failed.") << std::endl;
  return -1;
 }

 bool zip = ::atoi(param_map["--zip"].c_str()) != 0;
 zip = true;//!@ 始终需要ZIP处理

 do {
  std::string buffer = shared::Win::File::Read(param_map["--input"]);
  if (buffer.empty()) {
   std::cout << std::format("Input file({}) buffer is empty.", param_map["--input"]) << std::endl;
   break;
  }

  if (zip) {
   std::string zipBuffer;
   unsigned long buffer_origin_size = static_cast<unsigned long>(buffer.size());
   if (Z_OK != shared::Zip::zipCompress(buffer, zipBuffer)) {
    std::cout << std::format("Input file zip compress failed.") << std::endl;
    break;
   }
   buffer = zipBuffer;
   buffer.append((char*)&buffer_origin_size, sizeof(buffer_origin_size));
  }

  std::string final_buffer = shared::Win::GenerateHexBuffer(buffer, param_map["--variable"], ",");
  if (final_buffer.empty()) {
   std::cout << std::format("Generate hex buffer failed.") << std::endl;
   break;
  }

  std::string final_filename = shared::Win::PathFixedA(param_map["--output"] + "\\" + param_map["--outfilename"]);
  if (!shared::Win::File::Write(final_filename, final_buffer)) {
   std::cout << std::format("Write to file failed.") << std::endl;
   break;
  }

  std::cout << std::format("Final success on({}) total({}).", final_filename, final_buffer.size()) << std::endl;
 } while (0);


 return 0;
}

bool ParseParameters(const int& argc, char** argv, tfCommandLineNode& parameters) {
 bool result = false;
 do {
  if (!shared::Win::ParseCommandLineParameters(argc, argv, parameters))
   break;
  if (parameters["--input"].empty() || parameters["--outfilename"].empty() || parameters["--output"].empty() || parameters["--variable"].empty()) {
   for (const auto& node : parameters)
    std::cout << node.first << "=" << node.second << std::endl;
   std::cout << "Required parameters are missing." << std::endl;
   break;
  }
  if (!shared::Win::AccessA(parameters["--input"])) {
   std::cout << "--input=" << parameters["--input"] << std::endl;
   std::cout << "Input file nothingness." << std::endl;
   break;
  }

  if (!shared::Win::AccessA(parameters["--output"])) {
   auto create_directory_result = shared::Win::CreateDirectoryA(parameters["--output"]);
   std::cout << std::format("Create dir({}) {}.", parameters["--output"], create_directory_result ? "success" : "failed") << std::endl;
   if (!create_directory_result)
    break;
  }

  char check_variable = parameters["--variable"].at(0);
  if (!((check_variable >= 'a' && check_variable <= 'z') || (check_variable >= 'A' && check_variable <= 'Z'))) {
   std::cout << std::format("Variable '{}' not in accordance with the rules.", parameters["--variable"]) << std::endl;
   break;
  }


  result = true;
 } while (0);
 return result;
}