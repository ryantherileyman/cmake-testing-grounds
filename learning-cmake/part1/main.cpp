
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
	
	bool printGreetingFlag = true;
	if (argc > 1) {
		if (strcmp(argv[1], "-v") == 0) {
			std::string versionString;
			std::string buildString;

			std::ifstream aboutFile("resources/about.txt");
			std::string aboutLine;
			while (std::getline(aboutFile, aboutLine)) {
				if (aboutLine.find("version=") == 0) {
					versionString = aboutLine.substr(8);
				}
				else if (aboutLine.find("build=") == 0) {
					buildString = aboutLine.substr(6);
				}
			}

			std::cout << "Version: " << versionString << "\n";
			std::cout << "Build Date: " << buildString << "\n";

			printGreetingFlag = false;
		}
	}

	if (printGreetingFlag) {
		std::ifstream greetingFile("resources/greeting.txt");
		if (greetingFile.is_open()) {
			std::string greetingText;
			getline(greetingFile, greetingText);
			std::cout << greetingText;
		}
		else {
			std::cout << "Error: " << greetingFile.rdstate();
		}
	}

	return 0;
}
