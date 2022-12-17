#include <iostream>
#include <sstream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main()
{
    // create matrix for storage
    Mat image;
    Mat image2;
    Mat image3;
    Mat image4;

    // initialize capture
    VideoCapture cap;
    cap.open(0);

    // create window to show image
    namedWindow("window", 1);

    while (1)
    {

        // copy webcam stream to image
        cap >> image;
        cap >> image2;

        absdiff(image, image2, image3);
        threshold(image3, image4, 128, 255, THRESH_BINARY);

        // print image to screen
        if (!image.empty())
        {

            imshow("window", image3);
        }

        // delay33ms

        waitKey(10);

        //
    }
}