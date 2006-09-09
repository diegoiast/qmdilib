/**
 * \file main6.cpp
 * \brief Entry point of 2nd demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */
 
// $Id$
 
#include <QApplication>
#include "mainwindow6.h"

int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow6 w;
        w.show();
        return app.exec();
}
