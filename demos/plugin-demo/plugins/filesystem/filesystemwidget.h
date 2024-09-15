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

class FileSystemWidget : public QWidget {
    Q_OBJECT
  public:
    FileSystemWidget(QWidget *parent = nullptr);

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

    QModelIndex selectedFileIndex;
};
