cmake_minimum_required(VERSION 3.2)

if(WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "c:/Users/$ENV{USERNAME}/Downloads/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

project(interactive-computer-graphics)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

find_package(glfw3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_subdirectory(tinygl)
include_directories(src tinygl/include)
link_libraries(fmt::fmt spdlog::spdlog tinygl)

set(CHAPTERS
    02
)

set(02
    gasket1
    gasket2
    gasket3
)

foreach(CHAPTER ${CHAPTERS})
    message(STATUS "Configuring demos for chapter ${CHAPTER}")
    foreach(DEMO ${${CHAPTER}})
        message(STATUS "Configuring demo ${DEMO}")
        set(NAME ${CHAPTER}-${DEMO})
        add_executable(${NAME} ${CHAPTERS}/${DEMO}.cpp)
        target_compile_definitions(${NAME} PRIVATE NAME="${NAME}")
        file(GLOB SHADERS ${CHAPTERS}/*.glsl)
        foreach(SHADER ${SHADERS})
            add_custom_command(TARGET ${NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
        endforeach(SHADER)
    endforeach(DEMO)
endforeach(CHAPTER)
