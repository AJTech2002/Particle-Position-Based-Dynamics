cmake_minimum_required(VERSION 3.10)
project(SokolTest LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(${CMAKE_SOURCE_DIR}/include)

## Find all files in the src directory
file(GLOB SOURCES "src/*.mm" "src/*.cpp" "src/*.h")
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