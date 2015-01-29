#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageclipper.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QDir>
#include <QSettings>
#include <QTextStream>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_clipper(new ImageClipper)
{
    ui->setupUi(this);

    connect(ui->inputDirButton, SIGNAL(clicked()), this, SLOT(inputDirButtonClicked()));
    connect(ui->outputDirButton, SIGNAL(clicked()), this, SLOT(outputDirButtonClicked()));
    connect(ui->inputDirEdit, SIGNAL(textEdited(QString)), this, SLOT(inputDirChanged(QString)));
    connect(ui->outputDirEdit, SIGNAL(textEdited(QString)), this, SLOT(outputDirChanged(QString)));
    connect(ui->widthEdit, SIGNAL(textEdited(QString)), this, SLOT(setWidth(QString)));
    connect(ui->heightEdit, SIGNAL(textEdited(QString)), this, SLOT(setHeight(QString)));
    connect(ui->paddingEdit, SIGNAL(textEdited(QString)), this, SLOT(setPadding(QString)));
    connect(ui->lightColorButton, SIGNAL(clicked()), this, SLOT(lightColorButtonClicked()));
    connect(ui->darkColorButton, SIGNAL(clicked()), this, SLOT(darkColorButtonClicked()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(run()));

    ui->widthEdit->setText(QString::number(m_clipper->width()));
    ui->heightEdit->setText(QString::number(m_clipper->height()));
    ui->paddingEdit->setText(QString::number(m_clipper->margin()));

    cv::Vec3i cvColor = m_clipper->darkColor();
    QColor qtColor(cvColor[2], cvColor[1], cvColor[0]);
    ui->darkColorLabel->setText(qtColor.name());
    ui->darkColorLabel->setPalette(QPalette(qtColor));
    ui->darkColorLabel->setAutoFillBackground(true);

    cvColor = m_clipper->lightColor();
    qtColor = QColor(cvColor[2], cvColor[1], cvColor[0]);
    ui->lightColorLabel->setText(qtColor.name());
    ui->lightColorLabel->setPalette(QPalette(qtColor));
    ui->lightColorLabel->setAutoFillBackground(true);

    readSettings();
    ui->inputDirEdit->setText(m_inputDir);
    ui->outputDirEdit->setText(m_outputDir);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::inputDirButtonClicked(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    m_inputDir,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(m_inputDir != dir){
        ui->inputDirEdit->setText(dir);
        m_inputDir = dir;
        writeSettings();
    }
}

void MainWindow::outputDirButtonClicked(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    m_outputDir,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(m_outputDir != dir){
        ui->outputDirEdit->setText(dir);
        m_outputDir = dir;
        writeSettings();
    }
}

void MainWindow::inputDirChanged(const QString & dir){
    m_inputDir = dir;
    writeSettings();
}

void MainWindow::outputDirChanged(const QString & dir){
    m_outputDir = dir;
    writeSettings();
}

void MainWindow::setWidth(const QString & s) {
    m_clipper->setWidth(s.toInt());
}

void MainWindow::setHeight(const QString & s) {
    m_clipper->setHeight(s.toInt());
}

void MainWindow::setPadding(const QString & s) {
    m_clipper->setMargin(s.toInt());
}

void MainWindow::lightColorButtonClicked(){
    cv::Vec3i cvColor = m_clipper->lightColor();
    QColor oldColor(cvColor[2], cvColor[1], cvColor[0]);
    const QColor color = QColorDialog::getColor(oldColor, this, "Select Color");

    if (color.isValid() && color != oldColor) {
        ui->lightColorLabel->setText(color.name());
        ui->lightColorLabel->setPalette(QPalette(color));
        ui->lightColorLabel->setAutoFillBackground(true);
        m_clipper->setLightColor(cv::Vec3i(color.blue(), color.green(), color.red()));
    }
}

void MainWindow::darkColorButtonClicked(){
    cv::Vec3i cvColor = m_clipper->darkColor();
    QColor oldColor(cvColor[2], cvColor[1], cvColor[0]);
    const QColor color = QColorDialog::getColor(oldColor, this, "Select Color");

    if (color.isValid() && color != oldColor) {
        ui->darkColorLabel->setText(color.name());
        ui->darkColorLabel->setPalette(QPalette(color));
        ui->darkColorLabel->setAutoFillBackground(true);
        m_clipper->setDarkColor(cv::Vec3i(color.blue(), color.green(), color.red()));
    }
}

void MainWindow::run(){
    QDir inDir(m_inputDir);
    if(!inDir.exists()){
        return;
    }
    QDir outDir(m_outputDir);
    if(!outDir.exists()){
        outDir.mkdir(m_outputDir);
    }

    inDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList list = inDir.entryInfoList();

    QFile logFile(m_outputDir + "/Log.txt");
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
           return;
    QTextStream outStream(&logFile);

    for(int i=0; i<list.size(); i++){
        QFileInfo fileInfo = list.at(i);
        QString suffix = fileInfo.suffix();
        if((!suffix.compare("jpg", Qt::CaseInsensitive)) || (!suffix.compare("bmp", Qt::CaseInsensitive))
                || (!suffix.compare("jpeg", Qt::CaseInsensitive))
                || (!suffix.compare("png", Qt::CaseInsensitive)) ){
//            outStream<<fileInfo.fileName()<<"\n";
            logFile.write((fileInfo.fileName()+"\n").toStdString().c_str());
            logFile.flush();
            QTextCodec *code;
            code = QTextCodec::codecForName("gb18030");
            cv::Mat out = m_clipper->clip(code->fromUnicode(fileInfo.absoluteFilePath()).data());
            QString outFile = m_outputDir + "/" + fileInfo.completeBaseName() + ".png";
            cv::imwrite(code->fromUnicode(outFile).data(), out);
        }
    }
    statusBar()->showMessage(tr("Finish!"));
    logFile.close();
}

void MainWindow::writeSettings(){
    QSettings settings("Fangcheng","ImageClipping");
    settings.setValue("inputDir", m_inputDir);
    settings.setValue("outputDir", m_outputDir);
}

void MainWindow::readSettings(){
    QSettings settings("Fangcheng","ImageClipping");
    m_inputDir = settings.value("inputDir").toString();
    m_outputDir = settings.value("outputDir").toString();
}
