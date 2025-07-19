cmake_minimum_required(VERSION 3.10)
project(SokolTest LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_compile_options("$<$<CONFIG:Debug>:-fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.>")


include_directories(${CMAKE_SOURCE_DIR}/include)

## Find all files in the src directory
# file(GLOB SOURCES "src/*.mm" "src/*.cpp" "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.mm" "src/*.cpp" "src/*.h")

set_source_files_properties(src/main.mm PROPERTIES LANGUAGE OBJCXX)
add_executable(SokolTest ${SOURCES})

target_link_libraries(SokolTest
    "-framework Cocoa"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework MetalKit"
    "-framework CoreGraphics"
    "-framework CoreFoundation"
)
