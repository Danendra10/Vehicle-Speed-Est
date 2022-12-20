#include "opencv4/opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include "yaml-cpp/yaml.h"
#include <string>

using namespace cv;
using namespace std;
using namespace std::chrono;

vector<Point> centers;
vector<double> regresi;

Mat frame;
float speed_kmph;

void LoadConfig()
{
    cout << "LoadConfig" << endl;
    char config_file[100];
    sprintf(config_file, "/home/dancoeks/Downloads/FinalProject-20221220T063437Z-001/FinalProject/db/regresion.yaml");
    cout << config_file << endl;
    YAML::Node config = YAML::LoadFile(config_file);
    cout << "n_orde: " << config["n_orde"].as<int>() << endl;
    for (int i = 1; i <= config["n_orde"].as<int>(); i++)
    {
        string orde = "orde_" + to_string(i);
        regresi.push_back(config[orde].as<double>());
        cout << orde << ": " << config[orde].as<double>() << endl;
    }
}

float PixelToCm(float pixel)
{
    double res = 0;
    for (int i = 0; i < regresi.size(); i++)
    {
        res += regresi[i] * pow(pixel, double(i));
    }
    return res;
}

Point getCenters(vector<vector<Point>> contours)
{
    Point centers;
    for (int i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) < 1000)
            continue;

        Rect rect = boundingRect(contours[i]);
        Point center = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
        centers = center;
    }
    return centers;
}

Point initial_points;
Point final_points;

Point initial_points_cm;
Point final_points_cm;

float initial_dist;
float initial_dist_cm;

float final_dist;
float final_dist_cm;

static uint16_t state;

float x_on_frame;
float y_on_frame;

float x_on_field;
float y_on_field;

#define WIDTH 640
#define HEIGHT 360

