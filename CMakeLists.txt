# CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(HeavyMatcher)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC -mcmodel=large")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Werror -O3 -fPIC -mcmodel=large")

# Include directories for the libraries
include_directories(include)

# cuckoo executable
add_executable(HeavyMatcher main.cpp)

# Link the libraries with the executable
target_link_libraries(HeavyMatcher
    ${CMAKE_SOURCE_DIR}/lib/libcuckoosketchflag.so
    ${CMAKE_SOURCE_DIR}/lib/libcuckoosketchpro.so
)

# Set the output directory for the executable
set_target_properties(HeavyMatcher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
