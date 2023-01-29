#include <http_parser.hpp>
#include <win.h>

int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif

 std::string strVersion;
 unsigned long nVersion = 0;
 httpparser::Method::version(strVersion, nVersion);

 const char* corrupted_connection =
  "GET / HTTP/1.1\r\n"
  "Host: www.example.com\r\n"
  "Connection: Keep-Alive\r\n"
  "Accept-Encoding: gzip\r\n"
  "\r\n";

 size_t parser_size = 0;
 httpparser::HttpRequest parse_map;
 httpparser::Method::parse_http_request(corrupted_connection, parser_size, parse_map);

 const char* test_parse_url = R"(https://github.com/Fndroid/clash_for_windows_pkg/issues/3172)";

 httpparser::HttpUrlFields url_fields;
 httpparser::Method::parse_http_url(test_parse_url, url_fields);

 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {

   if (input == "q") {
    exit = true;
   }
   else if (input == "test") {
    auto  sk = 0;
   }
  });

 return 0;
}
