cmake_minimum_required(VERSION 3.21)
project(csc3081_farm_project LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(cmake/FreeGLUT.cmake)
set(FARM_TEXT_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/assets/text.txt")
include(cmake/EmbedText.cmake)
find_package(OpenGL REQUIRED)
file(GLOB_RECURSE FARM_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
add_executable(csc3081_farm_project ${FARM_SOURCES})
target_include_directories(
  csc3081_farm_project PRIVATE "${freeglut_SOURCE_DIR}/include"
                               "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(csc3081_farm_project PRIVATE freeglut_static OpenGL::GL
                                                   OpenGL::GLU)
target_compile_definitions(
  csc3081_farm_project PRIVATE NOMINMAX FREEGLUT_STATIC
                               GLUT_DISABLE_ATEXIT_HACK)
set_property(TARGET csc3081_farm_project PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
                                                  "${CMAKE_SOURCE_DIR}")
if(MSVC)
  target_compile_options(csc3081_farm_project PRIVATE /W4 /permissive-)
else()
  target_compile_options(csc3081_farm_project PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_custom_command(
  TARGET csc3081_farm_project
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:csc3081_farm_project>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${freeglut_SOURCE_DIR}/COPYING"
          "$<TARGET_FILE_DIR:csc3081_farm_project>/FREEGLUT-LICENSE.txt"
  VERBATIM)
enable_testing()
add_test(NAME farm_invariants COMMAND csc3081_farm_project --self-test)
set_tests_properties(
  farm_invariants PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" TIMEOUT 30)
add_test(NAME farm_render
         COMMAND csc3081_farm_project --capture
                 "${CMAKE_BINARY_DIR}/verification/overview.bmp")
set_tests_properties(farm_render PROPERTIES WORKING_DIRECTORY
                                            "${CMAKE_SOURCE_DIR}" TIMEOUT 30)
