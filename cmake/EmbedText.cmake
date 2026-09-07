# Both build paths embed the same editable file for missing-asset fallback.
set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS "${FARM_TEXT_INPUT}")
file(READ "${FARM_TEXT_INPUT}" FARM_TEXT)
string(FIND "${FARM_TEXT}" ")FARMTEXT\"" FARM_TEXT_DELIMITER)
if(NOT FARM_TEXT_DELIMITER EQUAL -1)
  message(FATAL_ERROR "text.txt contains the reserved raw-string delimiter")
endif()
configure_file("${CMAKE_CURRENT_LIST_DIR}/TextDefaults.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/generated/TextDefaults.h" @ONLY)
