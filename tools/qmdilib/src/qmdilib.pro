# $Id: demo1.pro 123 2006-10-07 20:28:22Z elcuco $

QMDILIB_SRC_DIR  = .
include (qmdilib.pri)

DESTDIR  = ../
TEMPLATE = lib
CONFIG  += silent 
TARGET   = qmdi
VERSION  = 0.0.5
