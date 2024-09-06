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
#include <QSettings>
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
        model->setFilter(QDir::AllEntries | QDir::NoDot | QDir::AllDirs);
        model->setNameFilterDisables(false);

        backButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowBack), "", this);
        backButton->setToolTip(tr("Back"));
        connect(backButton, &QPushButton::clicked, this, &FileSystemWidget::navigateBack);

        homeButton = new QPushButton(style()->standardIcon(QStyle::SP_DirHomeIcon), "", this);
        homeButton->setToolTip(tr("Navigate to the home directory"));
        connect(homeButton, &QPushButton::clicked, this, &FileSystemWidget::navigateHome);

        upButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowUp), "", this);
        upButton->setToolTip(tr("Navigate to the parent directory"));
        connect(upButton, &QPushButton::clicked, this, &FileSystemWidget::navigateUp);

        nextButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowForward), "", this);
        nextButton->setToolTip(tr("Next"));
        connect(nextButton, &QPushButton::clicked, this, &FileSystemWidget::navigateNext);

        toggleButton = new QPushButton(style()->standardIcon(QStyle::SP_FileDialogInfoView), "", this);
        toggleButton->setToolTip(tr("Toggle View"));
        connect(toggleButton, &QPushButton::clicked, this, &FileSystemWidget::toggleView);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(backButton);
        buttonLayout->addWidget(nextButton);
        buttonLayout->addWidget(upButton);
        buttonLayout->addWidget(homeButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(toggleButton);

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
        treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

        rootPathEdit = new QLineEdit(homePath, this);
        rootPathEdit->setClearButtonEnabled(true);

        QCompleter *completer = new QCompleter(model, this);
        completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        rootPathEdit->setCompleter(completer);

        connect(rootPathEdit, &QLineEdit::returnPressed, this, &FileSystemWidget::onRootPathEdited);

        filterEdit = new QLineEdit("*.*", this);
        filterEdit->setClearButtonEnabled(true);
        connect(filterEdit, &QLineEdit::returnPressed, this, &FileSystemWidget::onFilterChanged);
        QLabel *filterLabel = new QLabel(tr("Filter:"), this); // Label for the filter input

        QHBoxLayout *filterLayout = new QHBoxLayout;
        filterLayout->addWidget(filterLabel);
        filterLayout->addWidget(filterEdit);
        showTreeView();

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addLayout(buttonLayout); // Add the button layout at the top
        layout->addWidget(rootPathEdit);
        layout->addWidget(treeView);
        layout->addWidget(iconView);
        layout->addLayout(filterLayout);
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

  public slots:
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
            navigateTo(path);
        }
    }

    void onFilterChanged() {
        QString filterText = filterEdit->text().trimmed();
        QStringList filters = filterText.split(QRegularExpression("[,;]"), Qt::SkipEmptyParts);
        filters.replaceInStrings(QRegularExpression("^\\s+|\\s+$"), "");
        model->setNameFilters(filters);
    }

  public:
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
    bool isTreeVisible = true;

    QStack<QString> historyStack;
    int currentHistoryIndex;

    void showTreeView() {
        isTreeVisible = true;
        treeView->show();
        iconView->hide();
    }

    void showIconView() {
        isTreeVisible = false;
        iconView->show();
        treeView->hide();
    }

    void navigateTo(const QString &path) {
        if (!QDir(path).exists()) {
            return;
        }
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
        
        upButton->setEnabled(!QDir(path).isRoot());
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

    config.pluginName = "FileBrowserPlugin";
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey(Config::DisplayTreeKey)
                                     .setUserEditable(false)
                                     .setType(qmdiConfigItem::Bool)
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey(Config::DirectoryKey)
                                     .setDefaultValue("")
                                     .setType(qmdiConfigItem::String)
                                     .setUserEditable(false)
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setKey(Config::FilterKey)
                                     .setDefaultValue("")
                                     .setType(qmdiConfigItem::String)
                                     .setUserEditable(false)
                                     .build());
}

FileSystemBrowserPlugin::~FileSystemBrowserPlugin() {}

void FileSystemBrowserPlugin::on_client_merged(qmdiHost *host) {
    auto *pluginManager = dynamic_cast<PluginManager *>(host);
    panel = new FileSystemWidget;
    pluginManager->createNewPanel(Panels::West, tr("File system"), panel);

    connect(panel, &FileSystemWidget::fileDoubleClicked, [this](const QString &filePath) {
        PluginManager *pluginManager = dynamic_cast<PluginManager *>(mdiServer->mdiHost);
        if (pluginManager) {
            pluginManager->openFile(filePath);
        }
    });
}

void FileSystemBrowserPlugin::on_client_unmerged(qmdiHost *host) {
    // auto *pluginManager = dynamic_cast<PluginManager *>(host);
}

void FileSystemBrowserPlugin::loadConfig(QSettings &settings) {
    IPlugin::loadConfig(settings);
    auto savedDir = getConfig().getDirectory();
    auto savedFilter = getConfig().getFilter();
    auto isTreeVisible = getConfig().getDisplayTree();
    auto indexPath = this->panel->model->index(savedDir);

    this->panel->filterEdit->setText(savedFilter);
    this->panel->rootPathEdit->setText(savedDir);
    this->panel->treeView->setRootIndex(indexPath);
    this->panel->iconView->setRootIndex(indexPath);
    if (isTreeVisible) {
        this->panel->showTreeView();
    } else {
        this->panel->showIconView();
    }
}

void FileSystemBrowserPlugin::saveConfig(QSettings &settings) {
    config.setVariable(Config::DisplayTreeKey, this->panel->isTreeVisible);
    config.setVariable(Config::DirectoryKey, this->panel->rootPathEdit->text());
    config.setVariable(Config::DisplayTreeKey, this->panel->isTreeVisible);
    IPlugin::saveConfig(settings);
}

#include "filesystembrowser.moc"
