# This module finds OpenMASK (see openmask.org/) 
# and determines where the
# include files and libraries are. 
# On Unix/Linux it relies on the output of omk-config.
##
# This code sets the following variables:
##
# OMK_FOUND       = system has OpenMASK lib
##
# OMK_INCFLAGS   = Cflags used on gcc
##
# OMK_INC      = where to find headers 
##
# OMK_LIBS     = libs used by OpenMASK
##
# OMK_LIBFLAGS   = ldflags used on gcc
##
# OMK_LIBDIR   = full path(S) to the libraries, ie OpenSG path + ...
##
# Xavier Larrod� 2006
# xlarrode@irisa.fr 
# or see http://openmask.org/wiki or http://openmask.org/forum
##


IF(WIN32)
	FIND_PATH(PATH_OPENMASK include/OpenMASK.h PATHS $ENV{OpenMASKDIR})
	IF(PATH_OPENMASK)
		MESSAGE(STATUS "  Found OpenMASK...")
		SET(OMK_FOUND TRUE)
		SET(OMK_INC ${PATH_OPENMASK}/include)
		SET(OMK_INCFLAGS "")
		SET(OMK_LIBS 3DVisD OpenMASKD SimpleLoaderD SoundD UserTypesD XmlLoaderD)
		SET(OMK_LIBFLAGS "")
		SET(OMK_LIBDIR ${PATH_OPENMASK}/lib)
	ELSE(PATH_OPENMASK)
		MESSAGE(STATUS "  FAILED to find OpenMASK...")
	ENDIF(PATH_OPENMASK)
ENDIF(WIN32)

# --------------------------------------------------------------

