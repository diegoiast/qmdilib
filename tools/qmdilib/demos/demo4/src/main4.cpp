/**
 * \file main4.cpp
 * \brief Entry point of 4th demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */
 
// $Id$ 

#include <QApplication>
#include "mainwindow4.h"
 
int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow4 w;
        w.show();
        return app.exec();
}
