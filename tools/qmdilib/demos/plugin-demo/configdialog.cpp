#include <QtGui>
#include <QIcon>
#include <QLabel>

#include "configdialog.h"

ConfigDialog::ConfigDialog()
{
	setSizeGripEnabled(true);
	
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing( 0 );
	contentsWidget->setModelColumn( 0 );
	contentsWidget->setFlow( QListView::TopToBottom );
	
	pagesWidget = new QStackedWidget(this);
	applyButton = new QPushButton(tr("&Apply"));
	closeButton = new QPushButton(tr("&Close"));

	QHBoxLayout *mainLayout		= new QHBoxLayout(this);
	QVBoxLayout *vLayout		= new QVBoxLayout;
	QHBoxLayout *buttonsLayout	= new QHBoxLayout;

	vLayout->setObjectName("vLayout");
	buttonsLayout->setObjectName("buttonsLayout");
	mainLayout->setObjectName("mainLayout");
	
// 	QFrame *line1 = new QFrame;
	QFrame *line2 = new QFrame;
// 	currentPageLabel = new QLabel;
	
// 	line1->setFrameShape(QFrame::HLine);
	line2->setFrameShape(QFrame::HLine);
	
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(applyButton);
	buttonsLayout->addWidget(closeButton);

// 	vLayout->addWidget( currentPageLabel );
// 	vLayout->addWidget( line1 );
	vLayout->addWidget( pagesWidget );
	vLayout->addWidget( line2 );
	vLayout->addLayout( buttonsLayout );
	
	mainLayout->addWidget(contentsWidget);
	mainLayout->addLayout(vLayout);
	setLayout(mainLayout);
	
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(contentsWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
	
	setWindowTitle(tr("Config Dialog"));
}

ConfigDialog::~ConfigDialog()
{
	contentsWidget->clear();

	// objects are not deleted by the GUI, but the plugins
	// without this code, glibc will complain about deleting the same 
	// object twise: the dangling pointer problem
	for( int i=pagesWidget->count(); i!=0; i--)
		pagesWidget->removeWidget( pagesWidget->currentWidget() );
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
        	current = previous;

	int newRow = contentsWidget->row(current);
	pagesWidget->setCurrentIndex( newRow );
// 	QListWidgetItem *w = contentsWidget->item(newRow);
// 	currentPageLabel->setText( w->text() );
}

void ConfigDialog::addPage( QWidget *w, QIcon i )
{
	QListWidgetItem *item = new QListWidgetItem;
	item->setIcon( i );
	item->setText( w->objectName() );
	item->setTextAlignment( Qt::AlignHCenter );
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	
	// QListWidget
	contentsWidget->insertItem( contentsWidget->count(), item );

	//QStackedWidget
	pagesWidget->addWidget( w );
}

void ConfigDialog::removePage( QWidget *w )
{
	int k = pagesWidget->indexOf ( w );
	contentsWidget->takeItem( k );
	
	//QStackedWidget 
	pagesWidget->removeWidget( w );
}


void ConfigDialog::applyChanges()
{
}
