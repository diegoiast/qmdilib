cmake_minimum_required (VERSION 2.6) 
project (qmdilib) 
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# $Id$
# build the library 
#
set(QMDILIB_VERSION "0.0.5")
set(QMDILIB_SOVERSION "0")

include_directories(src)
set(QMDILIB_SOURCES
	src/qmdiactiongroup.cpp
	src/qmdiactiongrouplist.cpp
	src/qmdiclient.cpp
	src/qmdihost.cpp
	src/qmdiserver.cpp
	src/qmditabwidget.cpp
	src/qmdiworkspace.cpp
)
set(QMDILIB_MOCHEADERS
	src/qmditabwidget.h
	src/qmdiworkspace.h
)
QT4_WRAP_CPP(QMDILIB_MOC ${QMDILIB_MOCHEADERS} )
add_library(QMDILIB_STATIC STATIC ${QMDILIB_SOURCES} ${QMDILIB_MOC})
add_library(QMDILIB_SHARED SHARED ${QMDILIB_SOURCES} ${QMDILIB_MOC})
# add_library(qmdilib_shared SHARED)
# target_link_libraries(qmdilib_shared qmdilib_static)

set_target_properties(
  QMDILIB_SHARED
    PROPERTIES
      VERSION     ${QMDILIB_VERSION}
      SOVERSION   ${QMDILIB_SOVERSION}
)

if(MSVC)
	set_target_properties(QMDILIB_STATIC PROPERTIES OUTPUT_NAME qmdi_static)
	set_target_properties(QMDILIB_SHARED PROPERTIES OUTPUT_NAME qmdi)
else()
	set_target_properties(QMDILIB_STATIC PROPERTIES OUTPUT_NAME qmdi)
	set_target_properties(QMDILIB_SHARED PROPERTIES OUTPUT_NAME qmdi)
endif()

#
# common definitions for all demos
#
include_directories(demos/common)
set(QDMDILIB_COMMON_SOURCES demos/common/helpbrowse.cpp demos/common/qexeditor.cpp)
set(QDMDILIB_COMMON_HEADERS demos/common/helpbrowse.h   demos/common/qexeditor.h  )
QT4_ADD_RESOURCES(QDMDILIB_COMMON_RC_SRCS demos/common/common.qrc )

#
# demos
#
set(QMDILIB_DEMO1_SOURCES demos/demo1/main.cpp demos/demo1/mainwindow.cpp)
set(QMDILIB_DEMO1_HEADERS demos/demo1/mainwindow.h)
QT4_WRAP_CPP(QMDILIB_DEMO1_MOC ${QMDILIB_DEMO1_HEADERS})
add_executable(demo1 ${QMDILIB_DEMO1_SOURCES} ${QMDILIB_DEMO1_MOC})
target_link_libraries(demo1 ${QT_LIBRARIES} QMDILIB_STATIC)

set(QMDILIB_DEMO2_SOURCES demos/demo2/main2.cpp demos/demo2/mainwindow2.cpp ${QDMDILIB_COMMON_SOURCES})
set(QMDILIB_DEMO2_HEADERS demos/demo2/mainwindow2.h)
QT4_WRAP_CPP(QMDILIB_DEMO2_MOC ${QMDILIB_DEMO2_HEADERS} ${QDMDILIB_COMMON_HEADERS} )
add_executable(demo2 ${QMDILIB_DEMO2_SOURCES} ${QMDILIB_DEMO2_MOC} ${QDMDILIB_COMMON_RC_SRCS})
target_link_libraries(demo2 ${QT_LIBRARIES} QMDILIB_STATIC)

set(QMDILIB_DEMO3_SOURCES demos/demo3/main3.cpp demos/demo3/mainwindow3.cpp ${QDMDILIB_COMMON_SOURCES})
set(QMDILIB_DEMO3_HEADERS demos/demo3/mainwindow3.h)
QT4_WRAP_CPP(QMDILIB_DEMO3_MOC ${QMDILIB_DEMO2_HEADERS} ${QDMDILIB_COMMON_HEADERS} )
add_executable(demo3 ${QMDILIB_DEMO2_SOURCES} ${QMDILIB_DEMO2_MOC} ${QDMDILIB_COMMON_RC_SRCS})
target_link_libraries(demo3 ${QT_LIBRARIES} QMDILIB_STATIC)
