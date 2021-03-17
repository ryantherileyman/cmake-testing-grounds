
#include <iostream>
#include <fstream>
#include <string>

int main() {
	
	std::ifstream greetingFile("resources/greeting.txt");
	if (greetingFile.is_open()) {
		std::string greetingText;
		getline(greetingFile, greetingText);
		std::cout << greetingText;
	}
	else {
		std::cout << "Error: " << greetingFile.rdstate();
	}

	return 0;
}
