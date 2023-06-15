cmake_minimum_required(VERSION 3.25)
project(ImaEasyCrypt)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

add_executable(ImaEasyCrypt main.cpp PNGImage.h PNGImage.cpp Chunk.h Chunk.cpp Pixel.h Pixel.cpp)
target_link_libraries(ImaEasyCrypt ZLIB::ZLIB)
