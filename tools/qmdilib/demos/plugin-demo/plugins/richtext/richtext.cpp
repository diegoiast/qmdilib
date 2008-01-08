/**
 * \file richtext.cpp
 * \brief Implementation of the RichText class
 * \author Diego Iastrubni (elcuco@kde.org)
 *  License LGPL
 * \see RichText
 */

// $Id: pluginmanager.h 146 2007-04-23 22:45:01Z elcuco $

#include <QApplication>
#include <QLayout>
#include <QAction>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTabWidget>
#include <QTextCharFormat>
#include <QKeySequence>
#include <QActionGroup>
#include <QTextList>
#include <QFile>
#include <QTextStream>
#include <QColorDialog>
#include <QPixmap>
#include <QFontComboBox>
#include <QComboBox>

#include "richtext.h"

RichText::RichText( QWidget *parent, QString fileName ):
	QWidget(parent)
{
	initWidget( fileName, NULL );
}

RichText::RichText( QWidget *parent, QTextEdit *e ):
	QWidget(parent)
{
	initWidget( "", e );
}

RichText::RichText( QWidget *parent, QString fileName, QTextEdit *e )
	: QWidget( parent )
{
	initWidget( fileName, e );
}

RichText::~RichText()
{
	delete richText;
	delete textEdit;
	delete tabWidget;
}

void RichText::initWidget( QString fileName, QTextEdit *e )
{
	this->setObjectName( "RichText (container)" );
	tabWidget = new QTabWidget( this );
	tabWidget->setObjectName( "RichText::tabWidget" );

	// create the basic GUI
	richText = new QTextEdit( tabWidget );
	richText->setAcceptRichText( true );
	richText->setFrameStyle( QFrame::NoFrame );
	richText->setObjectName( "RichText::richText" );
	
	textEdit = (e == NULL) ? new QTextEdit( tabWidget ): e;
	textEdit->setAcceptRichText( false );
	textEdit->setFrameStyle( QFrame::NoFrame );
	textEdit->setObjectName( "RichText::textEdit" );
	
	tabWidget->setTabShape( QTabWidget::Triangular );
	tabWidget->setTabPosition( QTabWidget::South);
	tabWidget->addTab( richText, tr("Rich text") );
	tabWidget->addTab( textEdit, tr("HTML") );

	QLayout *myLayout = new QVBoxLayout( this );
	myLayout->addWidget( tabWidget );
	myLayout->setMargin( 0 );
	myLayout->setSpacing( 0 );
	myLayout->setObjectName( "RichText::myLayout" );
	setLayout( myLayout );

	// create the actions
	actionBold	 	= new QAction( "B", this );
	actionItalic	 	= new QAction( "I", this );
	actionUnderline	 	= new QAction( "U", this );
	
	alignGroup		= new QActionGroup(this);
	actionAlignLeft		= new QAction( "L", alignGroup );
	actionAlignCenter	= new QAction( "C", alignGroup );
	actionAlignJustify	= new QAction( "J", alignGroup );
	actionAlignRight	= new QAction( "R", alignGroup );
	
	listGroup		= new QActionGroup(this);
	actionListDisc		= new QAction( "*)", listGroup );
	actionListCircle	= new QAction( "()", listGroup );
	actionListSquare	= new QAction( "[]", listGroup );
	actionListDecmial	= new QAction( "1)", listGroup );
	actionListLowerAlpha	= new QAction( "a)", listGroup );
	actionListUpperAlpha	= new QAction( "A)", listGroup );
	
	QPixmap pix(16, 16);
	pix.fill(Qt::black);
	actionTextColor		= new QAction(pix, tr("&Color..."), this);
	connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
	
	fontComboBox = new QFontComboBox;
	fontComboBox->setObjectName("Choose font family");
	
	comboSize = new QComboBox;
	QFontDatabase db;
	comboSize->setObjectName("Choose font size");
	comboSize->setEditable(true);
	foreach(int size, db.standardSizes())
		comboSize->addItem(QString::number(size));

	
	actionBold->setCheckable( true );
	actionItalic->setCheckable( true );
	actionUnderline->setCheckable( true );
	actionAlignRight->setCheckable( true );
	actionAlignLeft->setCheckable( true );
	actionAlignCenter->setCheckable( true );
	actionAlignJustify->setCheckable( true );

	actionListDisc->setCheckable( true );
	actionListCircle->setCheckable( true );
	actionListSquare->setCheckable( true );
	actionListDecmial->setCheckable( true );
	actionListLowerAlpha->setCheckable( true );
	actionListUpperAlpha->setCheckable( true );
	
	connect( tabWidget	, SIGNAL(currentChanged(int)), this, SLOT(on_tabWidget_currentChanged(int)));
	connect( richText	, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));
	connect( richText	, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
	connect( richText	, SIGNAL(textChanged()), this, SLOT(on_richText_Modified()));
	
	connect( actionBold	, SIGNAL(triggered()), this, SLOT(markBold()));
	connect( actionItalic	, SIGNAL(triggered()), this, SLOT(markItalic()));
	connect( actionUnderline, SIGNAL(triggered()), this, SLOT(markUnderline()));
	connect( alignGroup	, SIGNAL(triggered(QAction *)), this, SLOT(textAlign(QAction *)));
	connect( listGroup	, SIGNAL(triggered(QAction *)), this, SLOT(setList_(QAction *)));
	connect( fontComboBox	, SIGNAL(activated(const QString &)), this, SLOT(textFamily(const QString &)));
	connect( comboSize	, SIGNAL(activated(const QString &)), this, SLOT(textSize(const QString &)));

	setMSWordShortCuts();
	
	if (!fileName.isEmpty())
		loadFile( fileName );
}

