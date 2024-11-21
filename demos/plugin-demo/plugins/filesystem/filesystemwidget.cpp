#include <QApplication>
#include <QClipboard>
#include <QCompleter>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QPushButton>

#include "filesystemwidget.h"
#include "qmdiactiongroup.h"

#if defined(WIN32)
// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <oleidl.h>
#include <shlobj.h>
// clang-format on

void showFileProperties(const QFileInfo &fileInfo) {
    // Convert QString to LPCWSTR (wide string)
    auto filePath = fileInfo.absoluteFilePath();
    SHELLEXECUTEINFO sei = {};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"properties";
    sei.lpFile = reinterpret_cast<LPCWSTR>(filePath.utf16());
    sei.nShow = SW_SHOW;
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    ShellExecuteEx(&sei);
}
#elif defined(Q_OS_MAC)
void showFileProperties(const QFileInfo &fileInfo) {
    auto filePath = fileInfo.absoluteFilePath();
    // macOS: Use 'open -R' to reveal the file in Finder, then show the info window
    QProcess::startDetached("open", QStringList() << "-R" << filePath);
}
#else
void showFileProperties(const QFileInfo &fileInfo) {
    if (!fileInfo.exists()) {
        qWarning() << "File does not exist:" << fileInfo.absoluteFilePath();
        return;
    }

    // Detect the desktop environment
    QString desktopEnv;
    QString envDesktop = qgetenv("XDG_CURRENT_DESKTOP").toUpper();
    if (envDesktop.contains("GNOME")) {
        desktopEnv = "GNOME";
    } else if (envDesktop.contains("KDE") || envDesktop.contains("KWIN")) {
        desktopEnv = "KDE";
    } else if (envDesktop.contains("XFCE") || envDesktop.contains("Xfce")) {
        desktopEnv = "Xfce";
    } else {
        desktopEnv = "Unknown";
    }

    QStringList args;
    if (desktopEnv == "GNOME") {
        args << "--properties" << fileInfo.absoluteFilePath();
        QProcess::startDetached("nautilus", args);
    } else if (desktopEnv == "KDE") {
        args << "openProperties" << fileInfo.absoluteFilePath();
        QProcess::startDetached("kioclient", args);
    } else if (desktopEnv == "Xfce") {
        args << "--properties" << fileInfo.absoluteFilePath();
        QProcess::startDetached("thunar", args);
    } else {
        QString properties =
            QCoreApplication::translate("showFileProperties",
                                        "Path: %1\nSize: %2 bytes\nType: %3\nLast Modified: %4")
                .arg(fileInfo.absoluteFilePath())
                .arg(fileInfo.size())
                .arg(fileInfo.isDir()
                         ? QCoreApplication::translate("showFileProperties", "Directory")
                         : QCoreApplication::translate("showFileProperties", "File"))
                .arg(fileInfo.lastModified().toString());

        QMessageBox::information(
            nullptr, QCoreApplication::translate("showFileProperties", "File Properties"),
            properties);
    }
}
#endif

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
    treeView->setEditTriggers(QAbstractItemView::EditKeyPressed);

    iconView = new QListView(this);
    iconView->setModel(model);
    iconView->setRootIndex(model->index(homePath));
    iconView->setViewMode(QListView::IconMode);
    iconView->setIconSize(QSize(64, 64));
    iconView->setEditTriggers(QAbstractItemView::EditKeyPressed);

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
    initContextMenu();

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    iconView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(treeView, &QTreeView::customContextMenuRequested, this,
            &FileSystemWidget::showContextMenu);
    connect(iconView, &QListView::customContextMenuRequested, this,
            &FileSystemWidget::showContextMenu);
}

