CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1)
CMAKE_POLICY(VERSION 2.8.1)

PROJECT(LIBRARIES)

SET(LIBRARIES_BASE_DIR ${PRODUCTION_DIR}/libs)

ADD_SUBDIRECTORY(potrace)
ADD_SUBDIRECTORY(pugixml)
ADD_SUBDIRECTORY(waffles)
ADD_SUBDIRECTORY(qgar)
#ADD_SUBDIRECTORY(libboard)
#ADD_SUBDIRECTORY(dxflib)
#ADD_SUBDIRECTORY(fog)
#ADD_SUBDIRECTORY(juce)

SET(POTRACE_INCLUDE_DIR ${POTRACE_INCLUDE_DIR} PARENT_SCOPE)
SET(PUGIXML_INCLUDE_DIR ${PUGIXML_INCLUDE_DIR} PARENT_SCOPE)
SET(LIBBOARD_INCLUDE_DIR ${LIBBOARD_INCLUDE_DIR} PARENT_SCOPE)
SET(DXFLIB_INCLUDE_DIR ${DXFLIB_INCLUDE_DIR} PARENT_SCOPE)
SET(JUCE_INCLUDE_DIRS ${JUCE_INCLUDE_DIRS} PARENT_SCOPE)
SET(FOG_INCLUDE_DIR ${FOG_INCLUDE_DIR} PARENT_SCOPE)
SET(WAFFLES_INCLUDE_DIR ${WAFFLES_INCLUDE_DIR} PARENT_SCOPE)
SET(VORONOI_INCLUDE_DIR ${LIBRARIES_BASE_DIR}/voronoi PARENT_SCOPE)
SET(QGAR_INCLUDE_DIR ${QGAR_INCLUDE_DIR} PARENT_SCOPE)

SET(SEMI_INTERNAL_LIBRARIES ${POTRACE_LIBRARIES} ${LIBBOARD_LIBRARIES} ${DXFLIB_LIBRARIES} ${JUCE_LIBRARIES} ${FOG_LIBRARIES} ${PUGIXML_LIBRARIES} ${WAFFLES_LIBRARIES} ${QGAR_LIBRARIES} PARENT_SCOPE)