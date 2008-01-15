/**
 * \file main2.cpp
 * \brief Entry point of 2nd demo
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License GPL
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