void RichText::loadFile( QString fileName )
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QString content;
	QTextStream in(&file);
	while (!in.atEnd())
		content += in.readLine() + "\n";
	
	textEdit->setPlainText( content );
	richText->setHtml( content );
	dirtyFlag = false;
}

void RichText::setMSWordShortCuts()
{
	actionBold->setShortcut( QKeySequence("Ctrl+B") );
	actionItalic->setShortcut( QKeySequence("Ctrl+I") );
	actionUnderline->setShortcut( QKeySequence("Ctrl+U") );

	actionAlignRight->setShortcut( QKeySequence("Ctrl+R") );
	actionAlignLeft->setShortcut( QKeySequence("Ctrl+L") );
	actionAlignCenter->setShortcut( QKeySequence("Ctrl+E") );
	actionAlignJustify->setShortcut( QKeySequence() );
}

void RichText::markBold()
{
	QTextCharFormat fmt;
	fmt.setFontWeight( actionBold->isChecked() ? QFont::Bold : QFont::Normal );
	mergeFormatOnWordOrSelection( fmt );
}

void RichText::markItalic()
{
	QTextCharFormat fmt;
	fmt.setFontItalic( actionItalic->isChecked() );
	mergeFormatOnWordOrSelection( fmt );
}

void RichText::markUnderline()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline( actionUnderline->isChecked() );
	mergeFormatOnWordOrSelection( fmt );
}

void RichText::setList( QTextListFormat::Style type )
{
	QTextCursor cursor = richText->textCursor();

	if (type != 0) {
		cursor.beginEditBlock();
		QTextBlockFormat blockFmt = cursor.blockFormat();
		QTextListFormat listFmt;

		if (cursor.currentList()) {
			listFmt = cursor.currentList()->format();
		} else {
			listFmt.setIndent(blockFmt.indent() + 1);
			blockFmt.setIndent(0);
			cursor.setBlockFormat(blockFmt);
		}

		listFmt.setStyle(type);
		cursor.createList(listFmt);
		cursor.endEditBlock();
	} else {
		QTextBlockFormat bfmt;
		bfmt.setObjectIndex(-1);
		cursor.mergeBlockFormat(bfmt);
	}
}

QTextList* RichText::getList()
{
	QTextCursor cursor = richText->textCursor();
	
	return cursor.currentList();
}

void RichText::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = richText->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	richText->mergeCurrentCharFormat(format);
}


void RichText::cursorPositionChanged()
{
	alignmentChanged( richText->alignment() );
	
	QTextList *l = getList();
	if (l == NULL)
	{
		actionListDisc->setChecked( false ); 
		actionListCircle->setChecked( false ); 
		actionListSquare->setChecked( false );
		actionListDecmial->setChecked( false ); 
		actionListLowerAlpha->setChecked( false );
		actionListUpperAlpha->setChecked( false );
	}
	else
	{
		QTextListFormat::Style style = l->format().style();
		
		if (style ==  QTextListFormat::ListDisc)
			actionListDisc->setChecked( true ); else
		if (style ==  QTextListFormat::ListCircle)
			actionListCircle->setChecked( true ); else
		if (style ==  QTextListFormat::ListSquare)
			actionListSquare->setChecked( true ); else
		if (style ==  QTextListFormat::ListDecimal) 
			actionListDecmial->setChecked( true ); else
		if (style ==  QTextListFormat::ListLowerAlpha)
			actionListLowerAlpha->setChecked( true ); else
		if (style ==  QTextListFormat::ListUpperAlpha) 
			actionListUpperAlpha->setChecked( true );
		else
		{
			actionListDisc->setChecked( false ); 
			actionListCircle->setChecked( false ); 
			actionListSquare->setChecked( false );
			actionListDecmial->setChecked( false ); 
			actionListLowerAlpha->setChecked( false );
			actionListUpperAlpha->setChecked( false );
		}
	}
}

