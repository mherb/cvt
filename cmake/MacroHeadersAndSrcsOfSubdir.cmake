# (C) 2010, Sebastian Klose, <kloses@in.tum.de>

# get all headers and cpp files in the folder:
MESSAGE("GLOB_RECURSING DIRECTORY: ${CMAKE_CURRENT_SRC_DIR}/${DIR}" )
FILE(GLOB_RECURSE ${SOURCES} "${DIR}/*.cpp" "${DIR}/*.c")
FILE(GLOB_RECURSE ${HDRS} "${DIR}/*.h" "${DIR}/*.hpp")

MESSAGE("${SOURCES}")
MESSAGE("${HDRS}")

ENDMACRO(HEADERS_AND_SRCS_OF_SUBDIR)
