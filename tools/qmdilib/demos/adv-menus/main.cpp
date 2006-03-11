#include <QApplication>
#include "mainwindow.h"

/**
 * \file main.c
 * \brief Entry point of this demo
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