Point center_frame = Point(WIDTH / 2, HEIGHT);
int main()
{
    VideoCapture cap("/home/dancoeks/Downloads/FinalProject-20221220T063437Z-001/FinalProject/video/data.webm");
    // VideoCapture cap(0);
    LoadConfig();
    cout << "Regresi:" << regresi.size() << endl;
    // for (int i = 0; i < regresi.size(); i++)
    // {
    //     cout << regresi[i] << endl;
    // }
    milliseconds start_time;
    milliseconds end_time;
    // make a time of seconds
    seconds start;
    seconds end;
    while (true)
    {

        cap >> frame;
        Mat frame_2;
        cap >> frame_2;

        // draw center_frame
        circle(frame, center_frame, 5, Scalar(0, 0, 255), -1);

        Mat diff;
        absdiff(frame, frame_2, diff);
        // flip(diff, diff, 1);
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

        vector<vector<Point>> contours;
        findContours(thresh_final, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        // draw rect
        for (int i = 0; i < contours.size(); i++)
        {
            if (contourArea(contours[i]) < 50)
                continue;

            Rect rect = boundingRect(contours[i]);
            rectangle(frame, rect, Scalar(0, 255, 0), 2);
            circle(frame, Point(rect.x + rect.width / 2, rect.y + rect.height / 2), 5, Scalar(0, 0, 255), -1);
            initial_points = getCenters(contours);
            cout << "State " << state << endl
                 << endl;

            switch (state)
            {
            case 0:

                x_on_frame = initial_points.x - center_frame.x;
                y_on_frame = center_frame.y - initial_points.y;
                x_on_field = PixelToCm(x_on_frame);
                y_on_field = PixelToCm(y_on_frame);

                initial_dist = sqrt(x_on_frame * x_on_frame + y_on_frame * y_on_frame);
                initial_dist_cm = PixelToCm(initial_dist);
                cout << "All Data: " << x_on_frame << " " << y_on_frame << " " << x_on_field << " " << y_on_field << " " << initial_dist << " " << initial_dist_cm << endl;
                start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                state = 1;
                break;
            case 1:
                final_points = getCenters(contours);
                x_on_frame = final_points.x - center_frame.x;
                y_on_frame = center_frame.y - final_points.y;

                final_dist = sqrt(x_on_frame * x_on_frame + y_on_frame * y_on_frame);
                final_dist_cm = PixelToCm(final_dist);
                cout << "All Final Data: " << x_on_frame << " " << y_on_frame << " " << final_dist << " " << final_dist_cm << endl;
                end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                state = 2;
                break;
            case 2:
                cout << "Time: " << (end_time - start_time).count() << endl;
                float time = (float)((end_time - start_time).count()) / 1000;
                float speed = (fabs(final_dist_cm - initial_dist_cm)) / time;
                if (speed != 0)
                    speed_kmph = speed * 0.036;
                cout << "Time In Second: " << time << endl;
                cout << "Speed: " << speed << " cm/s" << endl;
                cout << "Speed: " << speed_kmph << " km/h" << endl;
                state = 0;
                break;
            }
        }
        putText(frame, "Speed: " + to_string(speed_kmph) + " km/h", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        cout << "Final speed: " << speed_kmph << endl;

        // if (contourArea(contours[0]) > 50)
        // {

        // }

        // switch (state)
        // {
        // case 0:
        //     // initial_points_cm = initial_points;
        //     initial_points_cm.x = PixelToCm(initial_points.x);
        //     initial_points_cm.y = PixelToCm(initial_points.y);
        //     start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        //     state = 1;
        //     break;
        // case 1:
        //     end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        //     cout << "Time: " << (end_time - start_time).count() << endl;
        //     if (end_time - start_time > milliseconds(1000))
        //     {
        //         cout << "MASUK SINI GASI" << endl;
        //         final_points_cm.x = PixelToCm(initial_points.x);
        //         final_points_cm.y = PixelToCm(initial_points.y);
        //         state = 99;
        //     }
        // }
        // if (state == 99)
        // {
        //     initial_dist = sqrt(pow(initial_points.x - center_frame.x, 2) + pow(center_frame.y - initial_points.y, 2));
        //     initial_dist_cm = sqrt(pow(initial_points_cm.x - center_frame.x, 2) + pow(center_frame.y - initial_points_cm.y, 2));
        //     final_dist = sqrt(pow(final_points.x - center_frame.x, 2) + pow(center_frame.y - final_points.y, 2));
        //     final_dist_cm = sqrt(pow(final_points_cm.x - center_frame.x, 2) + pow(center_frame.y - final_points_cm.y, 2));
        //     cout << "Initial point: " << initial_points << endl;
        //     cout << "Initial point in cm: " << initial_points_cm << endl;
        //     cout << "Final point: " << final_points << endl;
        //     cout << "Final point in cm: " << final_points_cm << endl;
        //     cout << "Initial distance: " << initial_dist << " px" << endl;
        //     cout << "Initial distance in cm: " << initial_dist_cm << " cm" << endl;
        //     cout << "Final distance: " << final_dist << " px" << endl;
        //     cout << "Final distance in cm: " << final_dist_cm << " cm" << endl;
        //     cout << "Speed: " << (fabs(final_points.x - initial_points_cm.x)) / ((end_time - start_time).count() / 2000.0) << " px/s" << endl;
        //     cout << "Speed in cm: " << (fabs(PixelToCm(final_points.x) - PixelToCm(initial_points_cm.x))) / ((end_time - start_time).count() / (1000.0 * 2)) << " cm/s" << endl;

        //     state = 0;
        // }

        // for (int i = 0; i < contours.size(); i++)
        // {
        //     if (contourArea(contours[i]) < 1000)
        //         continue;

        //     Rect rect = boundingRect(contours[i]);
        //     Point center = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
        //     centers.push_back(center);
        // }
        // if (initial_points.size() > 0)
        // {
        //     initial_points = getCenters(contours);
        //     switch (state)
        //     {
        //     case 0:
        //         initial_points_cm = initial_points;
        //         start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        //         state = 1;
        //         break;
        //     case 1:
        //         end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        //         if (end_time - start_time > milliseconds(2000))
        //         {
        //             final_points = initial_points;
        //             state = 99;
        //         }
        //     }
        //     if (state == 99)
        //     {
        //         cout << "Initial Points: " << initial_points_cm[0] << endl;
        //         cout << "Final Points: " << final_points[0] << endl;
        //         cout << "Speed: " << (final_points[0].x - initial_points_cm[0].x) / ((end_time - start_time).count() / 1000.0) << " cm/s" << endl;
        //         state = 0;
        //     }
        // }
        imshow("Frame", frame);
        // imshow("diff", diff);
        // imshow("final", thresh_final);
        // waitKey(1);
        waitKey(30);
        // if (waitKey(1) == 27)
        //     break;
    }
}