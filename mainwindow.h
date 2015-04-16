#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTime>

class ImageClipper;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //for images
    void inputDirButtonClicked();
    void outputDirButtonClicked();
    void inputDirChanged(const QString&);
    void outputDirChanged(const QString&);
    void setWidth(const QString &);
    void setHeight(const QString &);
    void setPadding(const QString &);
    void lightColorButtonClicked();
    void darkColorButtonClicked();
    void run();

    //for texts
    void inputTextListBtnClicked();
    void outTextDirBtnClicked();
    void inputTextListChanged(const QString &);
    void outTextDirChanged(const QString &);
    void editRuleButtonClicked();
    void saveRules(const QString &str);
    void selectFont();
    void textRun();

private:

    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;
    QString m_inputDir;
    QString m_outputDir;
    QString m_inputTextList;
    QString m_outputTextImageDir;
    QString m_rules;
    ImageClipper *m_clipper;
    QTime m_time;
};

#endif // MAINWINDOW_H
