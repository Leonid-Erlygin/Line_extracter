
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
#include "compute_eigen_coords.h"
#include "segment_extractor.h"
#include <chrono>
#include <random>

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

bool all_unique(std::vector<std::vector<int>> &region) {
    for (int i = 0; i < region[0].size(); ++i) {
        for (int j = 0; j < region[0].size(); ++j) {
            if (i != j and region[0][i] == region[0][j] and region[1][i] == region[1][j]) return false;
        }
    }
    return true;
}

void draw_all_regions(std::map<int, std::vector<std::vector<int>>> &regions, int index = 0) {
    Mat canvas(512, 512, CV_8UC1, Scalar(0));
    int n = regions.size();
    for (int i = 1; i <= n; ++i) {
        if (index != 0 and i != index)continue;
        uchar r = uchar((double(std::rand()) / RAND_MAX) * 255 + 100);
        for (int j = 0; j < regions[i][0].size(); ++j) {
            canvas.at<uchar>(regions[i][0][j], regions[i][1][j]) = 255;
        }
    }
    string name = "F" + to_string(index);
    namedWindow(name, WINDOW_AUTOSIZE);
    imshow(name, canvas);
}


void draw(const cv::Mat &cls_bin) {
    namedWindow("F", WINDOW_AUTOSIZE);
    imshow("F", cls_bin);
    waitKey(0);
}

int main() {
    Mat cls_map = imread("/home/leonid/trunk/line_segment_detection/for_cpp/cls_map.tiff", IMREAD_ANYDEPTH);
    Mat angle_map = imread("/home/leonid/trunk/line_segment_detection/for_cpp/angle_map.tiff", IMREAD_ANYDEPTH);
    Mat image_fix;
//    image.convertTo(image_fix, CV_8UC1);
//    cout<<type2str(image.type())<<'\n';


    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    roi_extraction roi("local thresh", 0.5, 2, 11);
    region_growing rg(1, 0.05, 0.45, 20);
    Mat cls_bin = roi.get_roi(cls_map);
    //draw(cls_bin);
    std::map<int, std::vector<std::vector<int>>> regions = rg.get_regions(cls_map, angle_map, cls_bin);
//    for (int i = 1; i <=regions.size(); ++i) {
//        draw_all_regions(regions, i);
//
//    }
//    waitKey(0);
    compute_eigen_coords compute_eigen;
    segment_extractor seg_extractor;
    Mat final(512, 512, CV_8UC1, Scalar(0, 0, 0));
    for (int i = 1; i <= regions.size(); ++i) {
        std::vector<std::vector<int>> region = regions[i];

        std::vector<std::vector<float>> A = compute_eigen.compute_transformation_matrix(region, cls_map);
        std::vector<std::vector<float>> new_coords = compute_eigen.apply_trans(A, region, cls_map);
        std::vector<std::vector<float>> points_new = seg_extractor.extract(new_coords, compute_eigen.region_mean,
                                                                           compute_eigen.S);
        std::vector<std::vector<float>> points = compute_eigen.apply_inverse_trans(A, points_new);
        line(final, Point(int(points[1][0]), int(points[0][0])), Point(int(points[1][1]), int(points[0][1])),
             Scalar(255, 255, 255), 2);

    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << "[µs]" << std::endl;
    namedWindow("F", WINDOW_AUTOSIZE);
    imshow("F", final);
    draw_all_regions(regions, 0);
    waitKey(0);
    return 0;
}
