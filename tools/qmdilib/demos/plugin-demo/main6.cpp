#include <QApplication>
#include "mainwindow6.h"

/**
 * \file main6.cpp
 * \brief Entry point of the 6st demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	MainWindow6 w;
	w.show();
	return app.exec();
}

