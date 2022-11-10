#include <win.h>
#include <encryption.hpp>
#include <res\help.txt.res.h>

int main(int argc, char** argv) {



	shared::Win::MainProcess(
		[&](const std::string& input,bool& exit) {	
			if (input == "q") {
				exit = true;
			}

		},true);


	return 0;
}
