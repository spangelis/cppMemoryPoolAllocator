cmake_minimum_required(VERSION 3.27)
project(cppMemoryPoolAllocator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")


foreach(src_file ${SOURCES})
    get_filename_component(src_file_name ${src_file} NAME_WE)
    add_executable(${src_file_name} ${src_file})
endforeach()
