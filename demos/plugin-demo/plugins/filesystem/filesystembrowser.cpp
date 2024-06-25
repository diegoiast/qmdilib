/**
 * \file filesystembrowser.cpp
 * \brief Implementation of the configuration file system browser
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL 2 or 3
 * \see ConfigDialog
 */

#include "filesystembrowser.h"
#include "pluginmanager.h"
#include "qmdiserver.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>

#include <QStack>

class FileSystemWidget : public QWidget {
    Q_OBJECT
  public:
    FileSystemWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QString homePath = QDir::homePath();

        model = new QFileSystemModel(this);
        model->setRootPath(homePath);
        model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        model->setNameFilterDisables(false);

        backButton = new QPushButton(tr("Back"), this);
        connect(backButton, &QPushButton::clicked, this, &FileSystemWidget::navigateBack);

        homeButton = new QPushButton(tr("Home"), this);
        connect(homeButton, &QPushButton::clicked, this, &FileSystemWidget::navigateHome);

        upButton = new QPushButton(tr("Up"), this);
        connect(upButton, &QPushButton::clicked, this, &FileSystemWidget::navigateUp);

        nextButton = new QPushButton(tr("Next"), this);
        connect(nextButton, &QPushButton::clicked, this, &FileSystemWidget::navigateNext);

        toggleButton = new QPushButton(tr("Toggle View"), this);
        connect(toggleButton, &QPushButton::clicked, this, &FileSystemWidget::toggleView);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(backButton);
        buttonLayout->addWidget(nextButton);
        buttonLayout->addWidget(upButton);
        buttonLayout->addWidget(homeButton);
        buttonLayout->addWidget(toggleButton);
        buttonLayout->addStretch();

        treeView = new QTreeView(this);
        treeView->setModel(model);
        treeView->setRootIndex(model->index(homePath));
        treeView->expand(model->index(homePath));

        iconView = new QListView(this);
        iconView->setModel(model);
        iconView->setRootIndex(model->index(homePath));
        iconView->setViewMode(QListView::IconMode);
        iconView->setIconSize(QSize(64, 64));

        for (int i = 1; i < model->columnCount(); ++i) {
            if (i != 1) {
                treeView->hideColumn(i);
            }
        }
        // treeView->header()->setDefaultAlignment(Qt::AlignRight);
        // treeView->header()->setStretchLastSection(true);
        treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

        rootPathEdit = new QLineEdit(homePath, this);

        QCompleter *completer = new QCompleter(model, this);
        completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        rootPathEdit->setCompleter(completer);

        connect(rootPathEdit, &QLineEdit::returnPressed, this, &FileSystemWidget::onRootPathEdited);

        filterEdit = new QLineEdit("*.*", this);
        connect(filterEdit, &QLineEdit::returnPressed, this, &FileSystemWidget::onFilterChanged);

        showTreeView();

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addLayout(buttonLayout); // Add the button layout at the top
        layout->addWidget(rootPathEdit);
        layout->addWidget(treeView);
        layout->addWidget(iconView);
        layout->addWidget(filterEdit);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        setLayout(layout);
        setWindowTitle(tr("File System Viewer"));

        connect(treeView, &QTreeView::doubleClicked, this, &FileSystemWidget::onItemDoubleClicked);
        connect(iconView, &QListView::doubleClicked, this, &FileSystemWidget::onItemDoubleClicked);

        historyStack.push(homePath);
        currentHistoryIndex = 0;

        updateButtonStates();
    }

  signals:
    void fileDoubleClicked(const QString &filePath);

  private slots:
    void toggleView() {
        if (treeView->isVisible()) {
            showIconView();
        } else {
            showTreeView();
        }
    }

    void onItemDoubleClicked(const QModelIndex &index) {
        QFileInfo fileInfo = model->fileInfo(index);
        if (fileInfo.isDir()) {
            QString path = fileInfo.absoluteFilePath();
            navigateTo(path);
        } else {
            emit fileDoubleClicked(fileInfo.filePath());
        }
    }

    void navigateUp() {
        QDir currentDir = QDir::current();
        currentDir.cdUp();
        QString path = currentDir.absolutePath();
        navigateTo(path);
    }

    void navigateBack() {
        if (currentHistoryIndex > 0) {
            QString path = historyStack.at(--currentHistoryIndex);
            navigateTo(path);
        }
    }

    void navigateNext() {
        if (currentHistoryIndex < historyStack.size() - 1) {
            QString path = historyStack.at(++currentHistoryIndex);
            navigateTo(path);
        }
    }

    void navigateHome() {
        QString homePath = QDir::homePath();
        navigateTo(homePath);
    }

    void onRootPathEdited() {
        QString path = rootPathEdit->text();
        QFileInfo fileInfo(path);
        if (fileInfo.isFile()) {
            emit fileDoubleClicked(fileInfo.filePath());
        } else if (fileInfo.isDir()) {
            if (QDir(path).exists()) {
                navigateTo(path);
            }
        }
    }

    void onFilterChanged() {
        QString filterText = filterEdit->text().trimmed();
        QStringList filters = filterText.split(";", Qt::SkipEmptyParts);
        model->setNameFilters(filters);
    }

  private:
    QFileSystemModel *model;
    QTreeView *treeView;
    QListView *iconView;
    QPushButton *toggleButton;
    QPushButton *upButton;
    QLineEdit *rootPathEdit;
    QLineEdit *filterEdit;
    QPushButton *backButton;
    QPushButton *nextButton;
    QPushButton *homeButton;

    QStack<QString> historyStack;
    int currentHistoryIndex;

    void showTreeView() {
        treeView->show();
        iconView->hide();
    }

    void showIconView() {
        iconView->show();
        treeView->hide();
    }

    void navigateTo(const QString &path) {
        treeView->setRootIndex(model->index(path));
        iconView->setRootIndex(model->index(path));
        rootPathEdit->setText(path);
        QDir::setCurrent(path);

        if (historyStack.isEmpty() || historyStack.top() != path) {
            while (historyStack.size() > currentHistoryIndex + 1) {
                historyStack.pop();
            }
            historyStack.push(path);
            currentHistoryIndex = historyStack.size() - 1;
        }

        updateButtonStates();
    }

    void updateButtonStates() {
        backButton->setEnabled(currentHistoryIndex > 0);
        nextButton->setEnabled(currentHistoryIndex < historyStack.size() - 1);
    }
};

FileSystemBrowserPlugin::FileSystemBrowserPlugin() {
    name = tr("File system browser");
    author = tr("Diego Iastrubni <diegoiast@gmail.com>");
    iVersion = 0;
    sVersion = "0.0.1";
    autoEnabled = true;
    alwaysEnabled = false;
}

FileSystemBrowserPlugin::~FileSystemBrowserPlugin() {}

void FileSystemBrowserPlugin::on_client_merged(qmdiHost *host) {
    auto *pluginManager = dynamic_cast<PluginManager *>(host);
    auto panel = new FileSystemWidget;
    pluginManager->createNewPanel(Panels::West, tr("File system"), panel);

    connect(panel, &FileSystemWidget::fileDoubleClicked, [this, host](const QString &filePath) {
        PluginManager *pluginManager = dynamic_cast<PluginManager *>(mdiServer->mdiHost);
        if (pluginManager) {
            pluginManager->openFile(filePath);
        }
    });
}

void FileSystemBrowserPlugin::on_client_unmerged(qmdiHost *host) {}

#include "filesystembrowser.moc"
