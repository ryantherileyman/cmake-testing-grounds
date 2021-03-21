
cmake_minimum_required(VERSION 3.19)

project(LearnCMakePart1 VERSION 1.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

string(TIMESTAMP BUILD_DATE "%Y%m%d")
string(TIMESTAMP BUILD_TIME "%H%M%S")

configure_file(configure/about.txt resources/about.txt)

add_executable(
	LearnCMakePart1
	main.cpp
)

add_custom_command(
	TARGET LearnCMakePart1 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:LearnCMakePart1>/resources
)

enable_testing()

add_test(NAME GreetingTest COMMAND LearnCMakePart1)
set_tests_properties(
	GreetingTest
	PROPERTIES PASS_REGULAR_EXPRESSION "Hello, CMake, from our resource file!"
)

add_test(NAME VersionTest COMMAND LearnCMakePart1 -v)
set_tests_properties(
	VersionTest
	PROPERTIES PASS_REGULAR_EXPRESSION "Version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)
