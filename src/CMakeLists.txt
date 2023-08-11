ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package(Boost
    1.82.0
    REQUIRED COMPONENTS
    program_options
    log
)

add_library(beeline_lib
    beeline.cpp
    lexer.cpp
    logging.cpp
    ast.cpp
    stringify.cpp
    parser.cpp
    interpreter.cpp
    environment.cpp
)

target_include_directories(beeline_lib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(beeline_lib
    PRIVATE
    Boost::program_options
    Boost::log
)

add_library(Beeline::beeline ALIAS beeline_lib)