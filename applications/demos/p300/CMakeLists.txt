
# FILE(GLOB FILENAMES "bci-examples/*") 
# FOREACH(FULLPATH ${FILENAMES})
# 	IF(IS_DIRECTORY ${FULLPATH} AND NOT ${FULLPATH} MATCHES ".*\\.svn")
#		GET_FILENAME_COMPONENT(DIRNAME ${FULLPATH} NAME)
#		IF(${DIRNAME} != "signals")
#			# MESSAGE(STATUS "HEP ${DIRNAME}")
#			INSTALL(DIRECTORY ${DIRNAME} DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/openvibe/scenarios/bci-examples/ PATTERN ".svn" 	 EXCLUDE)
#		ENDIF(${DIRNAME} != sig
#	ENDIF(IS_DIRECTORY ${FULLPATH} AND NOT ${FULLPATH} MATCHES ".*\\.svn")
#ENDFOREACH(FULLPATH ${FILENAMES})

# INSTALL(DIRECTORY p300-speller/       DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/openvibe/bci-examples   PATTERN ".svn" EXCLUDE)
# INSTALL(DIRECTORY p300-speller-xDAWN/ DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/openvibe/bci-examples   PATTERN ".svn" EXCLUDE)

INSTALL(DIRECTORY signals      DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/openvibe/scenarios/   PATTERN ".svn" EXCLUDE)
INSTALL(DIRECTORY bci-examples DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/openvibe/scenarios/   PATTERN ".svn" EXCLUDE)

# FILE(GLOB FILENAMES "*") 
# FOREACH(FULLPATH ${FILENAMES})
#	IF(IS_DIRECTORY ${FULLPATH} AND NOT ${FULLPATH} MATCHES ".*\\.svn")
#		GET_FILENAME_COMPONENT(DIRNAME ${FULLPATH} NAME)
#		INSTALL(DIRECTORY ${DIRNAME} DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/openvibe/scenarios PATTERN ".svn" EXCLUDE)
#	ENDIF(IS_DIRECTORY ${FULLPATH} AND NOT ${FULLPATH} MATCHES ".*\\.svn")
#ENDFOREACH(FULLPATH ${FILENAMES})

