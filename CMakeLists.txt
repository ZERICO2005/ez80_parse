cmake_minimum_required(VERSION 3.12)

set(SRC_DIR "./src")

set(PROJECT_NAME "parse_asm")
project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

# Compiler
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

# Set C and C++ standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")
# Create an executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

#target_include_directories(${PROJECT_NAME} PUBLIC )
# Compiler Flags Debug(-g -O0) Release(-O3)
set(OPT_FLAG -O2)
target_compile_options(
	${PROJECT_NAME} PUBLIC ${OPT_FLAG}
	-Wall -Wextra -Wshadow
	-Wno-unused-parameter
	-Wno-unused-variable
	-Wno-missing-braces
	-Wimplicit-fallthrough
)
target_link_libraries(${PROJECT_NAME} PUBLIC)
