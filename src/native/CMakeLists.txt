cmake_minimum_required(VERSION 3.17)
project(native)

set(CMAKE_CXX_STANDARD 17)

add_library(jpegxl_writer SHARED jpegxl_writer.cpp dev_ferrand_chunky_jpegxl_JpegxlWriter.h)


set_target_properties(jpegxl_writer PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        )

find_package(JNI)
target_link_libraries(jpegxl_writer PUBLIC ${JNI_LIBRARIES})
target_include_directories(jpegxl_writer PUBLIC ${JNI_INCLUDE_DIRS})

target_link_libraries(jpegxl_writer PUBLIC jxl)
target_link_libraries(jpegxl_writer PUBLIC jxl_threads)