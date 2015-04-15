#ifndef RULEEDITOR_H
#define RULEEDITOR_H

#include <QDialog>

class QPlainTextEdit;

class RuleEditor : public QDialog
{
    Q_OBJECT
public:
    explicit RuleEditor(const QString &rules, QWidget *parent = 0);
signals:
    void saveRules(QString rules);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void documentWasModified();
    void resetDefaultRules();
private:
    bool save();
    bool maybeSave();
    QPlainTextEdit *m_textEdit;
    QPushButton *m_resetButton;

};

#endif // RULEEDITOR_H
