#include "ruleeditor.h"
#include <QFile>
#include <QTextStream>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

RuleEditor::RuleEditor(const QString &rules, QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_resetButton = new QPushButton(this);
    m_resetButton->setText("reset default rules");
    m_resetButton->adjustSize();

     m_textEdit = new QPlainTextEdit(this);
     m_textEdit->resize(300,300);
     layout->addWidget(m_resetButton);
     layout->addWidget(m_textEdit);
     this->resize(300, 400);

     connect(m_textEdit->document(), SIGNAL(contentsChanged()),
             this, SLOT(documentWasModified()));
     connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetDefaultRules()));

     m_textEdit->setPlainText(rules);
}

void RuleEditor::documentWasModified()
{
    setWindowModified(m_textEdit->document()->isModified());
}

void RuleEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}


bool RuleEditor::maybeSave()
{
    if (m_textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void RuleEditor::resetDefaultRules(){
    QFile rulefile(":/settings.ini");
    if (!rulefile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,
                            tr("Error"),
                            tr("Can not find the default settings"));

        return;
    }
    QTextStream in(&rulefile);
    m_textEdit->setPlainText(in.readAll());
    m_textEdit->document()->setModified();
}

bool RuleEditor::save()
{
    emit saveRules(m_textEdit->toPlainText());
    return true;
}
