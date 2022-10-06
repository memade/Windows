#include <win.h>
#include <encryption.hpp>
#include <res\help.txt.res.h>

int main(int argc, char** argv) {






	std::thread __main(
		[&]() {
			do {
				std::string _input;
				char c;
				while (std::cin >> std::noskipws >> c) { if ('\n' == c) break; _input.push_back(c); }
				std::cin >> std::skipws;
				shared::IConv::ToLowerA(_input);
				std::vector<std::string> cmds;
				shared::Win::File::ParseA(_input, ' ', cmds);
				if (cmds.empty())
					continue;
				if (cmds[0] == "q" || cmds[0] == "quit" || cmds[0] == "exit") {
					break;
				}
				else if (cmds[0] == "10001" || cmds[0] == "/10001") {

				}
				else if (cmds[0] == "10002" || cmds[0] == "/10002") {
				}
				else if (cmds[0] == "10003" || cmds[0] == "/10003") {
			
				}
				else {
					continue;
				}
			} while (1);
		});
	__main.join();

	return 0;
}
