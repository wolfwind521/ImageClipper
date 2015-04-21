#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageclipper.h"
#include "textimagecreator.h"
#include "ruleeditor.h"
#include <iostream>
#include <QFileDialog>
#include <QColorDialog>
#include <QDir>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QJsonDocument>
#include <QTextEdit>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_clipper(new ImageClipper)
{
    ui->setupUi(this);

    // for images
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

    //for texts
    connect(ui->inputTextListButton, SIGNAL(clicked()), this, SLOT(inputTextListBtnClicked()));
    connect(ui->outputTextDirButton, SIGNAL(clicked()), this, SLOT(outTextDirBtnClicked()));
    connect(ui->inputTextListEdit, SIGNAL(textEdited(QString)), this, SLOT(inputTextListChanged(QString)));
    connect(ui->outputTextDirEdit, SIGNAL(textEdited(QString)), this, SLOT(outTextDirChanged(QString)));
    connect(ui->runTextButton, SIGNAL(clicked()), this, SLOT(textRun()));
    connect(ui->editRuleButton, SIGNAL(clicked()), this, SLOT(editRuleButtonClicked()));
    connect(ui->selectFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));

    ui->inputTextListEdit->setText(m_inputTextList);
    ui->outputTextDirEdit->setText(m_outputTextImageDir);

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
    statusBar()->showMessage("Proccessing...");
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
    settings.setValue("inputTextList", m_inputTextList);
    settings.setValue("outputTextImageDir", m_outputTextImageDir);
    settings.setValue("rules",m_rules);
}

void MainWindow::readSettings(){
    QSettings settings("Fangcheng","ImageClipping");
    m_inputDir = settings.value("inputDir").toString();
    m_outputDir = settings.value("outputDir").toString();
    m_inputTextList = settings.value("inputTextList").toString();
    m_outputTextImageDir = settings.value("outputTextImageDir").toString();
    m_rules = settings.value("rules").toString();
    if(m_rules.isEmpty() || m_rules.isNull()){
        QFile rulefile(":/settings.ini");
        if (!rulefile.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this,
                                tr("Error"),
                                tr("Can not find the settings file"));

            return;
        }
        QTextStream in(&rulefile);
        m_rules = in.readAll();
    }
}

////////////////// for texts///////////////////


void MainWindow::inputTextListBtnClicked(){

    QFileInfo fileInfo(m_inputTextList);
    QString filePath = fileInfo.absoluteFilePath();

    QString  fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open Text List"), filePath, tr("Text Files (*.txt *.csv)"));

    if(m_inputTextList != fileName){
        ui->inputTextListEdit->setText(fileName);
        m_inputTextList = fileName;
        writeSettings();
    }
}

void MainWindow::outTextDirBtnClicked(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    m_outputTextImageDir,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(m_outputTextImageDir != dir){
        ui->outputTextDirEdit->setText(dir);
        m_outputTextImageDir = dir;
        writeSettings();
    }
}

void MainWindow::inputTextListChanged(const QString & fileName){
    m_inputTextList = fileName;
    writeSettings();
}

void MainWindow::outTextDirChanged(const QString & dir){
    m_outputTextImageDir = dir;
    writeSettings();
}

void MainWindow::textRun(){
    statusBar()->showMessage("Proccessing...");
    m_time.start();
    QFile textListfile(m_inputTextList);
    if (!textListfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this,
                            tr("Error"),
                            tr("invalid input textlist file"));
        return;
    }

    if(m_outputTextImageDir.isEmpty()){
        QMessageBox::warning(this,
                            tr("Error"),
                            tr("empty output directory"));
        return;
    }
    QDir outDir(m_outputTextImageDir);
        if(!outDir.exists()){
            outDir.mkdir(m_outputTextImageDir);
    }
    if(m_rules.isEmpty() || m_rules.isNull()){
        QMessageBox::warning(this,
                            tr("Error"),
                            tr("no rules detected"));
        return;
    }

    QByteArray jsonData = m_rules.toUtf8();
    QJsonDocument loadDoc(QJsonDocument::fromJson(jsonData));


    QTextStream in(&textListfile);

    QString savefileName;

    while (!in.atEnd()) {
        TextImageCreator *creator = new TextImageCreator();
        creator->inputRules( loadDoc.object() );

        QString line = in.readLine();
        QStringList strlist = line.split(",");

        QString savefilePath;
        if(strlist.size() >= 3){
            savefilePath = strlist.at(2);
        }
        if(strlist.size() >= 2){
            savefileName = strlist.at(1);
        }else{
            savefileName = strlist.at(0);
        }

        if(savefilePath.isEmpty()){
            savefileName = m_outputTextImageDir+"/"+savefileName+".png";
        }else{
            savefilePath = m_outputTextImageDir + "/" + savefilePath;
            QDir saveDir(savefilePath);
                if(!saveDir.exists()){
                    saveDir.mkdir(savefilePath);
            }
            savefileName = savefilePath + "/" + savefileName + ".png";
        }
        creator->process(strlist.at(0), savefileName);
        delete creator;

    }
    statusBar()->showMessage(("Finish! Time Cost:" + QString::number(m_time.elapsed()) + "ms"));
}

void MainWindow::editRuleButtonClicked(){
    RuleEditor editor(m_rules, this);
    connect(&editor, SIGNAL(saveRules(QString)), this, SLOT(saveRules(QString)));
    editor.exec();

}

void MainWindow::saveRules( const QString &str){
    m_rules = str;
    writeSettings();
}

void MainWindow::selectFont(){

    QByteArray jsonData = m_rules.toUtf8();
    QJsonDocument loadDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject rule = loadDoc.object();

    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(rule["font"].toString()), this );
    if(ok){
        rule["font"] = font.family();
        loadDoc = QJsonDocument(rule);
        m_rules = QString(loadDoc.toJson());
        writeSettings();
    }
}
