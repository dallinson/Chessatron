cmake_minimum_required(VERSION 3.16)

project(Chessatron LANGUAGES CXX)

option(PGO "Whether to enable or disable profile-guided optimisations" OFF)
option(RELEASES "Build specific release binaries" OFF)

file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")
file(GLOB TESTS "tests/*.cpp" "tests/**/*.cpp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-unused-private-field")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g --coverage")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g --coverage")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-ops-limit=250000000")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=250000000")
endif()

add_executable(Chessatron ${SOURCES})
target_include_directories(Chessatron PRIVATE src)
target_compile_options(Chessatron PRIVATE -march=native -mtune=native)
add_definitions(-DIS_TUNE)

if(RELEASES)
    add_definitions(-DCHESSATRON_VERSION="${CHESSATRON_VERSION}")

    add_executable(Chessatron_x86-64-v3 ${SOURCES})
    target_include_directories(Chessatron_x86-64-v3 PRIVATE src)
    target_compile_options(Chessatron_x86-64-v3 PRIVATE -march=x86-64-v3)

    add_executable(Chessatron_x86-64-v2 ${SOURCES})
    target_include_directories(Chessatron_x86-64-v2 PRIVATE src)
    target_compile_options(Chessatron_x86-64-v2 PRIVATE -march=x86-64-v2)
endif()

if(PGO)
    if(EXISTS ${CMAKE_BINARY_DIR}/chessatron.profdata)
        target_compile_options(Chessatron PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/chessatron.profdata)
        target_link_options(Chessatron PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/chessatron.profdata)
        if(RELEASES)
            target_compile_options(Chessatron_x86-64-v3 PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/chessatron.profdata)
            target_link_options(Chessatron_x86-64-v3 PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/chessatron.profdata)

            target_compile_options(Chessatron_x86-64-v2 PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/chessatron.profdata)
            target_link_options(Chessatron_x86-64-v2 PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/chessatron.profdata)
        endif()
    else()
        target_compile_options(Chessatron PRIVATE -fprofile-generate)
        target_link_options(Chessatron PRIVATE -fprofile-generate)
        if(RELEASES)
            target_compile_options(Chessatron_x86-64-v3 PRIVATE -fprofile-generate)
            target_link_options(Chessatron_x86-64-v3 PRIVATE -fprofile-generate)

            target_compile_options(Chessatron_x86-64-v2 PRIVATE -fprofile-generate)
            target_link_options(Chessatron_x86-64-v2 PRIVATE -fprofile-generate)
        endif()
    endif()
endif()


include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/61db1e1740a828d9df94fd167a9eb4137cd6def2.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        40626af88bd7df9a5fb80be7b25ac85b122d6c21) # 11.2.0
FetchContent_MakeAvailable(fmt)

enable_testing()

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
    if (RELEASES)
        set_property(TARGET Chessatron_x86-64-v3 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET Chessatron_x86-64-v2 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

target_link_libraries(
    chessatron_tests
    GTest::gtest_main
)

target_link_libraries(Chessatron fmt)
target_link_libraries(chessatron_tests fmt)
if (RELEASES)
    target_link_libraries(Chessatron_x86-64-v3 fmt)
    target_link_libraries(Chessatron_x86-64-v2 fmt)
endif()

target_compile_definitions(chessatron_tests PUBLIC IS_TESTING)

gtest_discover_tests(chessatron_tests)