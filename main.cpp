
#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "roi_extraction.h"
#include "region_growing.h"
#include "compute_eigen_coords.h"
#include "region_splitter.h"
#include "segment_extractor.h"
#include <chrono>

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
//    image.convertTo(image_fix, CV_8UC1);
//    cout<<type2str(image.type())<<'\n';

int main() {
    float thresh = 0.8;
    int test_image = 4;
    // current avg_time on test_image = 116070 (N = 100) (default)
    // current avg_time on test_image = 108466 (N = 100) (4 neighbors check)
    // current avg_time on test_image = 31702 (N = 100) (default, 256 x 256)
    // current avg_time on test_image = 30031 (N = 500) (default, 256 x 256, 4 neighbors check )
    // current avg_time on test_image = 12347 (N = 1000) (default, 256 x 256, parallel)
    // current avg_time on test_image = 46919 (N = 500) (default, 512 x 512, parallel)
    // current avg_time on test_image = 11454 (N = 1000) (default, 256 x 256, parallel, with region splitter)
    int N = 1;
    for (int i = test_image; i < 22; ++i) {


        Mat cls_map = imread("/home/leonid/trunk/line_segment_detection/for_cpp/cls_map" + to_string(i) + ".tiff",
                             IMREAD_ANYDEPTH);
        Mat angle_map = imread("/home/leonid/trunk/line_segment_detection/for_cpp/angle_map" + to_string(i) + ".tiff",
                               IMREAD_ANYDEPTH);

//        Mat cls_map;
//        Mat angle_map;
//        cv::resize(cls_map0, cls_map, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST );
//        cv::resize(angle_map0, angle_map, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST );

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for (int k = 0; k < N; ++k) {
            vector<vector<int>> predictions(4);
            roi_extraction roi("local thresh", 0.7, 2, 11);
            region_growing rg(1, 0.02, 0.35, 20);
            compute_eigen_coords compute_eigen;
            segment_extractor seg_extractor;
            region_splitter rs(10, 0.2, 0.1);

            Mat cls_bin = roi.get_roi(cls_map);

            std::vector<std::vector<std::vector<int>>> regions = rg.get_regions(cls_map, angle_map, cls_bin);
            int pp = 0;
            for (const auto &region : regions) {
//                cout<<pp<<'\n';
//                if (pp == 6){
//                    int x = 1;
//                }
//                pp++;

                std::vector<std::vector<float>> A = compute_eigen.compute_transformation_matrix(region, cls_map);
                std::vector<std::vector<float>> new_coords = compute_eigen.apply_trans(A, region, cls_map);

                std::pair<std::vector<std::vector<float>>, std::vector<std::vector<std::vector<float>>>> splitted_regions_and_conf = rs.get_splitted_regions_with_conf_and_weighted_mean(region,new_coords,cls_map);

                std::vector<std::vector<float>>& means_and_conf = splitted_regions_and_conf.first;
                std::vector<std::vector<std::vector<float>>>& splitted_regions = splitted_regions_and_conf.second;

//                cout<<splitted_regions.size()<<'\n';
//
//                if (splitted_regions.size() > 1){
//                    cout<<splitted_regions.size()<<'\n';
//                }
                for (int j = 0; j < splitted_regions.size(); ++j) {
                    float region_mean = means_and_conf[0][j];
                    float confidence =  means_and_conf[1][j];

                    std::vector<std::vector<float>> points_new = seg_extractor.extract(splitted_regions[j], region_mean
                    );

                    std::vector<std::vector<float>> points = compute_eigen.apply_inverse_trans(A, points_new);


                    if (confidence > thresh) {
                        predictions[0].push_back(int(points[1][0]));
                        predictions[1].push_back(int(points[0][0]));
                        predictions[2].push_back(int(points[1][1]));
                        predictions[3].push_back(int(points[0][1]));
                    }
                }

            }
            Mat final(cls_map.rows * 2, cls_map.cols * 2, CV_8UC1, Scalar(0, 0, 0));
            for (int p = 0; p < predictions[0].size(); ++p) {
                line(final, Point(predictions[0][p] * 2, predictions[1][p] * 2),
                     Point(predictions[2][p] * 2, predictions[3][p] * 2),
                     Scalar(255, 255, 255), 1);
            }
            namedWindow("F", WINDOW_AUTOSIZE);
            imshow("F", final);
            waitKey(0);
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / N
                  << "[µs]" << std::endl;


        break;
//        ofstream file;
//        file.open ("/home/leonid/trunk/line_segment_detection/Cpp_pred/"+to_string(i)+".csv");
//        for (int j = 0; j < predictions[0].size(); ++j) {
//            file << predictions[0][j] << " "<< predictions[1][j] << " "<< predictions[2][j] << " "<< predictions[3][j] << "\n";
//        }
//
//        file.close();

    }


    return 0;
}
