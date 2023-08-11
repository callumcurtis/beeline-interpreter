add_executable(beeline
    beeline.cpp
    cli.cpp
)

target_include_directories(beeline
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(beeline PRIVATE Beeline::beeline)