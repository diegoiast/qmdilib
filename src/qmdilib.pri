QT += widgets
SOURCES += \
	$$QMDILIB_SRC_DIR/qmdiactiongroup.cpp \
	$$QMDILIB_SRC_DIR/qmdiactiongrouplist.cpp \
	$$QMDILIB_SRC_DIR/qmdiclient.cpp \
	$$QMDILIB_SRC_DIR/qmdihost.cpp \
	$$QMDILIB_SRC_DIR/qmdiserver.cpp \
	$$QMDILIB_SRC_DIR/qmditabwidget.cpp

HEADERS += \
	$$QMDILIB_SRC_DIR/actiongroup.h \
	$$QMDILIB_SRC_DIR/actiongrouplist.h \
	$$QMDILIB_SRC_DIR/qmdiactiongroup.h \
	$$QMDILIB_SRC_DIR/qmdiactiongrouplist.h \
	$$QMDILIB_SRC_DIR/qmdiclient.h \
	$$QMDILIB_SRC_DIR/qmdihost.h \
	$$QMDILIB_SRC_DIR/qmdimainwindow.h \
	$$QMDILIB_SRC_DIR/qmdiserver.h \
	$$QMDILIB_SRC_DIR/qmditabwidget.h

lessThan(QT_MAJOR_VERSION, 5): SOURCES += $$QMDILIB_SRC_DIR/qmdiworkspace.cpp
lessThan(QT_MAJOR_VERSION, 5): HEADERS += $$QMDILIB_SRC_DIR/qmdiworkspace.h
