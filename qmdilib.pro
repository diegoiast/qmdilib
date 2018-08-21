# $Id$

TEMPLATE	= subdirs
CONFIG		+= ordered
SUBDIRS 	= src/qmdilib.pro src/qmdilib-static.pro demos/demo2 demos/demo1 demos/plugin-demo

lessThan(QT_MAJOR_VERSION, 5): SUBDIRS += demos/demo3

# TODO
# demo3 is not ready yet

