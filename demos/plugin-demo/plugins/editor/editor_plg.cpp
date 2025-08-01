/**
 * \file editor_plg.cpp
 * \brief Implementation of the EditorPlugin class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL
 * \see EditorPlugin
 */

// $Id$

#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QUrl>

#include "editor_plg.h"
#include "iplugin.h"
#include "qexeditor.h"
#include "qexeditor2.h"
#include "qmdiserver.h"

#define CONFIG_KEY_FONT "Font"
#define CONFIG_KEY_WRAP_TEXT "WrapText"

EditorPlugin::EditorPlugin() {
    name = tr("Text editor plugin");
    author = tr("Diego Iastrubni <diegoiast@gmail.com>");
    iVersion = 0;
    sVersion = "0.0.1";
    autoEnabled = true;
    alwaysEnabled = false;

    actionNew = new_action(QIcon(":images/filenew.png"), tr("&New text file"), this, tr("Ctrl+N"),
                           tr("Create a new file"), SLOT(fileNew()));
    _newFileActions = new QActionGroup(this);
    _newFileActions->addAction(actionNew);

    toolbars["main"]->addAction(actionNew, 0);
    makeBackups = false;
    showLineNumbers = true;
    makeCurrentLine = true;
    wordWrap = true;

    auto monospacedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    config.pluginName = tr("Editor");
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setDisplayName(tr("Editor font"))
                                     .setKey(CONFIG_KEY_FONT)
                                     .setType(qmdiConfigItem::Font)
                                     .setDefaultValue(monospacedFont)
                                     .build());
    config.configItems.push_back(qmdiConfigItem::Builder()
                                     .setDisplayName(tr("Wrap text"))
                                     .setKey(CONFIG_KEY_WRAP_TEXT)
                                     .setDefaultValue(true)
                                     .setType(qmdiConfigItem::Bool)
                                     .build());
}

EditorPlugin::~EditorPlugin() { delete actionNew; }

void EditorPlugin::showAbout() {
    QMessageBox::about(NULL, tr("Text Editor plugin"),
                       tr("This plugin brings text editing capabilities to the application"));
}

QStringList EditorPlugin::myExtensions() {
    QStringList s;
    s << tr("Sources", "EditorPlugin::myExtensions") + " (*.c *.cpp *.cxx *.h *.hpp *.hxx *.inc)";
    s << tr("Headers", "EditorPlugin::myExtensions") + " (*.h *.hpp *.hxx *.inc)";
    s << tr("Qt project", "EditorPlugin::myExtensions") + " (*.pro *.pri)";
    s << tr("All files", "EditorPlugin::myExtensions") + " (*.*)";

    return s;
}

int EditorPlugin::canOpenFile(const QString &fileName) {
    QUrl u(fileName);

    /*	this code fails on win32
            for example: c:\windows
            scheme = "c:\"
     */
    // if the scheme is a single character, lets assume this is a windows drive
    if (u.scheme().length() != 1) {
        if ((u.scheme().toLower() != "file") && (!u.scheme().isEmpty())) {
            return -2;
        }
    }

    if (fileName.endsWith(".c", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".cpp", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".cxx", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".h", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".hpp", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".hxx", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".inc", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".pro", Qt::CaseInsensitive)) {
        return 5;
    } else if (fileName.endsWith(".pri", Qt::CaseInsensitive)) {
        return 5;
    } else {
        return 1;
    }
}

/**
 * \brief open a file
 * \param x the row to move the cursor to
 * \param y the line to move the cursor to
 * \param z unused, ignored
 * \return true if the file was opened, and the cursor reached the specified
 * location
 *
 * This function is used to open a file. The \b x and \b y parameters
 * can be used to specify the row/column to move the cursor to. If those
 * parameters have the value -1 the cursor will move to the "0" position
 * of that coordinate.
 *
 * If the file was not open, the function will return false.
 * If the cursor position could not be reached (out of bounds for example)
 * the function will return false.
 * On all other cases, return true to represent that the action was completed
 * without any problems.
 *
 */
bool EditorPlugin::openFile(const QString &fileName, int x, int y, int z) {
    auto editor = new QexTextEdit2(fileName, true, dynamic_cast<QMainWindow *>(mdiServer));
    auto fontName = config.getVariable<QString>(CONFIG_KEY_FONT);
    auto newFont = QFont();
    newFont.fromString(fontName);
    editor->setFont(newFont);

    if (config.getVariable<bool>(CONFIG_KEY_WRAP_TEXT)) {
        editor->setLineWrapMode(QTextEdit::FixedColumnWidth);
    } else {
        editor->setLineWrapMode(QTextEdit::NoWrap);
    }
    mdiServer->addClient(editor);

    // TODO
    // 1) move the cursor as specified in the parameters
    // 2) return false if the was was not open for some reason
    return true;
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
}

void EditorPlugin::fileNew() {
    if (!mdiServer) {
        qDebug("%s - %d : warning no mdiServer defined", __FUNCTION__, __LINE__);
        return;
    }

    auto editor = new QexTextEdit2(QString(), true);
    auto fontName = config.getVariable<QString>(CONFIG_KEY_FONT);
    auto newFont = QFont();
    newFont.fromString(fontName);
    editor->setFont(newFont);

    editor->mdiClientName = tr("No name");
    editor->setObjectName(editor->mdiClientName);
    mdiServer->addClient(editor);
}

void EditorPlugin::configurationHasBeenModified() {
    // Note:
    // This example only applies the configturation to newly added editors.
    // in your application, you could emit a signal and all editors should
    // connect to it, and update their configuration.
    // Another option - use `configurationHasBeenModified()` - and modify all existing
    // editors.
}
