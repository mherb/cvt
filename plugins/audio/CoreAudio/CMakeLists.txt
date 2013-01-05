INCLUDE(MacroPlugin)

IF(UNIX)
   IF(APPLE)
	MACRO_PLUGIN( "CoreAudio" "" "-framework CoreAudio" )
	ENDIF()
ENDIF()
