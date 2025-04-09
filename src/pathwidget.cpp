#include "pathwidget.h"

#include <QApplication>
#include <QCompleter>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QStyle>

PathWidget::PathWidget(QWidget *parent)
    : QLineEdit(parent), fsModel(new QFileSystemModel(this)),
      completer(new QCompleter(fsModel, this)) {
    setupUI();
    fsModel->setRootPath(QDir::rootPath());
    fsModel->setNameFilterDisables(false);
    fsModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    completer->setModel(fsModel);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(completer);
    connect(this, &QLineEdit::textChanged, this, &PathWidget::onTextChanged);
}

void PathWidget::setupUI() {
    setPlaceholderText(fileMode ? tr("Select a file...") : tr("Select a directory..."));

    auto icon = QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen,
                                 QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
    auto browseAction = addAction(icon, QLineEdit::TrailingPosition);
    browseAction->setToolTip(tr("Browse..."));
    connect(browseAction, &QAction::triggered, this, &PathWidget::browse);
}

void PathWidget::setPath(const QString &path) {
    if (text() != path) {
        setText(path);
    }
}

void PathWidget::setFileMode(bool isFile) {
    if (fileMode != isFile) {
        fileMode = isFile;
        setPlaceholderText(fileMode ? tr("Select a file...") : tr("Select a directory..."));
        updateValidation();
    }
}

void PathWidget::setValidateExistence(bool validate) {
    if (validateExist != validate) {
        validateExist = validate;
        updateValidation();
    }
}

void PathWidget::browse() {
    QString selectedPath;
    auto initialDir = getInitialDir();

    if (fileMode) {
        selectedPath = QFileDialog::getOpenFileName(this, tr("Select File"), initialDir);
    } else {
        selectedPath = QFileDialog::getExistingDirectory(this, tr("Select Directory"), initialDir);
    }
    if (!selectedPath.isEmpty()) {
        setText(selectedPath);
    }
}

void PathWidget::onTextChanged(const QString &text) {
    updateValidation();
    emit pathChanged(text);

    if (!text.isEmpty()) {
        if (fileMode) {
            emit fileSelected(text);
        } else {
            emit directorySelected(text);
        }
    }
}

void PathWidget::updateValidation() {
    if (!validateExist) {
        isValid = true;
        updatePalette();
        return;
    }

    auto fileInfo = QFileInfo(text());
    if (fileMode) {
        isValid = fileInfo.exists() && fileInfo.isFile();
    } else {
        isValid = fileInfo.exists() && fileInfo.isDir();
    }
    updatePalette();
    if (!isValid) {
        emit validationFailed(text());
    }
}

void PathWidget::updatePalette() {
    auto pal = style()->standardPalette();
    if (!isValid && validateExist) {
        pal.setColor(QPalette::Text, pal.color(QPalette::LinkVisited));
    }
    setPalette(pal);
}

QString PathWidget::getInitialDir() const {
    auto currentPath = text();
    if (currentPath.isEmpty()) {
        return QDir::currentPath();
    }
    auto fileInfo = QFileInfo(currentPath);
    return fileInfo.isDir() ? currentPath : fileInfo.absolutePath();
}
