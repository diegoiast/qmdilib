#include "stringlistwidget.h"

StringListWidget::StringListWidget(QWidget *parent) : QWidget(parent) {
    comboBox = new QComboBox(this);
    addButton = new QPushButton(tr("Add"), this);
    deleteButton = new QPushButton(tr("Delete"), this);
    modifyButton = new QPushButton(tr("Modify"), this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(modifyButton);

    mainLayout->addWidget(comboBox);
    mainLayout->addLayout(buttonLayout);

    mainLayout->setContentsMargins(0, mainLayout->contentsMargins().top(), 0,
                                   mainLayout->contentsMargins().bottom());
    buttonLayout->setContentsMargins(0, buttonLayout->contentsMargins().top(), 0,
                                     buttonLayout->contentsMargins().bottom());

    connect(addButton, &QPushButton::clicked, this, &StringListWidget::addItem);
    connect(deleteButton, &QPushButton::clicked, this, &StringListWidget::deleteItem);
    connect(modifyButton, &QPushButton::clicked, this, &StringListWidget::modifyItem);
}

void StringListWidget::setList(const QStringList &items) {
    comboBox->clear();
    comboBox->addItems(items);
}

QStringList StringListWidget::getList() const {
    QStringList items;
    for (int i = 0; i < comboBox->count(); ++i) {
        items.append(comboBox->itemText(i));
    }
    return items;
}

void StringListWidget::addItem() {
    bool ok;
    QString text =
        QInputDialog::getText(this, tr("Add Item"), tr("Item:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        comboBox->addItem(text);
    }
}

void StringListWidget::deleteItem() {
    int index = comboBox->currentIndex();
    if (index != -1) {
        comboBox->removeItem(index);
    } else {
        QMessageBox::warning(this, tr("Delete Item"), tr("Please select an item to delete."));
    }
}

void StringListWidget::modifyItem() {
    int index = comboBox->currentIndex();
    if (index == -1) {
        return;
    }
    bool ok;
    QString text = QInputDialog::getText(this, tr("Modify Item"), tr("Item:"), QLineEdit::Normal,
                                         comboBox->itemText(index), &ok);
    if (ok && !text.isEmpty()) {
        comboBox->setItemText(index, text);
    }
}
