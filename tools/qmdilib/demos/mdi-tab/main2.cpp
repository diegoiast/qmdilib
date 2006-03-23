#include <QApplication>
#include "mainwindow2.h"

/**
 * \file main.c
 * \brief Entry point of this demo
 * \author Diego Iastrubni (elcuco@kde.org)
 * License GPL
 */
 
int main( int argc, char *argv[] )
{
        QApplication app( argc, argv );
        MainWindow2 w;
        w.show();
        return app.exec();
}
