#pragma once

#include <QLineEdit>

class QCompleter;
class QFileSystemModel;

class PathWidget : public QLineEdit {
    Q_OBJECT

  public:
    explicit PathWidget(QWidget *parent = nullptr);

    inline QString path() const { return text(); }
    void setPath(const QString &path);
    bool isFileMode() const { return fileMode; }
    void setFileMode(bool isFile);
    bool validateExistence() const { return validateExist; }
    void setValidateExistence(bool validate);

  signals:
    void validationFailed(const QString &path);
    void pathChanged(const QString &path);
    void fileSelected(const QString &path);
    void directorySelected(const QString &path);

  private slots:
    void browse();
    void onTextChanged(const QString &text);
    void updateValidation();

  private:
    void setupUI();
    void updatePalette();
    QString getInitialDir() const;
    QString placeholderForMode() const;
    QString dialogTitleForMode() const;

    QFileSystemModel *fsModel;
    QCompleter *completer;
    bool fileMode = true;
    bool validateExist = !false;
    bool isValid = true;
};
