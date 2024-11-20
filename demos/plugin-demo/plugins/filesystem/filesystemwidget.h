#include <QFileSystemModel>
#include <QStack>
#include <QTreeView>

class QFileSystemModel;
class QTreeView;
class QListView;
class QPushButton;
class QPushButton;
class QLineEdit;
class QLineEdit;
class QPushButton;
class QPushButton;
class QPushButton;

class qmdiActionGroup;

class FileSystemWidget : public QWidget {
    Q_OBJECT
  public:
    FileSystemWidget(QWidget *parent = nullptr);

    void initContextMenu();

  signals:
    void fileDoubleClicked(const QString &filePath);

  public slots:
    void handleFileDoubleClick(const QString &filePath);
    void showContextMenu(const QPoint &pos);
    void openFile();
    void editFile();
    void renameFile();
    void copyFile();
    void pasteFile();
    void cutFile();
    void deleteFile();
    void showProperties();
    void toggleView();
    void itemDoubleClicked(const QModelIndex &index);
    void navigateUp();
    void navigateBack();
    void navigateNext();
    void navigateHome();
    void onRootPathEdited();
    void onFilterChanged();
    void showTreeView();
    void showIconView();
    void navigateTo(const QString &path);
    void updateButtonStates();

  public:
    qmdiActionGroup *contextMenu;

    QFileSystemModel *model;
    QTreeView *treeView;
    QListView *iconView;
    QLineEdit *filterEdit;
    QLineEdit *rootPathEdit;
    bool isTreeVisible = true;

  private:
    QPushButton *toggleButton;
    QPushButton *upButton;
    QPushButton *backButton;
    QPushButton *nextButton;
    QPushButton *homeButton;
    QStack<QString> historyStack;
    int currentHistoryIndex;

    QAction *editAction;
    QModelIndex selectedFileIndex;
};
