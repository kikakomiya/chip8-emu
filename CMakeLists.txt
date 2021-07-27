cmake_minimum_required(VERSION 3.21)

project(chip8-emu)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (UNIX)
    add_executable(chip8-emu src/main.c)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(sdl2 IMPORTED_TARGET REQUIRED sdl2)
    target_link_libraries(chip8-emu PkgConfig::sdl2)
    add_custom_command(TARGET chip8-emu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:chip8-emu> bin/chip8-emu
        COMMAND ${CMAKE_COMMAND} -E rm chip8-emu
    )
endif()
if (WIN32)
    add_executable(chip8-emu_win src/main.c)
    add_custom_command(TARGET chip8-emu_win POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:chip8-emu_win> bin/chip8-emu_win.exe
        COMMAND ${CMAKE_COMMAND} -E copy chip8-emu_win.pdb bin/chip8-emu_win.pdb
        COMMAND ${CMAKE_COMMAND} -E rm chip8-emu_win.exe
        COMMAND ${CMAKE_COMMAND} -E rm chip8-emu_win.pdb
    )
endif()