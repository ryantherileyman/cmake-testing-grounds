
set(SAMPLE_SDL2_MOBILE_WEB_SOURCES
    main.cpp
)

add_executable(
    Sdl2MobileWeb
    ${SAMPLE_SDL2_MOBILE_WEB_SOURCES}
)

target_compile_features(
    Sdl2MobileWeb PRIVATE
    cxx_std_17
)

if(NOT EMSCRIPTEN)
    message(STATUS "Configuring native desktop build")

    if(WIN32)
        set_target_properties(
            Sdl2MobileWeb PROPERTIES
            WIN32_EXECUTABLE $<$<CONFIG:Release>:ON>
        )
    endif()

    if(TARGET SDL2::SDL2main)
        target_link_libraries(
            Sdl2MobileWeb PRIVATE
            SDL2::SDL2main
        )
    endif()
    target_link_libraries(
        Sdl2MobileWeb PRIVATE
        SDL2::SDL2-static
        SDL2_ttf::SDL2_ttf-static
    )

    add_custom_target(
        CopySdl2MobileWebResources ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf
            "$<TARGET_FILE_DIR:Sdl2MobileWeb>/resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/src/resources"
            "$<TARGET_FILE_DIR:Sdl2MobileWeb>/resources"
    )
    add_dependencies(Sdl2MobileWeb CopySdl2MobileWebResources)
endif()

if(EMSCRIPTEN)
    message(STATUS "Configuring Emscripten WebAssembly build")

    target_compile_options(
        Sdl2MobileWeb PRIVATE
        "-sUSE_SDL=2"
        "-sUSE_SDL_TTF=2"
    )
    target_link_options(
        Sdl2MobileWeb PRIVATE
        "-sUSE_SDL=2"
        "-sUSE_SDL_TTF=2"
        "--preload-file" "${CMAKE_SOURCE_DIR}/src/resources@resources"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sASSERTIONS=1"
        "-sFORCE_FILESYSTEM=1"
    )

    add_custom_target(
        CopySdl2MobileWebIndexHtml ALL
        COMMENT "Copying index.html to build folder"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/src/emscripten"
            "$<TARGET_FILE_DIR:Sdl2MobileWeb>"
    )
    add_dependencies(Sdl2MobileWeb CopySdl2MobileWebIndexHtml)
endif()
