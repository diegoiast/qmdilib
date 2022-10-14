cmake_minimum_required(VERSION 3.20)

project (qmdilib) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QMDILIB_VERSION "0.1.0")
set(QMDILIB_SOVERSION "0")

option(QMDILIB_BUILD_EXAMPLES "Build examples for qmdilib" ON)

find_package(Qt6 COMPONENTS Widgets PrintSupport Core5Compat)

add_library(qmdilib 
	src/qmdiactiongroup.h
	src/qmdiactiongroup.cpp
	src/qmdiactiongrouplist.h
	src/qmdiactiongrouplist.cpp
	src/qmdiclient.h
	src/qmdiclient.cpp
	src/qmdihost.h
	src/qmdihost.cpp
	src/qmdiserver.h
	src/qmdiserver.cpp
	src/qmditabwidget.h
	src/qmditabwidget.cpp
)
target_link_libraries(qmdilib Qt::Widgets)
target_include_directories(qmdilib PUBLIC src)

set_target_properties(qmdilib
    PROPERTIES
      VERSION     ${QMDILIB_VERSION}
      SOVERSION   ${QMDILIB_SOVERSION}
)

if (QMDILIB_BUILD_EXAMPLES)

add_executable(demo1 demos/demo1/main.cpp demos/demo1/mainwindow.cpp)
target_link_libraries(demo1 qmdilib)


# set(QMDILIB_DEMO2_SOURCES demos/demo2/main2.cpp demos/demo2/mainwindow2.cpp ${QDMDILIB_COMMON_SOURCES})
# set(QMDILIB_DEMO2_HEADERS demos/demo2/mainwindow2.h)
# QT4_WRAP_CPP(QMDILIB_DEMO2_MOC ${QMDILIB_DEMO2_HEADERS} ${QDMDILIB_COMMON_HEADERS} )
add_executable(demo2 
	demos/demo2/main2.cpp demos/demo2/mainwindow2.cpp
	demos/common/helpbrowse.cpp demos/common/qexeditor.cpp
	demos/common/common.qrc
)
target_link_libraries(demo2 qmdilib)
target_include_directories(demo2 PRIVATE demos/common)

endif()
