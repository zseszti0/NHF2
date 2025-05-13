cmake_minimum_required(VERSION 3.29)
project(NHF2)

set(CMAKE_CXX_STANDARD 20)

# SDL2 Path (Change this if you extracted SDL somewhere else)
set(SDL2_DIR "C:/SDL2-2.32.2/x86_64-w64-mingw32")
set(SDL2_IM_DIR "C:/SDL2_image-2.8.8/x86_64-w64-mingw32")
set(SDL2_TTF_DIR "C:/SDL2_ttf-2.24.0/x86_64-w64-mingw32")
# Include SDL2 headers
include_directories(${SDL2_DIR}/include)
include_directories(${SDL2_IM_DIR}/include)
include_directories(${SDL2_TTF_DIR}/include)


# Link SDL2 libraries
link_directories(${SDL2_DIR}/lib)
link_directories(${SDL2_IM_DIR}/lib)
link_directories(${SDL2_TTF_DIR}/lib)

# Add Executable
add_executable(NHF2 main.cpp
        UIAndDisplay.h
        UIAndDisplay.cpp
        combat.cpp
        fight.cpp
        animation.cpp
        scene.cpp
        adventure.cpp
        filesAndSaving.cpp
        gameEngine.cpp
)


# Link SDL2 properly
target_link_libraries(NHF2 mingw32 SDL2main SDL2 SDL2_image SDL2_ttf)