void FileSystemWidget::initContextMenu() {
    contextMenu = new qmdiActionGroup(tr("File actions"));
    editAction = new QAction(tr("Edit"), this);
    editAction->setObjectName("editAction");

    auto newFileAction = new QAction(tr("&New file.."), this);
    auto newFolderAction = new QAction(tr("New &folder..."), this);
    auto renameAction = new QAction(tr("&Rename"), this);
    auto copyAction = new QAction(tr("&Copy"), this);
    auto pasteAction = new QAction(tr("&Paste"), this);
    auto cutAction = new QAction(tr("Cu&t"), this);
    auto deleteAction = new QAction(tr("&Delete"), this);
    auto openAction = new QAction(tr("&Open externally by OS..."), this);
    auto actionCopyFileName = new QAction(tr("Copy filename to clipboard"), this);
    auto actionCopyFilePath = new QAction(tr("Copy full path to clipboard"), this);
    auto propertiesAction = new QAction(tr("&Properties"), this);

    newFileAction->setObjectName("newFileAction");
    newFolderAction->setObjectName("newFolderAction");
    renameAction->setObjectName("renameAction");
    copyAction->setObjectName("copyAction");
    pasteAction->setObjectName("pasteAction");
    cutAction->setObjectName("cutAction");
    deleteAction->setObjectName("deleteAction");
    openAction->setObjectName("openAction");
    actionCopyFileName->setObjectName("actionCopyFi");
    actionCopyFilePath->setObjectName("actionCopyFi");
    propertiesAction->setObjectName("propertiesAction");

    editAction->setIcon(QIcon::fromTheme("document-open"));
    newFileAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
    newFolderAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::FolderNew));
    renameAction->setIcon(QIcon::fromTheme("edit-rename"));
    copyAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy));
    pasteAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste));
    cutAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCut));
    deleteAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
    openAction->setIcon(QIcon::fromTheme("gtk-execute"));
    actionCopyFileName->setIcon(QIcon::fromTheme("edit-copy"));
    actionCopyFilePath->setIcon(QIcon::fromTheme("edit-copy-path-symbolic"));
    propertiesAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));

    connect(newFileAction, &QAction::triggered, this, &FileSystemWidget::newFile);
    connect(newFolderAction, &QAction::triggered, this, &FileSystemWidget::newFolder);
    connect(openAction, &QAction::triggered, this, &FileSystemWidget::openFile);
    connect(editAction, &QAction::triggered, this, &FileSystemWidget::editFile);
    connect(renameAction, &QAction::triggered, this, &FileSystemWidget::renameFile);
    connect(copyAction, &QAction::triggered, this, &FileSystemWidget::copyFile);
    connect(pasteAction, &QAction::triggered, this, &FileSystemWidget::pasteFile);
    connect(cutAction, &QAction::triggered, this, &FileSystemWidget::cutFile);
    connect(deleteAction, &QAction::triggered, this, &FileSystemWidget::deleteFile);
    connect(propertiesAction, &QAction::triggered, this, &FileSystemWidget::showProperties);
    connect(actionCopyFileName, &QAction::triggered, this, [this]() {
        auto c = QApplication::clipboard();
        QFileInfo fileInfo = model->fileInfo(selectedFileIndex);
        c->setText(fileInfo.fileName());
    });
    connect(actionCopyFilePath, &QAction::triggered, this, [this]() {
        auto c = QApplication::clipboard();
        QFileInfo fileInfo = model->fileInfo(selectedFileIndex);
        c->setText(fileInfo.filePath());
    });

    auto boldFont = editAction->font();
    boldFont.setBold(true);
    editAction->setFont(boldFont);

    contextMenu = new qmdiActionGroup(tr("File actions"));
    contextMenu->addAction(editAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(copyAction);
    contextMenu->addAction(pasteAction);
    contextMenu->addAction(cutAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(newFileAction);
    contextMenu->addAction(newFolderAction);
    contextMenu->addSeparator();
    contextMenu->setMergePoint();
    contextMenu->addAction(actionCopyFileName);
    contextMenu->addAction(actionCopyFilePath);
    contextMenu->addAction(openAction);
    contextMenu->addAction(propertiesAction);
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
    if (auto treeview1 = qobject_cast<QTreeView *>(sender())) {
        index = treeview1->indexAt(pos);
    } else if (auto listview1 = qobject_cast<QListView *>(sender())) {
        index = listview1->indexAt(pos);
    }
    if (!index.isValid()) {
        return;
    }
    selectedFileIndex = index;

    QFileInfo fileInfo = model->fileInfo(index);
    editAction->setEnabled(fileInfo.isFile());

    auto menu = contextMenu->updateMenu(new QMenu(this));
    auto nameAction = new QAction(index.data().toString(), menu);
    nameAction->setEnabled(false);
    menu->insertAction(menu->actions().first(), nameAction);
    menu->exec(QCursor::pos());
}

std::tuple<QString, QString> findUniqueName(const QModelIndex &currentIndex,
                                            const QString &baseName,
                                            const QFileSystemModel &model) {
    QString directoryPath = model.filePath(currentIndex);
    QFileInfo fileInfo(baseName);
    QString nameWithoutExtension = fileInfo.completeBaseName();
    QString extension = fileInfo.suffix();
    QString tempName = baseName;
    QString fullPath = directoryPath + QDir::separator() + tempName;
    int counter = 1;

    // Increment the counter in the base name, not including the extension
    while (QFileInfo(fullPath).exists()) {
        if (extension.isEmpty()) {
            tempName = QStringLiteral("%1 %2").arg(nameWithoutExtension).arg(counter++);
        } else {
            tempName =
                QStringLiteral("%1 %2.%3").arg(nameWithoutExtension).arg(counter++).arg(extension);
        }
        fullPath = directoryPath + QDir::separator() + tempName;
    }

    return std::make_tuple(fullPath, tempName);
}

void FileSystemWidget::createNewItem(const QString &baseName, bool isDirectory) {
    QModelIndex currentIndex = treeView->currentIndex();
    if (!currentIndex.isValid()) {
        currentIndex = model->index(QDir::homePath());
    }

    if (!model->isDir(currentIndex)) {
        currentIndex = currentIndex.parent();
    }

    QString fullPath;
    QString tempName;
    std::tie(fullPath, tempName) = findUniqueName(currentIndex, baseName, *model);

    QModelIndex newIndex;
    if (isDirectory) {
        newIndex = model->mkdir(currentIndex, tempName);
    } else {
        QFile file(fullPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            newIndex = model->index(fullPath);
        }
    }

    if (!newIndex.isValid()) {
        qDebug() << "Failed to create item:" << fullPath;
        return;
    }

    auto tempConnectionManager = new QObject(this);
    auto delegate = treeView->itemDelegate();
    treeView->setCurrentIndex(newIndex);
    treeView->edit(newIndex);

    connect(delegate, &QAbstractItemDelegate::closeEditor, tempConnectionManager,
            [=](QWidget *, QAbstractItemDelegate::EndEditHint hint) {
                if (hint == QAbstractItemDelegate::RevertModelCache) {
                    if (isDirectory) {
                        QDir(fullPath).removeRecursively();
                    } else {
                        QFile::remove(fullPath);
                    }
                }
                delete tempConnectionManager;
            });

    connect(model, &QAbstractItemModel::dataChanged, tempConnectionManager,
            [=](const QModelIndex &topLeft, const QModelIndex &, const QVector<int> &) {
                if (topLeft == newIndex) {
                    delete tempConnectionManager;
                }
            });
}

void FileSystemWidget::newFile() { createNewItem(QStringLiteral("New File.txt"), false); }

void FileSystemWidget::newFolder() { createNewItem(QStringLiteral("New Folder"), true); }

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
    QItemSelectionModel *selectionModel = nullptr;

    if (auto treeview1 = qobject_cast<QTreeView *>(sender())) {
        selectionModel = treeview1->selectionModel();
    } else if (auto listview1 = qobject_cast<QListView *>(sender())) {
        selectionModel = listview1->selectionModel();
    }
    if (!selectionModel) {
        return;
    }

    auto selectedIndices = selectionModel->selectedIndexes();
    if (selectedIndices.isEmpty()) {
        qWarning() << "No files selected for copying.";
        return;
    }

    QList<QUrl> fileUrls;
    for (const QModelIndex &index : selectedIndices) {
        QFileInfo fileInfo = model->fileInfo(index);
        if (!fileInfo.absoluteFilePath().isEmpty()) {
            fileUrls << QUrl::fromLocalFile(fileInfo.absoluteFilePath());
        }
    }

    if (!fileUrls.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        QMimeData *mimeData = new QMimeData();
        mimeData->setUrls(fileUrls); // Add all file URLs
        clipboard->setMimeData(mimeData);
    }
}

