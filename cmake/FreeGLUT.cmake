include(FetchContent)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(FREEGLUT_BUILD_DEMOS
    OFF
    CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_STATIC_LIBS
    ON
    CACHE BOOL "" FORCE)
set(FREEGLUT_INSTALL_MAN_PAGES
    OFF
    CACHE BOOL "" FORCE)

FetchContent_Declare(
  freeglut
  URL https://codeload.github.com/freeglut/freeglut/zip/refs/tags/v3.8.0
  URL_HASH
    SHA256=66c12fbf41ef5da34a386d59dbd389e30b601124937f8edb81a30a5006247f93
  TLS_VERIFY TRUE)
FetchContent_MakeAvailable(freeglut)
