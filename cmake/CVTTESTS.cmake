MACRO(CVTTESTS _LIB _OUT )
	IF(UNIX)
		IF(NOT APPLE)
			# linux
			SET(LIBNAME "lib/lib${_LIB}_d.so")
		ELSE()
			# mac os
			SET(LIBNAME "lib/${_LIB}_d.dylib")
		ENDIF()
	ELSE()
		# windows
	ENDIF()

    ADD_CUSTOM_COMMAND(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${_OUT}
        COMMAND ${CMAKE_SOURCE_DIR}/util/extracttests.sh
                ${CMAKE_BINARY_DIR}/${LIBNAME}
		${CMAKE_CURRENT_SOURCE_DIR}/${_OUT}
        DEPENDS ${_LIB}
    )
ENDMACRO(CVTTESTS)