void FileSystemWidget::pasteFile() {
    auto destinationInfo = model->fileInfo(selectedFileIndex);
    auto clipboard = QApplication::clipboard();
    auto mimeData = clipboard->mimeData();

    if (!mimeData->hasUrls()) {
        return;
    }

    auto urls = mimeData->urls();
    if (urls.isEmpty()) {
        return;
    }

    auto destinationPath = destinationInfo.absoluteFilePath();
    auto targetDirPath = QString();
    if (destinationInfo.isDir()) {
        targetDirPath = destinationPath;
    } else if (destinationInfo.isFile()) {
        targetDirPath = QFileInfo(destinationPath).absolutePath();
    } else {
        qWarning() << "Destination is neither a file nor a directory:" << destinationPath;
        return;
    }

    auto targetDir = QDir(targetDirPath);
    if (!targetDir.exists()) {
        qWarning() << "Target directory does not exist:" << targetDirPath;
        QMessageBox::warning(nullptr, "Paste", "Target directory does not exist.");
        return;
    }

    for (const QUrl &url : urls) {
        auto srcFilePath = url.toLocalFile();
        auto srcFileInfo = QFileInfo(srcFilePath);
        if (srcFileInfo.exists() && srcFileInfo.isFile()) {
            QString dstFilePath = targetDir.filePath(srcFileInfo.fileName());
            if (!QFile::copy(srcFilePath, dstFilePath)) {
                qWarning() << "Failed to copy file from" << srcFilePath << "to" << dstFilePath;
            }
        }
    }
}

