#ifndef TEXTIMAGECREATOR_H
#define TEXTIMAGECREATOR_H
#include <QJsonObject>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QPicture>
#include <QVector>
#include <QRect>
#include <QTextDocument>

class TextImageCreator
{
public:
    TextImageCreator();
    ~TextImageCreator();
    void inputRules(const QJsonObject & rule);
    void process(const QString & text, const QString & filePath);

private:

    void computeMaxSizes();
    void findProperFont(const QString & text);
    int findMaxWidthString(const QStringList &strList, QString &maxLenStr);
    int measureTextWidth(const QString & text);

    int m_width;
    int m_height;
    int m_validWidth;
    int m_validHeight;
    int m_margin;
    QColor m_fontColor;
    QColor m_bgColor;
    QPixmap m_bgImg;
    QString m_lineBreak;
    int m_maxCnSize;
    int m_minCnSize;
    int m_maxEnSize;
    int m_minEnSize;
    QFont m_font;

    int m_minSize;
    int m_maxSize;
    int m_fontSize;

    QPainter *m_painter;
    QPen m_pen;
    QImage *m_picture;
    QTextDocument m_textDoc;

    QVector<int> m_cnSizes;
    QVector<int> m_enSizes;
    QRect m_validRect;


};

#endif // TEXTIMAGECREATOR_H
