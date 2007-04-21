#include <QtGui>
#include <QIcon>
#include <QLabel>
#include <QListView>

#include "configdialog.h"
#include "pluginmanager.h"
#include "pluginmodel.h"

ConfigDialog::ConfigDialog( QWidget *owner ):
	QDialog(owner)
{
	setSizeGripEnabled(true);
	
	pluginManager = NULL;
	
	
	contentsWidget = new QListView;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
 	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing( 0 );
	contentsWidget->setModelColumn( 0 );
// 	contentsWidget->setSizePolicy( )
	contentsWidget->setFlow( QListView::TopToBottom );
	
	
	pagesWidget = new QStackedWidget(this);
	applyButton = new QPushButton(tr("&Apply"), this);
	closeButton = new QPushButton(tr("&Close"), this);

	QHBoxLayout	*mainLayout	= new QHBoxLayout(this);
	QVBoxLayout	*vLayout	= new QVBoxLayout;
	QHBoxLayout	*buttonsLayout	= new QHBoxLayout;
	QFrame		*line1		= new QFrame;
	QWidget		*w		= new QWidget;
	
	plugin_list_ui.setupUi( w );
	pagesWidget->addWidget( w );

	mainLayout->setObjectName("mainLayout");
	vLayout->setObjectName("vLayout");
	buttonsLayout->setObjectName("buttonsLayout");
	line1->setObjectName("line1");
	line1->setFrameShape(QFrame::HLine);
	
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(applyButton);
	buttonsLayout->addWidget(closeButton);

	vLayout->addWidget( pagesWidget );
	vLayout->addWidget( line1 );
	vLayout->addLayout( buttonsLayout );

	mainLayout->addWidget(contentsWidget);
	mainLayout->addLayout(vLayout);
	setLayout(mainLayout);
	
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
/*	connect(contentsWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));*/
	
	setWindowTitle(tr("Config Dialog"));
}

ConfigDialog::~ConfigDialog()
{
// 	contentsWidget->clear();

	// objects are not deleted by the GUI, but the plugins
	// without this code, glibc will complain about deleting the same 
	// object twise: the dangling pointer problem
	for( int i=pagesWidget->count(); i!=0; i--)
		pagesWidget->removeWidget( pagesWidget->currentWidget() );
}

void ConfigDialog::setManager( PluginManager *manager )
{
// 	qDebug("ConfigDialog::setManager - manager = %p", manager );
	pluginManager = manager;
	pluginModel = new PluginModel( pluginManager );
	plugin_list_ui.listWidget->setModel( pluginModel );
// 	contentsWidget->setModel( pluginModel );
}

void ConfigDialog::applyChanges()
{
}
