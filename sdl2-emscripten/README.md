## CMake, SDL2, and Emscripten Example Project

This project illustrates how to wire together a C++ game project that uses **CMake**, **SDL2**, and **Emscripten**.  The game can target either a desktop build, or a web-based (WebAssembly) build that runs in a browser.  It uses the **SDL2_image** library to load PNG texture files, the **SDL2_ttf** library to load a TTF font, and the **SDL2_mixer** library to load and play music and sound effects.

For the desktop build, SDL2 is brought in via CMake's FetchContent module.  For the Emscripten build, SDL2 is brought in via the Emscripten port.

The game itself is a simplified version of Whack-a-Mole.  I bet you didn't think Whack-a-Mole could be simplified.  But you were wrong.  😆  Will you hit the gopher to make him sad?  Or leave him alone to accumulate misses while you listen to Kevin MacLeod's catchy 8-bit tune?

### Build Instructions (Windows)

Provides a `CMakePresets.json` file, accepted by most integrated development environments.  If you have both Visual Studio Community and VS Code installed, you should be able to open the folder in VS Code and choose the appropriate configuration preset.

For command-line building, I've included batch files for Windows users to build separate targets for:
- Desktop Release (`configure-desktop-release.bat` and `build-desktop-release.bat`)
- Emscripten (WebAssembly) Debug (`configure-emscripten-debug.bat` and `build-emscripten-debug.bat`)
- Emscripten (WebAssembly) Release (`configure-emscripten-release.bat` and `build-emscripten-release.bat`)

### Attribution

Cartoon Gopher: Stable Diffusion (AI-Generated)  
Font: Open Sans (from fontsquirrel.com)  
Icons: Google Materials Icons (https://fonts.google.com/icons)  
Sound Effects: ZapSplat (from zapsplat.com)  

Music:  
"Bit Shift" Kevin MacLeod (incompetech.com)  
Licensed under Creative Commons: By Attribution 4.0 License  
http://creativecommons.org/licenses/by/4.0/  
