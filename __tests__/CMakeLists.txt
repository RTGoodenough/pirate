cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME pirate_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

project(
  ${PROJECT_NAME}
  LANGUAGES CXX
)

add_executable(
  ${PROJECT_NAME}
  testing.cpp
  arguments.tests.cpp
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  .. pirate
)

target_link_libraries(
  ${PROJECT_NAME}
  GTest::gtest_main
  GTest::gmock_main
)

target_compile_options(
  ${PROJECT_NAME} 
  PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wpedantic >
)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} )