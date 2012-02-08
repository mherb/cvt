FIND_FILE( GLCLTOHEADER_DST
	glcltoheader
	/usr/local/share/cvt/utils
	/usr/share/cvt/utils
)

MACRO(GLSLTOHEADER _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    GET_FILENAME_COMPONENT(_ext ${_filename} EXT )
    GET_FILENAME_COMPONENT(_path ${_filename} PATH )

	string(REGEX REPLACE "\\.(.*)" "\\1" _ext ${_ext} )

	STRING( STRIP "${_path}" _path )
	IF( NOT "${_path}" STREQUAL "" )       
		SET(_path "${_path}/" )
    ENDIF()

    ADD_CUSTOM_COMMAND(
		OUTPUT  "${CMAKE_CURRENT_SOURCE_DIR}/${_path}${_basename}_${_ext}.h"
		COMMAND "${GLCLTOHEADER_DST}" 
                "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}"
				"${CMAKE_CURRENT_SOURCE_DIR}/${_path}${_basename}_${_ext}.h"
				"${_basename}_${_ext}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}" 
    )
ENDMACRO(GLSLTOHEADER)

MACRO(CLTOHEADER _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    GET_FILENAME_COMPONENT(_path ${_filename} PATH )

	STRING( STRIP "${_path}" _path )
	IF( NOT "${_path}" STREQUAL "" )       
		SET(_path "${_path}/" )
    ENDIF()

    ADD_CUSTOM_COMMAND(
		OUTPUT  "${CMAKE_CURRENT_SOURCE_DIR}/${_path}${_basename}.h"		
		COMMAND "${GLCLTOHEADER_DST}" 
                "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}"
				"${CMAKE_CURRENT_SOURCE_DIR}/${_path}${_basename}.h"
				"${_basename}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}" 
    )
ENDMACRO(CLTOHEADER)


