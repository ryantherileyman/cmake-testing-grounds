@ECHO OFF
IF DEFINED EMSDK (
	ECHO Configuring Emscripten Debug build in "out/build/web-debug"...
	emcmake cmake -B out/build/web-debug -G "Ninja"
) ELSE (
	ECHO Emscripten environment has not been set up
)