void FileSystemWidget::cutFile() {
    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (selectedFilePath.isEmpty()) {
        qWarning() << "No file selected to cut.";
        return;
    }

    auto clipboard = QApplication::clipboard();
    auto mimeData = new QMimeData();
    auto urls = QList<QUrl>();

    urls.append(QUrl::fromLocalFile(selectedFilePath));
    mimeData->setUrls(urls);
#if defined(Q_OS_MAC) || defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
    // Should catch: Q_OS_FREEBSD, Q_OS_NETBSD, Q_OS_OPENBSD, and Q_OS_LINUX
    mimeData->setData("application/x-cut-operation", QByteArray("true"));

    // https://stackoverflow.com/questions/32612779/how-to-copy-local-file-to-qclipboard-in-gnome
    // QByteArray gnomeFormat =
    //     QByteArray("cut\n").append(QUrl::fromLocalFile(selectedFilePath).toEncoded());
    // mimeData->setData("x-special/gnome-copied-files", gnomeFormat);
#elif defined(Q_OS_WIN)
    // 2 for cut and 5 for copy
    int dropEffect = 2;
    QByteArray dataForClipboard;
    QDataStream stream(&dataForClipboard, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << dropEffect;
    mimeData->setData("Preferred DropEffect", dataForClipboard);
#else
    qWarning() << "Cut operation is not supported on this platform.";
#endif
    clipboard->setMimeData(mimeData);
}

void FileSystemWidget::deleteFile() {
    auto fi = model->fileInfo(selectedFileIndex);
    auto selectedFilePath = fi.absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        if (QMessageBox::question(
                this, tr("Confirm Delete"),
                tr("Are you sure you want to delete <b>%1</b>?").arg(selectedFilePath)) ==
            QMessageBox::Yes) {

            auto success = true;
            if (fi.isFile()) {
                success = QFile::remove(selectedFilePath);
            } else if (fi.isDir()) {
                auto dir = QDir(selectedFilePath);
                success = dir.removeRecursively();
            }
            navigateTo(QFileInfo(selectedFilePath).absolutePath());

            if (!success) {
                QMessageBox::warning(this, tr("Deletion Failed"),
                                     tr("Failed to delete <b>%1</b>").arg(selectedFilePath));
            }
        }
    }
}

void FileSystemWidget::showProperties() {

    auto selectedFilePath = model->fileInfo(selectedFileIndex).absoluteFilePath();
    if (!selectedFilePath.isEmpty()) {
        QFileInfo fileInfo(selectedFilePath);
        showFileProperties(fileInfo);
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
    rootPathEdit->setText(QDir::toNativeSeparators(path));
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
