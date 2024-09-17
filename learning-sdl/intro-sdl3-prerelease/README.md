
This project demonstrates how to link a C/C++ application to the SDL3 pre-release in a CMake project.

To achieve this, the developer must Navigate to the /external/ folder, and run `git clone https://github.com/libsdl-org/SDL.git` to clone the SDL repository into the `SDL` folder.
Take note that `external/` has been added to `.gitignore` in this example.
I'm not sure how Git handles a nested repository (something I should probably look into)...

Once SDL3 is available in a release tag, you should follow the FetchContent approach in the intro-sdl2-release sample project.
The FetchContent module requires you to clone a tag, to ensure dependencies are treated as a read-only library.
