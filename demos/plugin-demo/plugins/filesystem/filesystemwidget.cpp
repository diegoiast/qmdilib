#include "filesystemwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QCompleter>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>

FileSystemWidget::FileSystemWidget(QWidget *parent) : QWidget(parent) {
    QString homePath = QDir::homePath();

    model = new QFileSystemModel(this);
    model->setRootPath(homePath);
    model->setFilter(QDir::AllEntries | QDir::NoDot | QDir::AllDirs);
    model->setNameFilterDisables(false);
    model->setReadOnly(false);

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
    QLabel *filterLabel = new QLabel(tr("Filter:"), this);

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterEdit);

    showTreeView();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(buttonLayout);
    layout->addWidget(rootPathEdit);
    layout->addWidget(treeView);
    layout->addWidget(iconView);
    layout->addLayout(filterLayout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
    setWindowTitle(tr("File System Viewer"));

    connect(treeView, &QTreeView::doubleClicked, this, &FileSystemWidget::itemDoubleClicked);
    connect(iconView, &QListView::doubleClicked, this, &FileSystemWidget::itemDoubleClicked);

    historyStack.push(homePath);
    currentHistoryIndex = 0;

    updateButtonStates();

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    iconView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(treeView, &QTreeView::customContextMenuRequested, this,
            &FileSystemWidget::showContextMenu);
    connect(iconView, &QListView::customContextMenuRequested, this,
            &FileSystemWidget::showContextMenu);
}

void FileSystemWidget::handleFileDoubleClick(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        if (fileInfo.isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        } else if (fileInfo.isDir()) {
            navigateTo(filePath);
        }
    }
}

void FileSystemWidget::showContextMenu(const QPoint &pos) {
    QModelIndex index;
    if (QTreeView *view = qobject_cast<QTreeView *>(sender())) {
        index = view->indexAt(pos);
    } else if (QListView *view = qobject_cast<QListView *>(sender())) {
        index = view->indexAt(pos);
    }
    QFileInfo fileInfo = model->fileInfo(index);

    auto openAction = new QAction(tr("&Open in OS"), this);
    auto renameAction = new QAction(tr("&Rename"), this);
    auto copyAction = new QAction(tr("&Copy"), this);
    auto pasteAction = new QAction(tr("&Paste"), this);
    auto cutAction = new QAction(tr("Cu&t"), this);
    auto deleteAction = new QAction(tr("&Delete"), this);
    auto propertiesAction = new QAction(tr("&Properties"), this);

    connect(openAction, &QAction::triggered, this, &FileSystemWidget::openFile);
    connect(renameAction, &QAction::triggered, this, &FileSystemWidget::renameFile);
    connect(copyAction, &QAction::triggered, this, &FileSystemWidget::copyFile);
    connect(pasteAction, &QAction::triggered, this, &FileSystemWidget::pasteFile);
    connect(cutAction, &QAction::triggered, this, &FileSystemWidget::cutFile);
    connect(deleteAction, &QAction::triggered, this, &FileSystemWidget::deleteFile);
    connect(propertiesAction, &QAction::triggered, this, &FileSystemWidget::showProperties);

    auto contextMenu = new QMenu(this);
    contextMenu->addAction(openAction);
    if (fileInfo.isFile()) {
        auto editAction = new QAction(tr("Edit"), this);
        QFont boldFont = editAction->font();
        boldFont.setBold(true);
        editAction->setFont(boldFont);
        connect(editAction, &QAction::triggered, this, &FileSystemWidget::editFile);
        contextMenu->addAction(editAction);
    }

    contextMenu->addAction(renameAction);
    contextMenu->addAction(copyAction);
    contextMenu->addAction(pasteAction);
    contextMenu->addAction(cutAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(propertiesAction);

    if (index.isValid()) {
        selectedFileIndex = index;
        contextMenu->exec(QCursor::pos());
    }
}

void FileSystemWidget::openFile() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        QDesktopServices::openUrl(selectedFilePath);
    }
}

void FileSystemWidget::editFile() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        emit fileDoubleClicked(selectedFilePath);
    }
}

