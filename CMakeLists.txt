cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(beeline_interpreter LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

include(Sanitizers.cmake)

option(ENABLE_COVERAGE "Enable Lcov code-coverage analysis" false)

# Taken from: https://www.youtube.com/watch?v=_KM0rDQYFSg
if(ENABLE_COVERAGE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)
    include(CodeCoverage.cmake)
    append_coverage_compiler_flags()
endif()

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(test)

if(ENABLE_COVERAGE)
    # Create the coverage target.
endif()