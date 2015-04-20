#include "textimagecreator.h"
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

TextImageCreator::TextImageCreator():m_fontSize(25)
{
}

TextImageCreator::~TextImageCreator(){
    delete m_picture;
    delete m_painter;
}

void TextImageCreator::inputRules(const QJsonObject &rule){
    m_width = rule["width"].toInt();
    m_height = rule["height"].toInt();
    m_margin = rule["margin"].toInt();
    m_bgColor = QColor(rule["backgroundColor"].toString());
    m_fontColor = QColor(rule["fontColor"].toString());
    m_bgImg = QPixmap(rule["backgroundImage"].toString());
    m_lineBreak = rule["lineBreak"].toString();

    m_validWidth = m_width - 2 * m_margin;
    m_validHeight = m_height - 2 * m_margin;

    QJsonObject cnRule = rule["cn"].toObject();
    m_maxCnSize = cnRule["fontMaxSize"].toInt();
    m_minCnSize = cnRule["fontMinSize"].toInt();

    QJsonObject enRule = rule["en"].toObject();
    m_maxEnSize = enRule["fontMaxSize"].toInt();
    m_minEnSize = enRule["fontMinSize"].toInt();

    m_font = QFont(rule["font"].toString());
    //m_font.setLetterSpacing(QFont::AbsoluteSpacing,40);
    m_pen.setColor(QColor(rule["fontColor"].toString()));

    m_validRect = QRect(m_margin, m_margin, m_width-m_margin, m_height-m_margin);

    QTextOption textOption = m_textDoc.defaultTextOption();
    textOption.setAlignment(Qt::AlignCenter);
    m_textDoc.setDefaultTextOption(textOption);

    m_textDoc.setIndentWidth(0);
    m_textDoc.setDocumentMargin(0);

}

void TextImageCreator::computeMaxSizes(){
    //QString
}

void TextImageCreator::process(const QString &text, const QString &filepath){

    m_picture = new QImage(m_width, m_height, QImage::Format_ARGB32);
    m_painter = new QPainter();
    m_painter->begin(m_picture);           // paint in picture

    //draw background
    if(!m_bgImg.isNull()){
        m_painter->drawPixmap(0, 0, m_width, m_height, m_bgImg);
    }else if(m_bgColor.isValid()){
        m_picture->fill(m_bgColor);
        //m_painter->fillRect(0, 0, m_width, m_height, m_bgColor);
    }

//    m_painter->end();
//    return ;

    //check if has chinese characters
    QRegExp regExp("[\u4e00-\u9fa5]");
    bool hasCn = text.contains(regExp);

    //max, min font size
    if(hasCn){
        m_maxSize = m_maxCnSize;
        m_minSize = m_minCnSize;
    }else{
        m_maxSize = m_maxEnSize;
        m_minSize = m_minEnSize;
    }

    //max length
    QString maxLenString;
    QStringList strlist = text.split(m_lineBreak);
    QStringList::iterator iter;
    for(iter = strlist.begin(); iter != strlist.end(); iter++){
        (*iter) = (*iter).trimmed();
    }

    int maxWidth = findMaxWidthString(strlist, maxLenString);

    //minSize exceeded, need line breaking
    if(maxWidth > m_validWidth){
        m_fontSize = m_minSize;
        m_font.setPixelSize(m_fontSize);
    }else{
        findProperFont(maxLenString);
    }

    m_textDoc.setPlainText(strlist.join('\n'));
    m_painter->setPen(QPen(m_fontColor));
    m_painter->setBrush(QBrush(m_fontColor));
    m_textDoc.setDefaultFont(m_font);
    m_textDoc.setPageSize(QSizeF(m_validWidth, m_validHeight));
    m_textDoc.setTextWidth(m_validWidth);

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, m_fontColor);


    m_painter->save();
    m_painter->translate(m_margin, m_margin + (m_validHeight - m_textDoc.size().height() )/2);
    //m_painter.drawRect(0, 0, m_textDoc.size().width(), m_textDoc.size().height());
    m_textDoc.documentLayout()->draw(m_painter, ctx);
    m_painter->restore();
    m_painter->end();

    if(m_picture->save(filepath)){
        return;
    }else{
        qDebug()<<text <<" wrong";
    }

    //delete m_painter;

    return ;
    //m_painter.drawText(m_validRect,);
}

int TextImageCreator::findMaxWidthString(const QStringList &strlist , QString &maxLenStr){
    m_font.setPixelSize(m_minSize);
    int maxWidth = 0;
    int width;
    foreach(const QString &str, strlist){
        width = measureTextWidth(str);
        if(width > maxWidth){
            maxWidth = width;
            maxLenStr = str;
        }
    }
    return maxWidth;


}

int TextImageCreator::measureTextWidth(const QString &text){
    m_textDoc.setPageSize(QSizeF(1000,1000));
    m_textDoc.setTextWidth(1000);
    m_textDoc.setDefaultFont(m_font);
    m_textDoc.setPlainText(text);
    qreal width = m_textDoc.idealWidth();
    return int(width);
}

void TextImageCreator::findProperFont(const QString &text){

//    QFontMetrics minFm(m_font);
//    if(minFm.width(maxLenString) > m_validWidth){

//        QString str1 = text.section(" ", -2,-2);
//        if(!str1.isEmpty()){//has space
//            QString
//        }

//    }

    for(m_fontSize = m_maxSize; m_fontSize > m_minSize; m_fontSize--){
        m_font.setPixelSize(m_fontSize);
        int width = measureTextWidth(text);
        if(width < m_validWidth){
            return;
        }

//        QFontMetrics fm(m_font);
//        int pixelsWidth = fm.width(maxLenString);
//        if(pixelsWidth < m_validWidth){
//            break;
//        }
    }
}
