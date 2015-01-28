#ifndef IMAGECLIPPER_H
#define IMAGECLIPPER_H

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>


class ImageClipper
{
public:
    ImageClipper();
    cv::Mat clip(const std::string & fileName);
    cv::Mat clip(const cv::Mat &input);
    void setWidth(int width);
    void setHeight(int height);
    void setMargin(int margin);
    void setLightColor(const cv::Vec3i &color);
    void setDarkColor(const cv::Vec3i &color);
    void addBgColor(const cv::Mat &input, cv::Mat &output);

    int width();
    int height();
    int margin();
    cv::Vec3i lightColor();
    cv::Vec3i darkColor();
private:
    cv::Vec3b getBgColor(const cv::Mat &img, const cv::Rect &rect);
    int m_width;
    int m_height;
    int m_margin;
    cv::Vec3i m_lightColor;
    cv::Vec3i m_darkColor;
    int m_targetWidth;
    int m_targetHeight;
};

#endif // IMAGECLIPPER_H
