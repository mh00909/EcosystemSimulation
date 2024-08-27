# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ecosystem_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ecosystem_autogen.dir/ParseCache.txt"
  "ecosystem_autogen"
  )
endif()
