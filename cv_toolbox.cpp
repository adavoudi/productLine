#include "cv_toolbox.h"
#include <iostream>
#include <string>
#include <cmath>

int CV_toolbox::showHistogram_histSize = 256;
int CV_toolbox::showHistogram_lowerThresh = 0, CV_toolbox::showHistogram_upperThresh = 255;


CV_toolbox::CV_toolbox()
{

}

CV_toolbox::~CV_toolbox()
{

}


Vector<Vec3i> CV_toolbox::findAllCircles(const Mat &src, int minArea, int mode)
{
    Vector<Vec3i> circles;


    std::vector<cv::Point> approx; // The array for storing the approximation curve
    std::vector< std::vector< Point> > contours;
    std::vector<Vec4i> hierarchy;

    findContours(src.clone(), contours, hierarchy, mode, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    for (int i = 0; i < contours.size(); i++)
    {
        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(
                    cv::Mat(contours[i]),
                    approx,
                    cv::arcLength(cv::Mat(contours[i]), true) * 0.02,
                    true
                    );

        // Skip small or non-convex objects
        if (std::fabs(cv::contourArea(contours[i])) < minArea || !cv::isContourConvex(approx))
            continue;

        if (approx.size() == 3)
        {
            // Triangles
        }
        else if (approx.size() >= 4 && approx.size() <= 6)
        {
            // Number of vertices of polygonal curve
            int vtc = approx.size();

            // Get the degree (in cosines) of all corners
            std::vector<double> cos;
            for (int j = 2; j < vtc+1; j++)
                cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

            // Sort ascending the corner degree values
            std::sort(cos.begin(), cos.end());

            // Get the lowest and the highest degree
            double mincos = cos.front();
            double maxcos = cos.back();

            // Use the degrees obtained above and the number of vertices
            // to determine the shape of the contour
            if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
            {
                // Detect rectangle or square
                cv::Rect r = cv::boundingRect(contours[i]);
                double ratio = std::abs(1 - (double)r.width / r.height);

                //                ratio <= 0.02 ? "SQU" : "RECT", contours[i];
            }
            else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
            {
                //                "PENTA
            }
            else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
            {
                //                "HEXA"
            }
        }
        else
        {
            // Detect circles
            double area = cv::contourArea(contours[i]);
            cv::Rect r = cv::boundingRect(contours[i]);
            int radius = r.width / 2;


            std::cout << "std::abs(1 - ((double)r.width / r.height)): " << std::abs(1 - ((double)r.width / r.height)) << std::endl;
            std::cout << "std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))): " << std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) << std::endl;
            if (std::abs(1 - ((double)r.width / r.height)) <= 0.02 &&
                    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.04)
            {
                circles.push_back(Vec3i(r.x + r.width / 2, r.y + r.height / 2, radius));
            }
        }
    }

    return circles;
}

