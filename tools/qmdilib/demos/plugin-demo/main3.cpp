#include <QApplication>
#include "mainwindow3.h"

/**
 * \file main3.c
 * \brief Entry point of this demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	MainWindow3 w;
	w.show();
	return app.exec();
}

