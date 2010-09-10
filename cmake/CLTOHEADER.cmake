MACRO(CLTOHEADER _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
	SET(CLTOHEADER_DST "${CMAKE_BINARY_DIR}/bin/cltoheader")
    ADD_CUSTOM_COMMAND(
		OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.h		
        COMMAND "${CLTOHEADER_DST}" 
                ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
		${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.h
		${_basename}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
    )
ENDMACRO(CLTOHEADER)

