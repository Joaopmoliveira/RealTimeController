cmake_minimum_required(VERSION 3.24)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0075 NEW)

#Language definitions used throught the entire project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)


if(UNIX)
  set(VCPKG_TARGET_TRIPLET "x64-linux")
  set(VCPKG_HOST_TRIPLET "x64-linux")
endif(UNIX)

if(WIN32)
  set(VCPKG_TARGET_TRIPLET "x64-windows")
  set(VCPKG_HOST_TRIPLET "x64-windows")
endif(WIN32)

set(VCPKG_ROOT "${CMAKE_CURRENT_LIST_DIR}/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(Compiler)

if(WIN32)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif(WIN32)

enable_testing()

set(OPERATING_SYSTEM_REQUIRMENTS "")

if(UNIX)
  string(CONCAT OPERATING_SYSTEM_REQUIRMENTS "atomic")
endif(UNIX)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Find all required third parties (this should be moved elsewhere)
find_package(nlohmann_json REQUIRED)
find_package(asio CONFIG REQUIRED)


add_executable(Compiler 
src/compiler.cpp
)

target_link_libraries(Compiler nlohmann_json::nlohmann_json)

add_custom_command(
    OUTPUT Byproducts
    COMMAND Compiler "${CMAKE_CURRENT_LIST_DIR}/src/sharedmemory.json" "${CMAKE_CURRENT_LIST_DIR}/src"
    DEPENDS Compiler
    COMMENT "currently compiling shared memory layout from sharedmemory.json"
  )

# create a target that includes some_file, this gives us a name that we can use later
add_custom_target(
    CompilerByproducts
  DEPENDS Byproducts
)

add_executable(WatchDog 
src/watchdog.cpp
)

add_executable(Sensors 
src/simplified_sensors.cpp
)

add_executable(Client 
src/client.cpp
)

target_include_directories(Sensors PUBLIC 
${Boost_INCLUDE_DIRS}
)

target_include_directories(Client PUBLIC 
${Boost_INCLUDE_DIRS}
)

if(UNIX AND NOT APPLE)
    target_link_libraries(WatchDog PUBLIC 
    Threads::Threads
    asio)

    target_link_libraries(Sensors PUBLIC 
    Threads::Threads
    asio
    rt
    atomic
    )

    target_link_libraries(Client PUBLIC 
    Threads::Threads
    asio
    rt
    atomic
    )
else()
    target_link_libraries(Sensors PUBLIC 
    asio
    )

    target_link_libraries(Client PUBLIC 
    asio
    )

    target_link_libraries(WatchDog PUBLIC 
    asio)
endif()

# these projects require the shared memory layout to properly function, 
# thus we add a restriction so that the executables are only compiled 
# once the header filed with the shared memory layout have been compiled 
add_dependencies(Client CompilerByproducts)
add_dependencies(Sensors CompilerByproducts)
add_dependencies(WatchDog CompilerByproducts)