
#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/cuda.hpp>
#include "roi_extraction.h"
#include "region_growing.h"

using namespace cv;
using namespace std;

string type2str(int type) {
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
        case CV_8U:
            r = "8U";
            break;
        case CV_8S:
            r = "8S";
            break;
        case CV_16U:
            r = "16U";
            break;
        case CV_16S:
            r = "16S";
            break;
        case CV_32S:
            r = "32S";
            break;
        case CV_32F:
            r = "32F";
            break;
        case CV_64F:
            r = "64F";
            break;
        default:
            r = "User";
            break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

int main() {
    Mat cls_map = imread("/home/leonid/trunk/line_segment_detection/for_cpp/cls_map.tiff", IMREAD_ANYDEPTH);
    Mat angle_map = imread("/home/leonid/trunk/line_segment_detection/for_cpp/angle_map.tiff", IMREAD_ANYDEPTH);
    Mat image_fix;
//    image.convertTo(image_fix, CV_8UC1);
//    cout<<type2str(image.type())<<'\n';
    roi_extraction roi("local thresh", 0.6, 2, 11);
    region_growing rg(1, 0.05, 0.45, 10);

    Mat cls_bin = roi.get_roi(cls_map);

    std::map<int, std::vector<std::vector<int>>> regions = rg.get_regions(cls_map, angle_map, cls_bin);
    cout<<regions.size()<<'\n';
    std::vector<std::vector<int>> x = regions[1];
    namedWindow("F", WINDOW_AUTOSIZE);
    imshow("F", cls_bin);
    namedWindow("g", WINDOW_AUTOSIZE);
    imshow("g", angle_map);
    waitKey(0);
    return 0;
}
