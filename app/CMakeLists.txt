find_package(Boost
    REQUIRED COMPONENTS
    program_options
)

add_executable(beeline
    beeline.cpp
    cli.cpp
)

target_include_directories(beeline
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(beeline
    PRIVATE
    Beeline::beeline
    ${Boost_LIBRARIES}
)