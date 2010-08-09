PROJECT(Tools CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

SET(MODULE_NAME "Tools")

PROJECT(${MODULE_NAME} CXX C)

SET(${MODULE_NAME}_HEADERS
	io/AbstractCamera.hpp
	math/Utils.hpp
	math/CostFunction.h
	math/LevenbergMarquard.h
	math/NumericalJacobian.h
)

SET(${MODULE_NAME}_SOURCES
	math/Utils.cpp
	math/CostFunction.cpp
	math/LevenbergMarquard.cpp
#	math/NumericalJacobian.cpp
)

FIND_PACKAGE(Boost 1.42 COMPONENTS filesystem system program_options REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)

# include directories
INCLUDE_DIRECTORIES(
    BEFORE
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${Eigen2_INCLUDE_DIR}
)

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

MODULE_CONFIGURATION("${MODULE_NAME}" 
					 "${${MODULE_NAME}_HEADERS}" 
					 "${${MODULE_NAME}_SOURCES}" 
					 "${${MODULE_NAME}_DEP_LIBRARIES}")
