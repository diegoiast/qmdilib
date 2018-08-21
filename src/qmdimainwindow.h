#include "qmdihost.h"
#include <QMainWindow>

// $Id$

class QWidget;

class qmdiMainWindow: public QMainWindow, public qmdiHost
{
public:	
	qmdiMainWindow( QWidget * parent = 0, Qt::WindowFlags flags = 0 ):
		QMainWindow( parent, flags )
		{
			// stub function
		}
};
