/**
 * \file main3.cpp
 * \brief Entry point of 2nd demo
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License GPL 2 or 3
 */

// $Id$ 

#include <QApplication>
#include "mainwindow3.h"

int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow3 w;
        w.show();
        return app.exec();
}
