@ECHO OFF
IF DEFINED EMSDK (
	ECHO Building Emscripten Release target to "out/build/web-release"...
	cmake --build out/build/web-release --config Release
) ELSE (
	ECHO Emscripten environment has not been set up
)
