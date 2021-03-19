
cmake_minimum_required(VERSION 3.19)

project(LearnCMakePart1 VERSION 1.1)

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
