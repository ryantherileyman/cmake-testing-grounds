## SDL2 + Emscripten Fullscreen and Orientation Tests

These projects illustrate how the SDL2 Emscripten port deals with window resizing, fullscreen toggles, and orientation changes in a browser environment.  It's particularly useful for testing SDL2 and Emscripten in a mobile browser environment.

The `no-dpr` project allows the SDL2 Emscripten port to handle window resize events automatically, but does not attempt to perform device pixel ratio scaling.

The `with-dpr` project takes full control of window resize events, and handles device pixel ratio scaling.

### Attribution

Font: Open Sans (from fontsquirrel.com)  
