cmake_minimum_required(VERSION 3.19)

project(Chessatron)

file(GLOB SOURCES "src/*.cpp")
file(GLOB TESTS "tests/*.cpp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror -O3")

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

add_executable(Chessatron ${SOURCES})

target_include_directories(Chessatron PRIVATE src)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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