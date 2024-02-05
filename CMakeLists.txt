cmake_minimum_required(VERSION 3.16)

project(Chessatron)

file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")
file(GLOB TESTS "tests/*.cpp" "tests/**/*.cpp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-private-field -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g --coverage")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g --coverage")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-ops-limit=100000000")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=100000000")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

add_executable(Chessatron ${SOURCES})

target_include_directories(Chessatron PRIVATE src)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(Dart)
include(GoogleTest)

add_executable(
    chessatron_tests
    ${TESTS}
    ${SOURCES}
)

if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET Chessatron PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET chessatron_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

target_link_libraries(
    chessatron_tests
    GTest::gtest_main
)

target_compile_definitions(chessatron_tests PUBLIC IS_TESTING)

gtest_discover_tests(chessatron_tests)