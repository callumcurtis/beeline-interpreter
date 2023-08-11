set(DEMO_SCRIPT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/demo PARENT_SCOPE)
set(DEMO_SCRIPT_DEPENDENCIES
    ${CMAKE_CURRENT_SOURCE_DIR}/comments.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/arithmetic.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/concatenation.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/comparisons.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/logical_operators.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/precedence.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/variables.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/conditionals.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/while_loops.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/scope.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/fibonnaci.txt
    PARENT_SCOPE
)
