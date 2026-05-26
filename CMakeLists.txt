cmake_minimum_required(VERSION 3.10.0)
project(RaceGame VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(RaceGame 
    src/main.cpp 
    src/Game/Game.cpp 
    src/Game/Assets/AssetLoad.cpp 
    src/Game/Car/Car.cpp
    src/Game/Road/Road.cpp
    src/Game/UI/UI.cpp

)

target_include_directories(RaceGame PRIVATE ${X11_INCLUDE_DIR} Include src)

target_link_libraries(RaceGame PRIVATE ${X11_LIBRARIES})

target_compile_options(RaceGame PRIVATE 
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O3>
)