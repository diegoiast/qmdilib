/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <QIcon>

#include "configdialog.h"
// #include "pages.h"

ConfigDialog::ConfigDialog()
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
// 	contentsWidget->setSpacing(12);
// 	contentsWidget->setModelColumn( 0 );
	contentsWidget->setFlow( QListView::TopToBottom );

	
	pagesWidget = new QStackedWidget;
	QPushButton *closeButton = new QPushButton(tr("Close"));
	
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(contentsWidget);
	horizontalLayout->addWidget(pagesWidget, 1);
	
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
	
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(contentsWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
	
	setWindowTitle(tr("Config Dialog"));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
        	current = previous;

	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void ConfigDialog::addPage( QWidget *w, QIcon i )
{
	QListWidgetItem *item = new QListWidgetItem;
	item->setIcon( i );
	item->setText( w->objectName() );
	item->setTextAlignment( Qt::AlignHCenter );
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	
	contentsWidget->insertItem( 0, item );

	pagesWidget->addWidget( w );
}
