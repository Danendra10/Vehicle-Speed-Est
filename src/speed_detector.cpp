#include "opencv4/opencv2/opencv.hpp"

using namespace cv;

int main()
{
    VideoCapture cap(0);

    while (true)
    {
        Mat frame;
        cap >> frame;
        Mat frame_2;
        cap >> frame_2;

        Mat diff;
        absdiff(frame, frame_2, diff);
        flip(diff, diff, 1);
        Mat diff_gray;
        cvtColor(diff, diff_gray, COLOR_BGR2GRAY);

        Mat diff_gray_blur;
        GaussianBlur(diff_gray, diff_gray_blur, Size(21, 21), 0);

        Mat thresh;
        threshold(diff_gray_blur, thresh, 25, 255, THRESH_BINARY);

        Mat thresh_closed;
        Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));

        morphologyEx(thresh, thresh_closed, MORPH_CLOSE, kernel);

        Mat thresh_final;
        threshold(thresh_closed, thresh_final, 25, 255, THRESH_BINARY);

        imshow("Frame", frame);
        imshow("diff", diff);
        imshow("final", thresh_final);

        waitKey(10);
    }
}