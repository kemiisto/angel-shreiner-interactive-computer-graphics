cmake_minimum_required(VERSION 3.2)

project(interactive-computer-graphics)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wno-gnu-anonymous-struct -Wno-nested-anon-types)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W3 /WX)
endif()

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(tinygl)
include_directories(src tinygl/include tinygl/imgui)
link_libraries(fmt::fmt spdlog::spdlog tinygl)

set(CHAPTERS
    02
    03
    04
)

set(02
    gasket1
    gasket2
    gasket3
    gasket3v2
    gasket4
)

set(03
    rotatingSquare1
    rotatingSquare2
    square
    cad1
    cad2
)

set(04
    cube
    cubev)

foreach(CHAPTER ${CHAPTERS})
    message(STATUS "Configuring demos for chapter ${CHAPTER}")
    foreach(DEMO ${${CHAPTER}})
        message(STATUS "Configuring demo ${DEMO}")
        set(NAME ${CHAPTER}-${DEMO})
        add_executable(${NAME} ${CHAPTER}/${DEMO}.cpp)
        target_compile_definitions(${NAME} PRIVATE NAME="${NAME}")
        file(GLOB SHADERS ${CHAPTER}/*.frag ${CHAPTER}/*.vert)
        foreach(SHADER ${SHADERS})
            add_custom_command(TARGET ${NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
        endforeach(SHADER)
    endforeach(DEMO)
endforeach(CHAPTER)
