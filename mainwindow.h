#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

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
private:

    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;
    QString m_inputDir;
    QString m_outputDir;
    ImageClipper *m_clipper;
};

#endif // MAINWINDOW_H
