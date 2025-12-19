## CMake, SDL2, and Emscripten Example Project

This project illustrates how to write together a C++ game project that uses CMake, SDL2, and Emscripten.  The game can target either a desktop build, or a web-based (WebAssembly) build that runs in a browser.  It uses the **SDL2_image** library to load PNG texture files, the **SDL2_ttf** library to load a TTF font, and the **SDL2_mixer** library to load and play music and sound effects.

For the desktop build, SDL2 is brought in via CMake's FetchContent module.  For the Emscripten build, SDL2 is brought in via the Emscripten port.

Attribution:

Cartoon Gopher: Stable Diffusion (AI-Generated)
Font: Open Sans (from fontsquirrel.com)
Sound Effects: ZapSplat (from zapsplat.com)

Music:
"Bit Shift" Kevin MacLeod (incompetech.com)
Licensed under Creative Commons: By Attribution 4.0 License
http://creativecommons.org/licenses/by/4.0/