IF(UNIX) 
  SET(OMK_CONFIG_PATH "$ENV{OpenMASKDIR}/bin" CACHE STRING "preferred path to OpenMASK (omk-config)")
  #find omk  cflags and ldflags 
  FIND_PROGRAM(OMK_CONFIG "omk-config"
    ${OMK_CONFIG_PATH}
    $ENV{OpenMASKDIR}
    . )
  IF(OMK_CONFIG)
    SET(OMK_ARG_CFLAGS "--debug --cxxflags")
    SET(OMK_ARG_LDFLAGS "--debug --ldflags")
    EXEC_PROGRAM(${OMK_CONFIG}
      ARGS ${OMK_ARG_CFLAGS}
      OUTPUT_VARIABLE OMK_CFLAGS)
    EXEC_PROGRAM(${OMK_CONFIG}
      ARGS ${OMK_ARG_LDFLAGS}
      OUTPUT_VARIABLE OMK_LDFLAGS)
  ELSE(OMK_CONFIG)
    MESSAGE(STATUS "Cannot find omk-config anywhere on the system. Please put the file into your path or specify it in OMK_CONFIG.")
  ENDIF(OMK_CONFIG)

 
 SET(OMK_LIBDIR_EXTRACT_REGEX "[-][l]([a-zA-Z0-9/._+-]*)")
  SET(OMK_LIBS_EXTRACT_REGEX "[-][L]([ a-zA-Z0-9/._+-]*)")

  #add new delimiters % and handle -I /titi and -L /titi 
  STRING(REGEX REPLACE "-L " "-L" OMK_LDFLAGS ${OMK_LDFLAGS})
  STRING(REGEX REPLACE " " "%" OMK_LDFLAGS ${OMK_LDFLAGS})
  STRING(REGEX REPLACE "-I " "-I" OMK_CFLAGS ${OMK_CFLAGS})
  STRING(REGEX REPLACE " " "%" OMK_CFLAGS ${OMK_CFLAGS})
  #Get OMK_LINK Property : LIBFLAGS LIBDIR LIBS 
  ##  LDFLAGS_MINUS_LIB  :   erase -l*
  STRING(REGEX REPLACE "${OMK_LIBDIR_EXTRACT_REGEX}" ""
    OMK_LDFLAGS_MINUS_LIB "${OMK_LDFLAGS}")
  ### AREVI_LIBFLAGS = OMK_LDFLAGS_MINUS_LIB MINUS LIBDIR -space
  #erase -L and space
  STRING(REGEX REPLACE "${OMK_LIBS_EXTRACT_REGEX}" ""
    OMK_LIBFLAGS "${OMK_LDFLAGS_MINUS_LIB}")
  STRING(REGEX REPLACE "%" "" OMK_LIBFLAGS "${OMK_LIBFLAGS}")
  #Handle if there is no libFlags
  IF(OMK_LIBFLAGS)
  ELSE(OMK_LIBFLAGS)
    SET(OMK_LIBFLAGS " ")
  ENDIF(OMK_LIBFLAGS)
  ### OMK_LIBDIR = OMK_LDFLAGS_MINUS_LIB - OMK_LIBFLAGS -space
  #erase libFlags - space -L
  STRING(REGEX REPLACE "${OMK_LIBFLAGS}" ""
    OMK_LIBDIR "${OMK_LDFLAGS_MINUS_LIB}")
    STRING(REGEX REPLACE "%" "" OMK_LIBDIR "${OMK_LIBDIR}")
  STRING(REGEX REPLACE "-L" ";" OMK_LIBDIR "${OMK_LIBDIR}")
   ### OMK_LIBS = OMK_LDFLAGS - LIBS -libFlags -space -l 
  
  STRING(REGEX REPLACE "${OMK_LIBS_EXTRACT_REGEX}" ""
    OMK_LIBS "${OMK_LDFLAGS}")
  STRING(REGEX REPLACE "${OMK_LIBFLAGS}" ""
    OMK_LIBS "${OMK_LIBS}")
  STRING(REGEX REPLACE "%" "" OMK_LIBS "${OMK_LIBS}")
  STRING(REGEX REPLACE "-l" ";" OMK_LIBS "${OMK_LIBS}")

  SET(OMK_INCDIR_EXTRACT_REGEX "[-][I]([ a-zA-Z0-9/._+-]*)")
  #extract flags and include dir
  IF("${OMK_CFLAGS}" MATCHES "${OMK_INCDIR_EXTRACT_REGEX}")
    STRING(REGEX REPLACE "${OMK_INCDIR_EXTRACT_REGEX}" ""
      OMK_INCFLAGS "${OMK_CFLAGS}")
    STRING(REGEX MATCHALL "${OMK_INCDIR_EXTRACT_REGEX}"
      OMK_INC "${OMK_CFLAGS}")
    #carefull space make strnge \
    STRING(REGEX REPLACE "%" "" OMK_INC "${OMK_INC}")
    STRING(REGEX REPLACE "-I" "" OMK_INC "${OMK_INC}")
    STRING(REGEX REPLACE "%" " " OMK_INCFLAGS "${OMK_INCFLAGS}")
   ENDIF("${OMK_CFLAGS}" MATCHES "${OMK_INCDIR_EXTRACT_REGEX}")

  IF(OMK_CONFIG)
    IF(OMK_INC AND OMK_LIBS AND OMK_LIBDIR)
      SET(OMK_FOUND ON)
    ELSE(OMK_INC AND OMK_LIBS AND OMK_LIBDIR)
      MESSAGE("Error Parsing arevi-config,")
      MESSAGE("Got  OMK_INC : ${OMK_INC}") 
      MESSAGE("Got  OMK_LIBS : ${OMK_LIBS}") 
      MESSAGE("Got  OMK_LIBFLAGS : ${OMK_LIBFLAGS}") 
      MESSAGE("Got  OMK_LIBDIR : ${OMK_LIBDIR}") 
    ENDIF(OMK_INC AND OMK_LIBS AND OMK_LIBDIR)
  ENDIF(OMK_CONFIG)


ENDIF(UNIX)
MARK_AS_ADVANCED(
  OMK_FOUND
  OMK_INCFLAGS
  OMK_INC
  OMK_LIBS
  OMK_LIBDIR
  OMK_LIBFLAGS
  )