void FileSystemWidget::renameFile() {
    if (selectedFileIndex.isValid()) {
        treeView->edit(selectedFileIndex);
    }
}

void FileSystemWidget::copyFile() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(selectedFilePath);
    }
}

void FileSystemWidget::pasteFile() {
    QClipboard *clipboard = QApplication::clipboard();
    QString copiedFilePath = clipboard->text();
    if (QFileInfo::exists(copiedFilePath)) {
        QString destinationPath = QFileDialog::getExistingDirectory(this, tr("Paste File"));
        if (!destinationPath.isEmpty()) {
            QFile::copy(copiedFilePath,
                        destinationPath + "/" + QFileInfo(copiedFilePath).fileName());
            navigateTo(destinationPath);
        }
    }
}

void FileSystemWidget::cutFile() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(selectedFilePath);
        QFile::remove(selectedFilePath);
    }
}

void FileSystemWidget::deleteFile() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        if (QMessageBox::question(
                this, tr("Confirm Delete"),
                tr("Are you sure you want to delete %1?").arg(selectedFilePath)) ==
            QMessageBox::Yes) {
            QFile::remove(selectedFilePath);
            navigateTo(QFileInfo(selectedFilePath).absolutePath());
        }
    }
}

void FileSystemWidget::showProperties() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        QFileInfo fileInfo(selectedFilePath);
        QString properties = tr("Path: %1\nSize: %2 bytes\nType: %3\nLast Modified: %4")
                                 .arg(fileInfo.absoluteFilePath())
                                 .arg(fileInfo.size())
                                 .arg(fileInfo.isDir() ? tr("Directory") : tr("File"))
                                 .arg(fileInfo.lastModified().toString());
        QMessageBox::information(this, tr("File Properties"), properties);
    }
}

void FileSystemWidget::toggleView() {
    if (treeView->isVisible()) {
        showIconView();
    } else {
        showTreeView();
    }
}

void FileSystemWidget::itemDoubleClicked(const QModelIndex &index) {
    QFileInfo fileInfo = model->fileInfo(index);
    if (fileInfo.isDir()) {
        QString path = fileInfo.absoluteFilePath();
        navigateTo(path);
    } else {
        emit fileDoubleClicked(fileInfo.filePath());
    }
}

void FileSystemWidget::navigateUp() {
    QDir currentDir = QDir::current();
    currentDir.cdUp();
    QString path = currentDir.absolutePath();
    navigateTo(path);
}

void FileSystemWidget::navigateBack() {
    if (currentHistoryIndex > 0) {
        QString path = historyStack.at(--currentHistoryIndex);
        navigateTo(path);
    }
}

void FileSystemWidget::navigateNext() {
    if (currentHistoryIndex < historyStack.size() - 1) {
        QString path = historyStack.at(++currentHistoryIndex);
        navigateTo(path);
    }
}

void FileSystemWidget::navigateHome() {
    QString homePath = QDir::homePath();
    navigateTo(homePath);
}

void FileSystemWidget::onRootPathEdited() {
    QString path = rootPathEdit->text();
    QFileInfo fileInfo(path);
    if (fileInfo.isFile()) {
        emit fileDoubleClicked(fileInfo.filePath());
    } else if (fileInfo.isDir()) {
        navigateTo(path);
    }
}

void FileSystemWidget::onFilterChanged() {
    QString filterText = filterEdit->text().trimmed();
    QStringList filters = filterText.split(QRegularExpression("[,;]"), Qt::SkipEmptyParts);
    filters.replaceInStrings(QRegularExpression("^\\s+|\\s+$"), "");
    model->setNameFilters(filters);
}

void FileSystemWidget::showTreeView() {
    isTreeVisible = true;
    treeView->show();
    iconView->hide();
}

void FileSystemWidget::showIconView() {
    isTreeVisible = false;
    iconView->show();
    treeView->hide();
}

void FileSystemWidget::navigateTo(const QString &path) {
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

void FileSystemWidget::updateButtonStates() {
    backButton->setEnabled(currentHistoryIndex > 0);
    nextButton->setEnabled(currentHistoryIndex < historyStack.size() - 1);
}
