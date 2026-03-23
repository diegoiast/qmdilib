#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

enum class StringListMode { String, File, Directory };

class StringListWidget : public QWidget {
    Q_OBJECT

  public:
    explicit StringListWidget(QWidget *parent = nullptr);
    void setList(const QStringList &items);
    QStringList getList() const;

    void setMode(StringListMode mode);
    StringListMode mode() const { return listMode; }

  private slots:
    void addItem();
    void deleteItem();
    void modifyItem();

  private:
    QComboBox *comboBox;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *modifyButton;
    StringListMode listMode = StringListMode::String;
};
