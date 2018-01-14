//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
using namespace cv;
using namespace std;

int main(int argc, char const *argv[]) {
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    char c;
    Mat frame, mask, result, kernel(3,3, CV_8UC1), bf;
    Ptr<BackgroundSubtractor> mog2;

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<int> hull;
    double area, maxArea, indexMaxArea;

    int blur = 15;
    int thresh = 60;
    bool backgroundCaptured = false;

    namedWindow("hand reco", 1);
    for(;;){
        cap >> frame; // get a new frame from camera
        if(backgroundCaptured){
            // mog2->apply(frame, mask);
            // erode(mask, mask, kernel);
            // bitwise_and(frame, frame, result, mask);
            // cvtColor(result, result, COLOR_BGR2GRAY);
            // GaussianBlur(result, result, Size(blur, blur), 0);
            // threshold(result, result, thresh, 255, THRESH_BINARY);

            absdiff(frame, bf, result);
            cvtColor(result, result, COLOR_BGR2GRAY);
            GaussianBlur(result, result, Size(blur, blur), 0);
            threshold(result, result, 10, 255, CV_THRESH_BINARY);
            erode(result, result, kernel);

            //Canny(result, result, thresh, thresh*2, 3);
            findContours(result, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

            Mat drawing = Mat::zeros(result.size(), CV_8UC3);
            maxArea = -1;
            for(int i=0 ; i<contours.size() ; i++){
                // drawContours(drawing, contours, i, Scalar(0,0,255), 2, 8, hierarchy, 0, Point());

                area = contourArea(contours[i]);
                if(area>maxArea){
                    maxArea = area;
                    indexMaxArea = i;
                }
            }

            drawContours(drawing, contours, indexMaxArea, Scalar(0,0,255), 2, 8, hierarchy, 0, Point());
            convexHull(contours[indexMaxArea], hull, true);
            int hullcount = (int)hull.size();
            Point pt0 = contours[indexMaxArea][hull[hullcount-1]];
            for(int i = 0; i < hullcount; i++ ){
                Point pt = contours[indexMaxArea][hull[i]];
                line(drawing, pt0, pt, Scalar(0, 255, 0), 1,LINE_AA);
                pt0 = pt;
            }
            imshow("contours", drawing);


            imshow("result", result);
        }

        imshow("hand reco", frame);

        c = waitKey(10);
        if(c==27){
            break;
        } else if(c=='b'){
            //mog2 = createBackgroundSubtractorMOG2(0, 50);
            cap >> bf;
            backgroundCaptured = true;
        }
    }

    return 0;
}
