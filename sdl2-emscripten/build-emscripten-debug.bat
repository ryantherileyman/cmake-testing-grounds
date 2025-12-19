@ECHO OFF
IF DEFINED EMSDK (
	ECHO Building Emscripten Debug target to "out/build/web-debug"...
	cmake --build out/build/web-debug
) ELSE (
	ECHO Emscripten environment has not been set up
)
