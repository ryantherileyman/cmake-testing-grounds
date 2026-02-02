
cmake_minimum_required(VERSION 3.23)

project(
	learning_cmake_presets
	LANGUAGES CXX
)

add_executable(
	learning_cmake_presets
	src/main.cpp
)
target_compile_features(
	learning_cmake_presets PRIVATE
	cxx_std_17
)
