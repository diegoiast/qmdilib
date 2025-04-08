cmake_minimum_required(VERSION 3.20)
project(qmdilib VERSION 0.1.1 LANGUAGES CXX)


option(QMDILIB_BUILD_EXAMPLES "Build examples for qmdilib" ON)
option(QMDILIB_USE_MOLD "Link using mold, if available" ON)
option(QMDILIB_TESTS "Build and run tests for qmdilib" ON)

include(FetchContent)
include(cmake/CPM.cmake)
include(cmake/mold-linker.cmake)

cpmaddpackage(
    NAME     Format.cmake
    VERSION     1.8.2
    GITHUB_REPOSITORY
    TheLartians/Format.cmake
    OPTIONS # set to yes skip cmake formatting
    "FORMAT_SKIP_CMAKE NO"
    # path to exclude (optional, supports regular expressions)
    "CMAKE_FORMAT_EXCLUDE cmake/*"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QMDILIB_VERSION "0.1.0")
set(QMDILIB_SOVERSION "0")

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Test)

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

    src/qmdiconfigdialog.cpp
    src/qmdiconfigdialog.h
    src/qmdipluginconfig.cpp
    src/qmdipluginconfig.h
    src/qmdiglobalconfig.cpp
    src/qmdiglobalconfig.h
    src/qmdiconfigwidgetfactory.cpp
    src/qmdiconfigwidgetfactory.h
    src/stringlistwidget.cpp
    src/stringlistwidget.h
)

target_link_libraries(qmdilib Qt::Widgets)
target_include_directories(qmdilib PUBLIC src)
set_property(TARGET qmdilib PROPERTY AUTOMOC ON)

set_target_properties(qmdilib PROPERTIES
    VERSION     ${QMDILIB_VERSION}
    SOVERSION   ${QMDILIB_SOVERSION}
)

if (QMDILIB_BUILD_EXAMPLES)

add_executable(demo1 demos/demo1/main.cpp demos/demo1/mainwindow.cpp)
set_property(TARGET demo1 PROPERTY AUTOMOC ON)
target_link_libraries(demo1 qmdilib)

add_executable(demo2 
    demos/demo2/main2.cpp
    demos/demo2/mainwindow2.cpp
    demos/common/helpbrowse.cpp
    demos/common/qexeditor.cpp
    demos/common/common.qrc
)
set_property(TARGET demo2 PROPERTY AUTOMOC ON)
target_link_libraries(demo2 qmdilib)
target_include_directories(demo2 PRIVATE demos/common)


add_executable(demo3
    demos/demo3/main3.cpp
)
set_property(TARGET demo3 PROPERTY AUTOMOC ON)
target_link_libraries(demo3 qmdilib)

add_executable(pluginmanager
    demos/common/qexeditor.cpp
    demos/common/qexeditor.h
    demos/common/helpbrowse.cpp
    demos/common/helpbrowse.h

    demos/plugin-demo/iplugin.cpp
    demos/plugin-demo/iplugin.h
    demos/plugin-demo/main6.cpp
    demos/plugin-demo/pluginmanager.cpp
    demos/plugin-demo/plugin_list.ui
    demos/plugin-demo/richtext.qrc
    demos/plugin-demo/pluginmanager.cpp
    demos/plugin-demo/pluginmodel.cpp
    demos/plugin-demo/pluginmodel.h
    demos/plugin-demo/plugins/editor/editor_plg.cpp
    demos/plugin-demo/plugins/editor/editor_plg.h
    demos/plugin-demo/plugins/editor/qexeditor2.cpp
    demos/plugin-demo/plugins/editor/qexeditor2.h
    demos/plugin-demo/plugins/filesystem/filesystembrowser.cpp
    demos/plugin-demo/plugins/filesystem/filesystembrowser.h
    demos/plugin-demo/plugins/filesystem/filesystemwidget.cpp
    demos/plugin-demo/plugins/filesystem/filesystemwidget.h
    demos/plugin-demo/plugins/help/help_plg.cpp
    demos/plugin-demo/plugins/help/help_plg.h
    demos/plugin-demo/plugins/richtext/richtextwidget.cpp
    demos/plugin-demo/plugins/richtext/richtextwidget.h
    demos/plugin-demo/plugins/richtext/richtextclient.cpp
    demos/plugin-demo/plugins/richtext/richtext_plg.cpp
    demos/plugin-demo/plugins/richtext/richtext_plg.h
    demos/plugin-demo/plugins/richtext/richtextclient.h
)
set_property(TARGET pluginmanager PROPERTY AUTOMOC ON)
set_property(TARGET pluginmanager PROPERTY AUTOUIC ON)
set_property(TARGET pluginmanager PROPERTY AUTORCC ON)
target_include_directories(pluginmanager PRIVATE demos/common demos/plugin-demo/)
target_link_libraries(pluginmanager qmdilib)
endif()

if (QMDILIB_TESTS)
enable_testing()

add_executable(configItemTests tests/configItemTests.cpp)
target_link_libraries(configItemTests qmdilib Qt6::Test)
set_property(TARGET configItemTests PROPERTY AUTOMOC ON)
add_test(NAME qmdiConfigItemTest COMMAND configItemTests)

add_executable(pluginConfigTests tests/pluginConfigTests.cpp)
target_link_libraries(pluginConfigTests qmdilib Qt6::Test)
set_property(TARGET pluginConfigTests PROPERTY AUTOMOC ON)
add_test(NAME qmdiPluginConfigTests COMMAND pluginConfigTests)

add_executable(globalConfigTests tests/globalConfigTest.cpp)
target_link_libraries(globalConfigTests qmdilib Qt6::Test)
set_property(TARGET globalConfigTests PROPERTY AUTOMOC ON)
add_test(NAME globalConfigTests COMMAND globalConfigTests)

endif()
