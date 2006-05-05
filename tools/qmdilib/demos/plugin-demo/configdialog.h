#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QIcon>

class QIcon;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class ConfigDialog : public QDialog
{
	Q_OBJECT

	friend class PluginManager;

public:
	ConfigDialog();
	~ConfigDialog();

public slots:
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	void addPage( QWidget *w, QIcon i );
	void removePage( QWidget *w );
	void applyChanges();

private:
	QListWidget	*contentsWidget;
	QStackedWidget	*pagesWidget;
	QPushButton	*applyButton;
	QPushButton	*closeButton;
// 	QLabel		*currentPageLabel;
};

#endif
