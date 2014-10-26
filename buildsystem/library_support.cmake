FUNCTION(ADD_DEPENDENCY_TO_BOOST VERSION)

    SET(Boost_USE_STATIC_LIBS ON)
    SET(Boost_USE_STATIC_RUNTIME OFF)
    SET(Boost_USE_MULTITHREADED ON)

    FIND_PACKAGE(Boost ${VERSION} REQUIRED COMPONENTS filesystem unit_test_framework thread system)
    IF(NOT ("${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" STREQUAL ${VERSION}))
        MESSAGE(WARNING "Using Boost ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} - application is not tested with ${VERSION}, so keep cautious!")
    ENDIF(NOT ("${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" STREQUAL ${VERSION}))

    MESSAGE(STATUS "Using Boost version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
#`    MESSAGE(STATUS "Using Boost libraries: ${Boost_LIBRARIES}")
    MESSAGE(STATUS "Using Boost libraries from: ${Boost_LIBRARY_DIRS}")
    MESSAGE(STATUS "Using Boost headers from: ${Boost_INCLUDE_DIRS}")
ENDFUNCTION(ADD_DEPENDENCY_TO_BOOST VERSION)

FUNCTION(ADD_DEPENDENCY_TO_OPEN_CV)
    FIND_PACKAGE(OpenCV REQUIRED COMPONENTS core imgproc)
    MESSAGE(STATUS "Using OpenCV version: ${OpenCV_VERSION}")
    MESSAGE(STATUS "Using OpenCV libraries: ${OpenCV_LIBS}")
    MESSAGE(STATUS "Using OpenCV libraries from: ${OpenCV_LIB_DIR}")
    MESSAGE(STATUS "Using OpenCV headers from: ${OpenCV_INCLUDE_DIRS}")
ENDFUNCTION(ADD_DEPENDENCY_TO_OPEN_CV)
