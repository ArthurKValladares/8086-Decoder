cmake_minimum_required(VERSION 3.27)

project (8086)

set(project_root ${CMAKE_SOURCE_DIR})

include_directories(
    ${project_root}/8086/
)

add_executable(8086 
    ${project_root}/8086/8086_decode.cpp
    ${project_root}/8086/8086_text.cpp
    ${project_root}/8086/8086.cpp
    ${project_root}/8086/file.cpp
    ${project_root}/8086/main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)