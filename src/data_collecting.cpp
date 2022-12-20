#include "opencv4/opencv2/opencv.hpp"
#include <iostream>
#include <fstream>

#define WIDTH 640
#define HEIGHT 360

using namespace cv;
using namespace std;

char *cam = "/dev/v4l/by-id/usb-Huawei_HiCamera_12345678-video-index0";

Point p = Point(0, 0);
Point prev_p = Point(0, 0);
Point center_frame = Point(WIDTH / 2, HEIGHT);

VideoCapture cap(0);

int main()
{
    Mat video;

    cap.open(cam);
    cap.set(CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CAP_PROP_FRAME_HEIGHT, HEIGHT);
    cap.set(CAP_PROP_FPS, 30);

    // open .txt file
    ofstream db;
    db.open("/home/dancoeks/Downloads/FinalProject-20221220T063437Z-001/FinalProject/db/data.txt");

    ofstream db_dist;
    db_dist.open("/home/dancoeks/Downloads/FinalProject-20221220T063437Z-001/FinalProject/db/data_2.txt");

    while (true)
    {
        if (!cap.isOpened())
        {
            cout << "Cannot open camera" << endl;
            return -1;
        }
        cap >> video;
        // cap.read(video);
        // get the X and Y from mouse click
        setMouseCallback(
            // add ofstream db to capture list
            "Video", [](int event, int x, int y, int flags, void *userdata)
            {
            if (event == EVENT_LBUTTONDOWN)
            {
                Point *p = (Point *)userdata;
                p->x = x;
                p->y = y;
            } },
            &p);

        if (p != prev_p)
        {
            // write to .txt file
            db << p.x << " " << p.y << endl;
            db_dist << sqrt(pow(p.x - center_frame.x, 2) + pow(p.y - center_frame.y, 2)) << endl;
            prev_p = p;
        }

        // draw a circle on the video
        circle(video, p, 5, Scalar(0, 0, 255), 2);
        circle(video, center_frame, 5, Scalar(0, 255, 0), -1);
        // cout << p << endl;
        // print width and height of video
        cout << video.size() << endl;
        imshow("Video", video);
        waitKey(1);

        if (waitKey(1) == 27)
            break;
    }

    db.close();
    db_dist.close();
}