void RichText::on_tabWidget_currentChanged ( int index )
{
	if (index == 1)
	{
		// moved from the richText to the textEdit
		if (dirtyFlag)
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			// QTextEdit just messes up the makrup:
			// if the user is editing the raw html, and moves to the
			// html tab to preview, dont rewrite the html code
			textEdit->setPlainText( richText->toHtml() );
			QApplication::restoreOverrideCursor();
		}
		else
		{
			// stub block
		}
	}
	else
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		// moved from the textEdit to the richText
		richText->setHtml( textEdit->toPlainText() );
		QApplication::restoreOverrideCursor();
	}
	
	dirtyFlag = false;

	actionBold->setEnabled( index == 0 );
	actionItalic->setEnabled( index == 0 );
	actionUnderline->setEnabled( index == 0 );
	listGroup->setEnabled( index == 0 );
	alignGroup->setEnabled( index == 0 );
	fontComboBox->setEnabled( index == 0 );
	comboSize->setEnabled( index == 0 );
	actionTextColor->setEnabled( index == 0 );
}

void RichText::on_richText_Modified()
{
	dirtyFlag = true;
}

void RichText::alignmentChanged(Qt::Alignment a)
{
	if (a & Qt::AlignLeft) {
		actionAlignLeft->setChecked(true);
	} else if (a & Qt::AlignHCenter) {
		actionAlignCenter->setChecked(true);
	} else if (a & Qt::AlignRight) {
		actionAlignRight->setChecked(true);
	} else if (a & Qt::AlignJustify) {
		actionAlignJustify->setChecked(true);
	}
}

void RichText::setShowSource( bool shouldShow )
{
// 	tabWidget->tabBar()->setVisible( shouldShow );
 	if (!shouldShow)
 		tabWidget->setCurrentIndex( 0 );
}

bool RichText::getShowSource()
{
	// TODO
	return false;
}


void RichText::currentCharFormatChanged(const QTextCharFormat &format)
{
	fontChanged( format.font() );
	colorChanged( format.foreground().color() );
}

 void RichText::textAlign(QAction *a)
{
	if (a == actionAlignLeft)
		richText->setAlignment(Qt::AlignLeft);
	else if (a == actionAlignCenter)
		richText->setAlignment(Qt::AlignHCenter);
	else if (a == actionAlignRight)
		richText->setAlignment(Qt::AlignRight);
	else if (a == actionAlignJustify)
		richText->setAlignment(Qt::AlignJustify);
}

void RichText::setList_(QAction *a)
{
	if (a == actionListDisc)
		setList( QTextListFormat::ListDisc); else
	if (a == actionListCircle)
		setList( QTextListFormat::ListCircle); else
	if (a == actionListSquare)
		setList( QTextListFormat::ListSquare); else
	if (a == actionListDecmial)
		setList( QTextListFormat::ListDecimal); else
	if (a == actionListLowerAlpha)
		setList( QTextListFormat::ListLowerAlpha); else
	if (a == actionListUpperAlpha)
		setList( QTextListFormat::ListUpperAlpha); 
}

void RichText::fontChanged(const QFont &f)
{
	fontComboBox->setCurrentIndex(fontComboBox->findText(QFontInfo(f).family()));
	comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
	actionBold->setChecked(f.bold());
	actionItalic->setChecked(f.italic());
	actionUnderline->setChecked(f.underline());
}

void RichText::colorChanged(const QColor &c)
{
	QPixmap pix(16, 16);
	pix.fill(c);
	actionTextColor->setIcon(pix);
}

void RichText::textColor()
{
	QColor col = QColorDialog::getColor(textEdit->textColor(), this);
	if (!col.isValid())
		return;
	
	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void RichText::textSize( QString p )
{
	QTextCharFormat fmt;
	fmt.setFontPointSize(p.toFloat());
	mergeFormatOnWordOrSelection(fmt);
}

void RichText::textFamily(const QString &f)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(f);
	mergeFormatOnWordOrSelection(fmt);
}

/*virtual */
/*
bool RichText::event ( QEvent * event )
{
	if (event->type() == QEvent::KeyPress)
	{
		setCursor(Qt::IBeamCursor);
	}
	else if (event->type() == QEvent::MouseButtonPress)
	{
		setCursor(Qt::IBeamCursor);
	}
	return false;
}
*/
void RichText::timerEvent ( QTimerEvent *event )
{
	Q_UNUSED( event );
}
