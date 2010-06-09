# ---------------------------------
# Finds Neurosky ThinkGear library
# Adds library to target
# Adds include path
# ---------------------------------
IF(WIN32)
	FIND_PATH(PATH_ThinkGearAPI thinkgear.h PATHS "C:/Program Files/MindSet Development Tools/tgcd/win32" $ENV{OpenViBE_dependencies})
	IF(PATH_ThinkGearAPI)
		MESSAGE(STATUS "  Found ThinkGear API...")
		INCLUDE_DIRECTORIES(${PATH_ThinkGearAPI})
		FIND_LIBRARY(LIB_ThinkGearAPI thinkgear PATHS ${PATH_ThinkGearAPI} )
		IF(LIB_ThinkGearAPI)
			MESSAGE(STATUS "    [  OK  ] lib ${LIB_ThinkGearAPI}")
			TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dynamic ${LIB_ThinkGearAPI} )
		ELSE(LIB_ThinkGearAPI)
			MESSAGE(STATUS "    [FAILED] lib thinkgear")
		ENDIF(LIB_ThinkGearAPI)

		ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyThinkGearAPI)
		
		# Copying the DLL file at postbuild
		ADD_CUSTOM_COMMAND(
				TARGET ${PROJECT_NAME}-dynamic
				POST_BUILD
				COMMAND ${CMAKE_COMMAND}
				ARGS -E copy "${PATH_ThinkGearAPI}/thinkgear.dll" "${PROJECT_SOURCE_DIR}/bin"
				COMMENT "      --->   Copying dll file ${PATH_ThinkGearAPI}/thinkgear.dll for the Neurosky MindSet driver."
			VERBATIM)
			
	ELSE(PATH_ThinkGearAPI)
		MESSAGE(STATUS "  FAILED to find ThinkGear API - cmake looked in 'C:/Program Files/MindSet Development Tools/tgcd/win32' and '$ENV{OpenViBE_dependencies}'")
	ENDIF(PATH_ThinkGearAPI)
ENDIF(WIN32)

IF(UNIX)
	MESSAGE(STATUS "The ThinkGear API for Neurosky MindSet is only available on windows.")
ENDIF(UNIX)