#include <QApplication>
#include "mainwindow.h"

/**
 * \file main.cpp
 * \brief Entry point of first demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */
 
int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow w;
        w.show();
        return app.exec();
}
