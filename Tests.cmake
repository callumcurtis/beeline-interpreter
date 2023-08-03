find_package(Catch2 REQUIRED)

file(GLOB SOURCES "tests/unit/*.cpp" "tests/unit/*.hpp")

add_executable(tests ${SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2)