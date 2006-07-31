#include <QApplication>
#include "mainwindow5.h"

/**
 * \file main5.cpp
 * \brief Entry point of 5th demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */
 
int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow5 w;
        w.show();
        return app.exec();
}
