#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

class StringListWidget : public QWidget {
    Q_OBJECT

public:
    StringListWidget(QWidget *parent = nullptr);
    void setList(const QStringList &items);
    QStringList getList() const;

private slots:
    void addItem();
    void deleteItem();
    void modifyItem();

private:
    QComboBox *comboBox;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *modifyButton;
};
