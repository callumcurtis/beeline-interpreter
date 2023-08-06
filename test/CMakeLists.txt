find_package(Catch2 REQUIRED)

add_executable(tests
    unit/test_lexer.cpp
    unit/ast/test_ast.cpp
)

target_include_directories(tests
    PRIVATE
    $<TARGET_PROPERTY:Beeline::beeline,INCLUDE_DIRECTORIES>
)

target_link_libraries(tests
    PRIVATE
    Beeline::beeline
    Catch2::Catch2WithMain
)
