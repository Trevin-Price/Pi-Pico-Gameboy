cmake_minimum_required(VERSION 3.24.2)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# define project name and languages
project("Gameboy" C CXX ASM)

#define versions
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# define a few compile options that came with the stock example
add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
    -Wno-psabi # disable std::vector warning with parameter passing having changed
)

# add my project to be assembled
add_executable(gameboy
    Source/main.cpp
    Source/DisplayDriver.cpp
    Source/DisplayTests.cpp
    Source/MainMenu.cpp
    Source/SnakeGame.cpp
    Source/UserInputHandler.cpp
    Source/Camera.cpp

    Source/Matrix.cpp
    Source/Vector.cpp
)

# include my headers
include_directories(${PROJECT_SOURCE_DIR}/Headers)

# pull in common dependencies
target_link_libraries(gameboy pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(gameboy)