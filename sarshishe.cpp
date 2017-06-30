#include "sarshishe.h"
#include <iostream>


bool SarShishe::isDefected(Mat &src, Mat &output)
{

    Mat src_ = src.clone();
    if(src_.channels() == 3)
        cvtColor(src_, src_, CV_BGR2GRAY);

    bool defForm = hasDefectiveForm(src_, output);
    if(defForm == true)
        return true;

    src_ = output.clone();
    bool defSpot = hasDarkSpot(src_, output);
    if(defSpot == true)
        return true;

    return false;
}

bool SarShishe::hasDefectiveForm(Mat &src, Mat &problem)
{
    Mat dst;
    threshold(src, dst, 50, 255, CV_THRESH_BINARY);

    std::vector<Vec3i> circles = CV_toolbox::findAllCircles(dst);

    if(circles.size() == 2){
        double d = sqrt(pow(circles[0][0] - circles[1][0], 2) + pow(circles[0][1] - circles[1][1], 2));

        std::cout << "D: " << d << std::endl;

        if(d < 2.5){
            Mat mask = Mat::zeros(src.size(), CV_8UC1);
            circle(mask, Point(circles[0][0], circles[0][1]), circles[0][2], Scalar::all(1), -1);
            circle(mask, Point(circles[1][0], circles[1][1]), circles[1][2], Scalar::all(0), -1);

            multiply(src, mask, problem);
            return false;
        }
    }

    problem = src.clone();
    for(int i = 0; i < circles.size(); i++)
    {
        circle(problem, Point(circles[i][0], circles[i][1]), circles[i][2], Scalar::all(255), 2);
    }

    return true;
}

bool SarShishe::hasDarkSpot(Mat &src, Mat &problem, double minArea)
{

    Mat dst, blured;

    GaussianBlur(src, blured, Size(5,5), 1);
    Laplacian(blured, dst, CV_8UC1, 3);
    dst = blured - dst;
    GaussianBlur(dst, dst, Size(5,5), 5);
    Laplacian(dst, dst, CV_8UC1, 5);
    dst = blured - dst;

    std::vector< std::vector< Point> > contours;
    std::vector<Vec4i> hierarchy;
    RNG rng(12345);
    Scalar color;

    findContours(dst.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    int count = 0;

    dst = Mat::zeros(src.size(), CV_8UC3);

    for(int i = 0; i < contours.size(); i++)
    {

        if(contourArea(contours[i]) < minArea)
            continue;

        count ++;

        color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours( dst, contours , i, color, -1, 8, hierarchy, 0, Point() );
    }

    if(count == 2)
        return false;

    cvtColor(src, src, CV_GRAY2BGR);
    problem = 0.5 * src + 0.5 * dst;

    return true;
}



SarShishe::SarShishe()
{

}

SarShishe::~SarShishe()
{
}
