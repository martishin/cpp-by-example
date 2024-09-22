cmake_minimum_required(VERSION 3.29)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
else()
    message(FATAL_ERROR "Please define VCPKG_ROOT to point to the vcpkg directory")
endif()

project(cpp_by_example)

set(CMAKE_CXX_STANDARD 20)

find_package(boost_asio CONFIG REQUIRED)

add_executable(hello_world src/hello_world.cpp)

add_executable(asio_hello_world src/asio_hello_world.cpp)
target_link_libraries(asio_hello_world PRIVATE Boost::asio)
