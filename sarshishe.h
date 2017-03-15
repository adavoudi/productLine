#ifndef SARSHISHE_H
#define SARSHISHE_H
#include <cv_toolbox.h>

class SarShishe
{
public:
    SarShishe();
    ~SarShishe();

    // returns True if the input image is defected
    static bool isDefected(Mat &src, Mat &output);

private:

    static bool hasDefectiveForm(Mat &src, Mat &problem);
    static bool hasDarkSpot(Mat &src, Mat &problem, double minArea = 8);

};

#endif // SARSHISHE_H
