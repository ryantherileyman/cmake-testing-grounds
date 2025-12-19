@ECHO OFF
IF DEFINED EMSDK (
	ECHO Configuring Emscripten Release build in "out/build/web-release"...
	emcmake cmake -B out/build/web-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release
) ELSE (
	ECHO Emscripten environment has not been set up
)
