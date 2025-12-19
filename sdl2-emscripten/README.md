## CMake, SDL2, and Emscripten Example Project

This project illustrates how to wire together a C++ game project that uses **CMake**, **SDL2**, and **Emscripten**.  The game can target either a desktop build, or a web-based (WebAssembly) build that runs in a browser.  It uses the **SDL2_image** library to load PNG texture files, the **SDL2_ttf** library to load a TTF font, and the **SDL2_mixer** library to load and play music and sound effects.

For the desktop build, SDL2 is brought in via CMake's FetchContent module.  For the Emscripten build, SDL2 is brought in via the Emscripten port.

The game itself is a simplified version of Whack-a-Mole.  I bet you didn't think Whack-a-Mole could be simplified.  But you were wrong.  😆  Will you hit the gopher to make him sad?  Or leave him alone to accumulate misses while you listen to Kevin MacLeod's catchy 8-bit tune?

### Desktop Command-Line Build Instructions (Windows)

The following will output the desktop build to the `out/cl-build` folder:
```
cmake -B out/cl-build -S . -A x64 -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug
cmake --build out/cl-build --config debug
```

### Emscripten Command-Line Build Instructions

The following will output the WebAssembly build to the `out/web-build` folder:
```
emcmake cmake -B out/web-build -G "Ninja"
cmake --build out/web-build
```

### Attribution

Cartoon Gopher: Stable Diffusion (AI-Generated)  
Font: Open Sans (from fontsquirrel.com)  
Sound Effects: ZapSplat (from zapsplat.com)  

Music:  
"Bit Shift" Kevin MacLeod (incompetech.com)  
Licensed under Creative Commons: By Attribution 4.0 License  
http://creativecommons.org/licenses/by/4.0/  
