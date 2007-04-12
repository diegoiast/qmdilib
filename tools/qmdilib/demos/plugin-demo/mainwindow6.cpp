#include <QTimer>
#include <QAction>
#include <QStatusBar>
#include <QToolButton>


#define SINGLE_TOOLBAR !false

#include "mainwindow6.h"
#include "pluginmanager.h"
#include "qmditabwidget.h"

// plugins
#include "plugins/editor/editor.h"
#include "plugins/help/help.h"
#include "plugins/richtext/richtext_plg.h"
#include "plugins/richtext/richtext.h"


MainWindow6::MainWindow6(QWidget *owner):
	QMainWindow( owner )
{
	QTimer::singleShot( 0, this, SLOT(initGUI()));
}

MainWindow6::~MainWindow6()
{
	delete pluginManager;
}

void MainWindow6::initGUI()
{
	actionConfig	= new QAction( "&Config", this );
	actionQuit	= new QAction( "Ex&it", this );

	menus[tr("&File")]->setMergePoint();
	menus[tr("&File")]->addSeparator();
	menus[tr("&File")]->addAction(actionQuit);
	menus[tr("&Edit")];
	menus[tr("&Search")];
	menus[tr("&Navigation")];
	menus[tr("Se&ttings")]->addAction( actionConfig );
	menus[tr("&Help")];

	toolbars["Main"]->addAction(actionConfig);
	
	tabWidget = new qmdiTabWidget(this);
	pluginManager = new PluginManager( this, dynamic_cast<qmdiServer*>(tabWidget) );
	pluginManager->addPlugin( new HelpPlugin );
	pluginManager->addPlugin( new EditorPlugin );
	pluginManager->addPlugin( new RichTextPlugin );
	updateGUI( this );
	
	
	QToolButton *tabCloseBtn = new QToolButton(tabWidget);
	tabCloseBtn->setAutoRaise( true );
	connect( tabCloseBtn, SIGNAL(clicked()), this, SLOT(closeClient()));
	tabCloseBtn->setIcon(QIcon(":images/closetab.png"));
	tabWidget->setCornerWidget( tabCloseBtn, Qt::TopRightCorner  );
	setCentralWidget( tabWidget );
// 	connect( actionConfig, SIGNAL(triggered()), pluginManager, SLOT(configurePlugins()));
	connect( actionQuit, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
	
	statusBar()->showMessage("Welcome - feel free to configure the GUI to your needs",5000);
}

void MainWindow6::closeClient()
{
	qmdiClient *client = dynamic_cast<qmdiClient*>( tabWidget->currentWidget() );

	if (client == NULL){
// 		qDebug( "not mdi client  %s", qPrintable(tabWidget->currentWidget()->objectName()) );
		qDebug( "not mdi client  %s", qPrintable(tabWidget->currentWidget()->objectName()) );
		tabWidget->currentWidget()->deleteLater();
	}
	else {
		qDebug( "this is an mdi client  %s", qPrintable(client->name) );
		client->closeClient();
	}
}

void MainWindow6::on_actionQuit_triggered()
{
	this->close();
}