void CV_toolbox::findClosedContours(const Mat &src, Mat &dst, int minArea, int mode, bool approximatePoly, Scalar external_color, Scalar hole_color, bool show)
{
    std::vector< std::vector< Point> > contours;
    std::vector<Vec4i> hierarchy;
    RNG rng(12345);
    Scalar color;

    findContours(src.clone(), contours, hierarchy, mode, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    dst.create(src.size(), CV_8UC3);
    dst = Scalar::all(0);

    for(int i = 0; i < contours.size(); i++)
    {

        //        bool Closed = hierarchy[i][2] != -1 || hierarchy[i][3] != -1;

        if(contourArea(contours[i]) < minArea)// || !Closed)
            continue;


        if(mode == CV_RETR_CCOMP) {
            if( hierarchy[i][3] == -1 )
            {
                if(external_color(0) == -1)
                    color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                else
                    color = external_color;
            } else {
                if(hole_color(0) == -1)
                    color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                else
                    color = hole_color;
            }
        } else {
            color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        }

        if(approximatePoly)
            approxPolyDP(Mat(contours[i]), contours[i], arcLength(Mat(contours[i]), true) * 0.01, true);

        drawContours( dst, contours , i, color, -1, 8, hierarchy, 0, Point() );
    }

    if(show)
        imshow("CLOSED CONTOURS", dst);
}

Mat CV_toolbox::barPlot(Mat_<float> &vectorOfData, int bins, int windowWidth, int windowHieght, bool show, std::string WIN_NAME)
{
    int numberBarHeight = 50;
    Mat plate(windowHieght + numberBarHeight, windowWidth, CV_8UC1, Scalar::all(255));

    normalize(vectorOfData, vectorOfData, 0, windowHieght, CV_MINMAX, CV_32F);

    int binW = round((double)windowWidth/bins);

    for( int i = 0; i < bins; i++ )
        rectangle( plate, Point(i*binW, windowHieght),
                   Point((i+1)*binW, windowHieght - round(vectorOfData(i))),
                   Scalar::all(0), -1, 8, 0 );

    int bestSizeForText = 30;
    double text_Bin_ratio = (double)bestSizeForText / binW < 1 ? 1: (double)bestSizeForText / binW;
    int numberOfTextWeCanDraw = text_Bin_ratio == 1 ? bins:round((double)windowWidth/bestSizeForText);

    for( int i = 0; i < numberOfTextWeCanDraw; i++ )
        putText(plate, std::to_string((int)round(i * ((double)bins/numberOfTextWeCanDraw))),
                Point(round(i*binW * text_Bin_ratio), windowHieght + 20),
                FONT_HERSHEY_SIMPLEX, bestSizeForText * 0.01, Scalar::all(0));

    if(show)
        imshow(WIN_NAME, plate);

    return plate;
}

Mat CV_toolbox::histogram(const Mat &src, int histSize, bool normalize, bool cumulative)
{
    float range[] = {0, 256};
    const float *ranges[] = {range};
    int channels = 0;

    Mat_<float> hist;
    calcHist(&src, 1, &channels, Mat(), hist, 1, &histSize, ranges, true, false);

    double div = src.rows * src.cols;

    if(normalize)
        for(int i = 0; i < histSize; i++)
            hist(i) /= div;

    if(cumulative)
        for(int i = 1; i < histSize; i++)
            hist(i) += hist(i-1);

    return hist;
}

void CV_toolbox::showHistogram_update(int pos, void* data)
{
    Mat *src_img = (Mat*) data;

    Mat_<float> hist = histogram(*src_img, showHistogram_histSize, true);
    barPlot(hist, showHistogram_histSize, 512, 256, true, "histogram - showHistogram function");

    Mat dst_img;
    inRange(*src_img, showHistogram_lowerThresh, showHistogram_upperThresh, dst_img);

    dst_img /= 255;
    multiply(*src_img, dst_img, dst_img);
    imshow("Input image - showHistogram function", dst_img);
}

void CV_toolbox::showHistogram(const Mat &src, int histSize, bool interactive)
{
    Mat_<float> hist = histogram(src, histSize, true);
    barPlot(hist, histSize, 512, 256, true, "histogram - showHistogram function");

    if(interactive)
    {
        imshow("Input image - showHistogram function", src);
        createTrackbar("Lower threshold", "histogram - showHistogram function",
                       &showHistogram_lowerThresh, 255, &showHistogram_update, (void *)(&src));
        createTrackbar("Upper threshold", "histogram - showHistogram function",
                       &showHistogram_upperThresh, 255, &showHistogram_update, (void *)(&src));

        createTrackbar("hist Size", "histogram - showHistogram function",
                       &showHistogram_histSize, histSize, &showHistogram_update, (void *)(&src));

    }
}

void CV_toolbox::histeq(const Mat &src, Mat &dst, int notCounted )
{

    Mat_<float> hist = histogram(src, 256, true);

    if(notCounted >= 0)
        hist(notCounted) = 0;

    for(int i = 1; i< 256; i++)
        hist(i) += hist(i-1);

    Mat_<int> lut(1, 256);
    for(int i = 0; i < 256; i++)
    {
        lut(i) = round(255.0 * hist(i));
    }

    LUT(src, lut, dst);
    dst.convertTo(dst, src.depth());

}

double CV_toolbox::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
