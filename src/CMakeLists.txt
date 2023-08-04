find_package(Boost
    REQUIRED COMPONENTS
    program_options
    log
)

add_custom_command(
    OUTPUT ast.hpp
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/generate_ast.py hpp > ast.hpp
    COMMENT "Generating AST header file"
    VERBATIM
)

add_custom_command(
    OUTPUT ast.cpp
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/generate_ast.py cpp --header \"ast.hpp\" > ast.cpp
    DEPENDS ast.hpp
    COMMENT "Generating AST source file"
    VERBATIM
)

add_library(beeline_lib
    beeline.cpp
    lexer.cpp
    logging.cpp
    ast.cpp
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