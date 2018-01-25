#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char const *argv[]) {
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    char key;
    Mat frame, mask, result, kernel(3,3, CV_8UC1), bf;

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<int> hull;
    double area, maxArea, indexMaxArea;

    int blur = 15;
    bool backgroundCaptured = false;

    namedWindow("hand reco", 1);

    for(;;){
        cap >> frame; // get a new frame from camera
        if(backgroundCaptured){
            absdiff(frame, bf, result);
            cvtColor(result, result, COLOR_BGR2GRAY);
            GaussianBlur(result, result, Size(blur, blur), 0);
            threshold(result, result, 10, 255, CV_THRESH_BINARY);
            erode(result, result, kernel);

            findContours(result, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

            Mat drawing = Mat::zeros(result.size(), CV_8UC3);
            maxArea = -1;

            for(int i=0 ; i<contours.size() ; i++){
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

        key = waitKey(10);
        if(key==27){
            break;
        }
        else if(key=='b'){
            cap >> bf;
            backgroundCaptured = true;
        }
    }

    return 0;
}
