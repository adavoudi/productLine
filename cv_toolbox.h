#ifndef CV_TOOLBOX_H
#define CV_TOOLBOX_H

#include <opencv2/opencv.hpp>

using namespace cv;

class CV_toolbox
{
public:
    CV_toolbox();
    ~CV_toolbox();

    // Histogram processing
    static void histeq(const Mat &src, Mat &dst, int notCounted = -1);
    static Mat histogram(const Mat &src, int histSize = 256, bool normalize = false, bool cumulative = false);
    static void showHistogram(const Mat &src, int histSize = 256, bool interactive = false);
    static Mat barPlot(Mat_<float> &vectorOfData, int bins, int windowWidth = 512, int windowHieght = 256, bool show = true, std::string WIN_NAME = "HISTOGRAM");

    // Contours
    static void findClosedContours(const Mat &src, Mat &dst, int minArea = -1, int mode = CV_RETR_TREE, bool approximatePoly = false, Scalar external_color = Scalar::all(255), Scalar hole_color = Scalar::all(0), bool show = false);
    static Vector<Vec3i> findAllCircles(const Mat &src, int minArea = -1, int mode = CV_RETR_TREE);

private:


    // Histogram processing
    static int showHistogram_histSize;
    static int showHistogram_lowerThresh, showHistogram_upperThresh;
    static void showHistogram_update(int pos, void*data);
    static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
};

#endif // CV_TOOLBOX_H
