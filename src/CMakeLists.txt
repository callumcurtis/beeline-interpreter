find_package(Boost
    REQUIRED COMPONENTS
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
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(beeline_lib
    PRIVATE
    ${Boost_LIBRARIES}
)

add_library(Beeline::beeline ALIAS beeline_lib)