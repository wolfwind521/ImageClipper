#include "imageclipper.h"
#include <math.h>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace cv;
using std::vector;

ImageClipper::ImageClipper():
    m_width(210), m_height(210), m_margin(20), m_lightColor(235, 235, 235), m_darkColor(141, 140, 159)
{
    m_targetHeight = m_height - 2*m_margin;
    m_targetWidth = m_width - 2*m_margin;
}

cv::Mat ImageClipper::clip(const std::string &fileName) {
    Mat inImg = imread(fileName, IMREAD_COLOR);
    return clip(inImg);
}

cv::Mat ImageClipper::clip(const Mat &input) {
    Mat inGray, thresholdOut;
    vector<vector<Point> > contours;
    vector<Point> samplePoints;

    cvtColor(input, inGray, CV_BGR2GRAY);
    blur(inGray, inGray, Size(3,3));
    Canny(inGray, inGray, 100, 200, 3);

#ifdef _DEBUG
    imshow("Canny", inGray);
#endif

    findContours(inGray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

    //get the bounding Rect
    Rect boundRect;
    if(contours.empty()){
        boundRect = Rect(Point(0,0), Point(input.cols, input.rows));
    }else{
        for(int i=0; i<contours.size(); i++){
            for(int j=0; j < contours[i].size(); j++){
                samplePoints.push_back(contours[i][j]);
            }
        }
        boundRect = boundingRect(Mat(samplePoints));
        //enlarge the rect
        Point tl = boundRect.tl();
        Point br = boundRect.br();

        //enlarge by 3 pixels
        if(tl.x > 3){
            tl.x -= 3;
        }else{
            tl.x = 0;
        }
        if(tl.y > 3){
            tl.y -= 3;
        }else{
            tl.y = 0;
        }
        if(br.x < input.cols - 4){
            br.x += 3;
        }else{
            br.x = input.cols - 1;
        }
        if(br.y < input.rows -4){
            br.y += 3;
        }else{
            br.y = input.rows - 1;
        }
        boundRect = Rect(tl, br);
    }

#ifdef _DEBUG
    rectangle( inGray, boundRect.tl(), boundRect.br(), Scalar(34,22,123) );
    imshow("original boundRec", inGray);
#endif

    //scale the image
    Mat scaleImg;
    double scaleWidth = double(m_targetWidth) / double(boundRect.width) ;
    double scaleHeight = double(m_targetHeight) / double(boundRect.height) ;
    double scale;

    if(scaleWidth < 1 || scaleHeight < 1){//shrink
        scale = scaleWidth < scaleHeight ? scaleWidth : scaleHeight;
        resize(input, scaleImg, Size(), scale, scale, CV_INTER_AREA);
    }else if(scaleWidth > 1 && scaleHeight > 1){ //enlarge
        scale = scaleWidth < scaleHeight ? scaleWidth : scaleHeight;
        resize(input, scaleImg, Size(), scale, scale, CV_INTER_CUBIC);
    }else{
        scale = 1.0;
    }

    //get the scaled bounding Rect
    Point tl,br;
    tl = boundRect.tl();
    tl = Point(floor(tl.x*scale), floor(tl.y*scale));
    br = boundRect.br();
    br = Point(floor(br.x*scale), floor(br.y*scale));
    boundRect = Rect(tl, br);

#ifdef _DEBUG
    Mat scaleImgCopy;
    scaleImg.copyTo(scaleImgCopy);
    rectangle( scaleImgCopy, boundRect.tl(), boundRect.br(), Scalar(34,22,123) );
    imshow("scale img rect", scaleImgCopy);
#endif

    //clipping
    Point rectCenter;
    rectCenter.x = (boundRect.tl().x + boundRect.br().x)/2;
    rectCenter.y = (boundRect.tl().y + boundRect.br().y)/2;
    if( rectCenter.x > m_width/2 && (scaleImg.cols - rectCenter.x) >= m_width/2
            && rectCenter.y > m_height/2 && (scaleImg.rows - rectCenter.y) >= m_height/2 )//just clip
    {
        int wStart = (boundRect.tl().x + boundRect.br().x - m_width)/2 ;
        int hStart = (boundRect.tl().y + boundRect.br().y - m_height)/2;
        Mat resultImg =  scaleImg.colRange(wStart, wStart + m_width).rowRange(hStart, hStart + m_height);

        if(getBgColor(scaleImg, boundRect) == Vec3b(255,255,255)){
            addBgColor(resultImg, resultImg);
        }
        return resultImg;
    }
    else{//fill the boundaries with background color
        Vec3b bgColor = getBgColor(scaleImg, boundRect);
        Mat resultImg(m_width, m_height, CV_8UC3, Scalar(bgColor));

        int wStart, hStart;
        wStart = (m_width-boundRect.width)/2;
        hStart = (m_height-boundRect.height)/2;
        Mat aux = resultImg.colRange(wStart, wStart + (boundRect.width)).rowRange(hStart, hStart + (boundRect.height));

        wStart = boundRect.x;
        hStart = boundRect.y;
        scaleImg.colRange(wStart, wStart + (boundRect.width)).rowRange(hStart, hStart + (boundRect.height)).copyTo(aux);

        if(bgColor == Vec3b(255,255,255)){
            addBgColor(resultImg, resultImg);
        }

#ifdef _DEBUG
        imshow("result", resultImg);
#endif
        return resultImg;
    }
}

cv::Vec3b ImageClipper::getBgColor(const cv::Mat & img, const cv::Rect &rect){

    struct ColorHash{
        size_t operator() (const Vec3i &key) const {
            return key[0] + key[1] +key[2];
        }
    };

    typedef std::unordered_map<Vec3i, int, ColorHash> ColorMap;


    ColorMap colorMap;
    ColorMap::iterator iter;
    Vec3b tmpColor;
    for(int it=rect.tl().y; it<rect.br().y; it++){

        tmpColor = img.at<Vec3b>(it, rect.tl().x);

        iter = colorMap.find(tmpColor);
        if(iter == colorMap.end()){
            colorMap.insert(std::make_pair(tmpColor,1));
        }else{
            (*iter).second++;
        }

        tmpColor = img.at<Vec3b>(it,  rect.br().x);

        iter = colorMap.find(tmpColor);
        if(iter == colorMap.end()){
            colorMap.insert(std::make_pair(tmpColor,1));
        }else{
            (*iter).second++;
        }

    }
    for(int i=rect.tl().x; i<rect.br().x; i++){
        tmpColor = img.at<Vec3b>(rect.tl().y, i);

        iter = colorMap.find(tmpColor);
        if(iter == colorMap.end()){
            colorMap.insert(std::make_pair(tmpColor,1));
        }else{
            (*iter).second++;
        }

        tmpColor = img.at<Vec3b>(rect.br().y, i);

        iter = colorMap.find(tmpColor);
        if(iter == colorMap.end()){
            colorMap.insert(std::make_pair(tmpColor,1));
        }else{
            (*iter).second++;
        }
    }

    int maxCount = 0;
    Vec3b bgColor;
    for(iter = colorMap.begin(); iter!=colorMap.end(); iter++){
        if((*iter).second > maxCount){
            maxCount = (*iter).second;
            bgColor = (*iter).first;
        }
    }

    return bgColor;
}

void ImageClipper::setWidth(int width){
    m_width = width;
    m_targetWidth = m_width - m_margin;
}

void ImageClipper::setHeight(int height) {
    m_height = height;
    m_targetHeight = m_height - m_margin;
}

void ImageClipper::setMargin(int margin) {
    m_margin = margin;
    m_targetWidth = m_width - m_margin;
    m_targetHeight = m_height - m_margin;
}

void ImageClipper::setLightColor(const Vec3i &color){
    m_lightColor = color;
}

void ImageClipper::setDarkColor(const Vec3i &color) {
    m_darkColor = color;
}

int ImageClipper::width(){
    return m_width;
}

int ImageClipper::height(){
    return m_height;
}

int ImageClipper::margin(){
    return m_margin;
}

cv::Vec3i ImageClipper::lightColor(){
    return m_lightColor;
}

cv::Vec3i ImageClipper::darkColor(){
    return m_darkColor;
}


void ImageClipper::addBgColor(const Mat &input, Mat & output){
    for(int y = 0; y < input.rows; y++){
        for(int x = 0; x < input.cols; x++){
            for(int c=0; c<3; c++){
                int ic = input.at<Vec3b>(y,x)[c];
                output.at<Vec3b>(y,x)[c] =  ic * m_lightColor[c] /255;
            }
        }
    }
}
