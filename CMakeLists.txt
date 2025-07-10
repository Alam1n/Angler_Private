cmake_minimum_required(VERSION 3.10)
project(MyApp)

# Tell CMake to use the vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add your executable
add_executable(MyApp main.cpp)

# Use nlohmann_json from vcpkg
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE nlohmann_json::nlohmann_json)
