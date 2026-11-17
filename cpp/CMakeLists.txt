cmake_minimum_required(VERSION 3.15)
project(CursedClash VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include /opt/homebrew/include /usr/local/include)
link_directories(/opt/homebrew/lib /usr/local/lib)

file(GLOB SOURCES "src/*.cpp")

add_executable(cursed_clash ${SOURCES})

if (APPLE)
    target_link_libraries(cursed_clash PRIVATE
        raylib
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(cursed_clash PRIVATE raylib GL m pthread dl rt X11)
endif()
