/**
 * \file main2.cpp
 * \brief Entry point of 2nd demo
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License GPL 2 or 3
 */
 
// $Id$ 
 
#include <QApplication>
#include "mainwindow2.h"

int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow2 w;
        w.show();
        return app.exec();
}
