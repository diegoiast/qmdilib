#include <QTimer>
#include <QAction>
#include <QStatusBar>
#include <QToolButton>

#include "mainwindow3.h"
#include "pluningmanager.h"
#include "qmditabwidget.h"

// plugins
#include "editor.h"
#include "help.h"


MainWindow3::MainWindow3(QWidget *owner):
	QMainWindow( owner )
{
	QTimer::singleShot( 0, this, SLOT(initGUI()));
}

MainWindow3::~MainWindow3()
{
	delete pluginManager;
}

void MainWindow3::initGUI()
{
	tabWidget = new qmdiTabWidget(this);
	
	QToolButton *tabCloseBtn = new QToolButton(tabWidget);
	tabCloseBtn->setAutoRaise( true );
	connect( tabCloseBtn, SIGNAL(clicked()), this, SLOT(closeClient()));
	tabCloseBtn->setIcon(QIcon(":images/closetab.png"));
	tabWidget->setCornerWidget( tabCloseBtn, Qt::TopRightCorner  );
	setCentralWidget( tabWidget );

	actionConfig = new QAction( "&Config", this );

	menus[tr("&File")];
	menus[tr("&Edit")];
	menus[tr("&Search")];
	menus[tr("&Navigation")];
	menus[tr("Se&ttings")]->addAction( actionConfig );
	menus[tr("&Help")];

	toolbars["Settings"]->addAction( actionConfig );
	
	pluginManager = new PluginManager( this, dynamic_cast<qmdiServer*>(tabWidget) );
	pluginManager->addPlugin( new HelpPlugin );
	pluginManager->addPlugin( new EditorPlugin );
	updateGUI( this );
	
	connect( actionConfig, SIGNAL(triggered()), pluginManager, SLOT(configurePlugins()));
	statusBar()->showMessage("Welcome - feel free to configure the GUI to your needs",5000);
}

void MainWindow3::closeClient()
{
	qmdiClient *client = dynamic_cast<qmdiClient*>( tabWidget->currentWidget());

	if (client == NULL)
		tabWidget->currentWidget()->deleteLater();

	client->closeClient();
}
