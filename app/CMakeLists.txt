ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package(Boost
    1.82.0
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
)

target_link_libraries(beeline
    PRIVATE
    Beeline::beeline
    Boost::program_options
)