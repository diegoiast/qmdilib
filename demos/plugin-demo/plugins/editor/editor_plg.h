#pragma once
/**
 * \file editorplugin.h
 * \brief Definition of the EditorPlugin class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 *  License LGPL
 * \see EditorPlugin
 */

#include "iplugin.h"

class QAction;

class EditorPlugin : public IPlugin {
    Q_OBJECT
  public:
    EditorPlugin();
    ~EditorPlugin();

    void showAbout();
    QActionGroup *newFileActions();
    QStringList myExtensions();
    virtual int canOpenFile(const QString &fileName) override;
    virtual bool openFile(const QString &fileName, int x = -1, int y = -1, int z = -1) override;

  public slots:
    void fileNew();

  private:
    QAction *actionNew;
    QActionGroup *_newFileActions;

    bool makeBackups;
    bool showLineNumbers;
    bool makeCurrentLine;
    bool wordWrap;
    QFont editorFont;
    int endOfLine;
};
