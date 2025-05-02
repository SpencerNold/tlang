cmake_minimum_required(VERSION 4.0.0)
project(TLang VERSION 0.0.1 DESCRIPTION "Trefoil-Lang Compiler")

find_program(RE2C_EXE re2c REQUIRED)

set(LEXER_IN ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.re)
set(LEXER_OUT ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.c)

add_custom_command(
    OUTPUT ${LEXER_OUT}
    COMMAND ${RE2C_EXE} -o ${LEXER_OUT} ${LEXER_IN}
    DEPENDS ${LEXER_IN}
    COMMENT "Generating lexer.c with re2c"
    VERBATIM
)

set(SOURCES
    src/lexer.c
    src/tok_queue.c
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    src/main.c
)

target_include_directories(${PROJECT_NAME}
    PRIVATE include
)