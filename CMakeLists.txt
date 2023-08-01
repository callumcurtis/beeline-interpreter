cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(beeline_interpreter LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

include(Sanitizers.cmake)

option(ENABLE_COVERAGE "Enable Lcov code-coverage analysis" false)

find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options log)

# Taken from: https://www.youtube.com/watch?v=_KM0rDQYFSg
if(ENABLE_COVERAGE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)
    include(CodeCoverage.cmake)
    append_coverage_compiler_flags()
endif()

file(GLOB SOURCES "src/*.cpp" "src/*.hpp")
add_executable(beeline ${SOURCES})
target_include_directories(beeline PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(beeline PRIVATE ${Boost_LIBRARIES})

if(ENABLE_COVERAGE)
    setup_target_for_coverage_lcov(
        NAME test_queue_coverage
        EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/test_queue
        LCOV_ARGS --rc lcov_branch_coverage=1
        GENHTML_ARGS --legend --branch-coverage
        DEPENDENCIES test_queue
    )
endif()