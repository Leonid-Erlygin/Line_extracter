//
// Created by leonid on 16.03.2021.
//



#include<vector>
#include <string>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui.hpp>

#include <fstream>
#include <ftw.h>
#include <iostream>

#include "roi_extraction.h"
#include "region_growing.h"
#include "compute_eigen_coords.h"
#include "region_splitter.h"
#include "segment_extractor.h"

#include "lsd_methods.h"

void compute_predictions_and_confidences(int img_index, std::vector<std::vector<int>> &predictions,
                                         std::vector<float> &confidences) {
    cv::Mat cls_map = imread(
            "/home/leonid/trunk/line_segment_detection/for_cpp/cls_map" + std::to_string(img_index) + ".tiff",
            cv::IMREAD_ANYDEPTH);
    cv::Mat angle_map = imread(
            "/home/leonid/trunk/line_segment_detection/for_cpp/angle_map" + std::to_string(img_index) + ".tiff",
            cv::IMREAD_ANYDEPTH);


    roi_extraction roi("local thresh", 0.4, 2, 11);
    region_growing rg(1, 0.05, 0.35, 10);
    compute_eigen_coords compute_eigen;
    segment_extractor seg_extractor;
    region_splitter rs(1, 15,15, 1);

    cv::Mat cls_bin = roi.get_roi(cls_map);

    std::vector<std::vector<std::vector<int>>> regions = rg.get_regions(cls_map, angle_map, cls_bin);
    for (const auto &region : regions) {

        std::vector<std::vector<float>> A = compute_eigen.compute_transformation_matrix(region, cls_map);
        std::vector<std::vector<float>> new_coords = compute_eigen.apply_trans(A, region, cls_map);

        std::pair<std::vector<std::vector<float>>, std::vector<std::vector<std::vector<float>>>> splitted_regions_and_conf = rs.get_splitted_regions_with_conf_and_weighted_mean(
                region, new_coords, cls_map);

        std::vector<std::vector<float>> &means_and_conf = splitted_regions_and_conf.first;
        std::vector<std::vector<std::vector<float>>> &splitted_regions = splitted_regions_and_conf.second;

        for (int j = 0; j < splitted_regions.size(); ++j) {
            float region_mean = means_and_conf[0][j];
            float confidence = means_and_conf[1][j];

            std::vector<std::vector<float>> points_new = seg_extractor.extract(splitted_regions[j],
                                                                               region_mean
            );

            std::vector<std::vector<float>> points = compute_eigen.apply_inverse_trans(A, points_new);


            predictions[0].push_back(int(points[1][0]));
            predictions[1].push_back(int(points[0][0]));
            predictions[2].push_back(int(points[1][1]));
            predictions[3].push_back(int(points[0][1]));
            confidences.push_back(confidence);
        }
    }
}

void evaluation(std::vector<float> &threshes, int data_set_size) {
    for (int i = 0; i < data_set_size; ++i) {

        std::vector<std::vector<int>> predictions(4);
        std::vector<float> confidences;

        compute_predictions_and_confidences(i, predictions, confidences);

        for (auto &thresh:threshes) {
            std::ofstream file;

            std::string path =
                    std::string("/home/leonid/trunk/line_segment_detection/Cpp_pred/") + std::string("raw_pred/") +
                    std::to_string(thresh);
            mkdir(path.c_str(), 0777);
            file.open(
                    path + std::string("/") + std::to_string(i) +
                    ".csv");
            for (int j = 0; j < predictions[0].size(); ++j) {
                if (confidences[j] > thresh) {
                    file << predictions[0][j] << " " << predictions[1][j] << " " << predictions[2][j] << " "
                         << predictions[3][j] << "\n";
                }
            }
            file.close();
        }

    }
}

void speed_test(int img_index, int N) {

    cv::Mat cls_map = imread(
            "/home/leonid/trunk/line_segment_detection/for_cpp/cls_map" + std::to_string(img_index) + ".tiff",
            cv::IMREAD_ANYDEPTH);
    cv::Mat angle_map = imread(
            "/home/leonid/trunk/line_segment_detection/for_cpp/angle_map" + std::to_string(img_index) + ".tiff",
            cv::IMREAD_ANYDEPTH);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {


        std::vector<std::vector<int>> predictions(4);
        std::vector<float> confidenses;


        roi_extraction roi("local thresh", 0.5, 2, 11);
        region_growing rg(1, 0.05, 0.35, 20);
        compute_eigen_coords compute_eigen;
        segment_extractor seg_extractor;
        region_splitter rs(1, 15, 15, 1);

        cv::Mat cls_bin = roi.get_roi(cls_map);

        std::vector<std::vector<std::vector<int>>> regions = rg.get_regions(cls_map, angle_map, cls_bin);
        for (const auto &region : regions) {

            std::vector<std::vector<float>> A = compute_eigen.compute_transformation_matrix(region, cls_map);
            std::vector<std::vector<float>> new_coords = compute_eigen.apply_trans(A, region, cls_map);

            std::pair<std::vector<std::vector<float>>, std::vector<std::vector<std::vector<float>>>>
                    splitted_regions_and_conf = rs.get_splitted_regions_with_conf_and_weighted_mean(
                    region, new_coords, cls_map);

            std::vector<std::vector<float>> &means_and_conf = splitted_regions_and_conf.first;
            std::vector<std::vector<std::vector<float>>> &splitted_regions = splitted_regions_and_conf.second;

            for (int j = 0; j < splitted_regions.size(); ++j) {
                float region_mean = means_and_conf[0][j];
                float confidence = means_and_conf[1][j];

                std::vector<std::vector<float>> points_new = seg_extractor.extract(splitted_regions[j],
                                                                                   region_mean
                );

                std::vector<std::vector<float>> points = compute_eigen.apply_inverse_trans(A, points_new);


                predictions[0].push_back(int(points[1][0]));
                predictions[1].push_back(int(points[0][0]));
                predictions[2].push_back(int(points[1][1]));
                predictions[3].push_back(int(points[0][1]));
                confidenses.push_back(confidence);
            }

        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / N
              << "[µs]" << std::endl;
}

void compute_and_draw_image(int img_index, float thresh, int img_size) {
    std::vector<std::vector<int>> predictions(4);
    std::vector<float> confidenses;

    compute_predictions_and_confidences(img_index, predictions, confidenses);

    cv::Mat final(img_size * 2, img_size * 2, CV_8UC1, cv::Scalar(0, 0, 0));
    for (int p = 0; p < predictions[0].size(); ++p) {
        if (confidenses[p] > thresh) {
            line(final, cv::Point(predictions[0][p] * 2, predictions[1][p] * 2 ),
                 cv::Point(predictions[2][p]*2, predictions[3][p]*2),
                 cv::Scalar(255, 255, 255), 1);
        }
    }
//    line(final, cv::Point(0, img_size - 2 ),
//         cv::Point(img_size*2, img_size-2),
//         cv::Scalar(255,255, 255), 1);
//    line(final, cv::Point(0, img_size + 1 ),
//         cv::Point(img_size*2, img_size + 1),
//         cv::Scalar(255,255, 255), 1);
//
//    line(final, cv::Point(img_size -2, 0 ),
//         cv::Point(img_size -2 , img_size*2),
//         cv::Scalar(255,255, 255), 1);
//
//    line(final, cv::Point(img_size + 1, 0 ),
//         cv::Point(img_size  + 1 , img_size*2),
//         cv::Scalar(255,255, 255), 1);
//    final.at<uchar>(cv::Point(img_size, img_size)) = 255;
//    final.at<uchar>(cv::Point(img_size-1, img_size)) = 255;
//    final.at<uchar>(cv::Point(img_size, img_size-1)) = 255;
//    final.at<uchar>(cv::Point(img_size-1, img_size-1)) = 255;
    namedWindow("F", cv::WINDOW_AUTOSIZE);
    imshow("F", final);
    cv::waitKey(0);

}

//    image.convertTo(image_fix, CV_8UC1);

std::string type2str(int type) {
    /*
     * Usage:
     * cout<<type2str(image.type())<<'\n';
     */
    std::string r;

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

void draw_all_regions_of_img(int img_index) {

    cv::Mat cls_map = imread(
            "/home/leonid/trunk/line_segment_detection/for_cpp/cls_map" + std::to_string(img_index) + ".tiff",
            cv::IMREAD_ANYDEPTH);
    cv::Mat angle_map = imread(
            "/home/leonid/trunk/line_segment_detection/for_cpp/angle_map" + std::to_string(img_index) + ".tiff",
            cv::IMREAD_ANYDEPTH);

    roi_extraction roi("local thresh", 0.5, 2, 11);
    region_growing rg(1, 0.05, 0.35, 25);
    region_splitter rs(1, 15, 15, 1);
    cv::Mat cls_bin = roi.get_roi(cls_map);
    std::vector<std::vector<std::vector<int>>> regions = rg.get_regions(cls_map, angle_map, cls_bin);
    cv::Mat canvas(256, 256, CV_8UC1, cv::Scalar(0));
    int n = regions.size();
    for (int i = 0; i < n; ++i) {
        uchar r = uchar((double(std::rand()) / RAND_MAX) * 255 + 100);
        for (int j = 0; j < regions[i][0].size(); ++j) {
            canvas.at<uchar>(regions[i][0][j], regions[i][1][j]) = 255;
        }
    }
    int img_size = canvas.rows/2;
    line(canvas, cv::Point(0, img_size - 2 ),
         cv::Point(img_size*2, img_size-2),
         cv::Scalar(0), 1);
    line(canvas, cv::Point(0, img_size + 1 ),
         cv::Point(img_size*2, img_size + 1),
         cv::Scalar(0), 1);

    line(canvas, cv::Point(img_size -2, 0 ),
         cv::Point(img_size -2 , img_size*2),
         cv::Scalar(0), 1);

    line(canvas, cv::Point(img_size + 1, 0 ),
         cv::Point(img_size  + 1 , img_size*2),
         cv::Scalar(0), 1);
    std::string name = "F";
    namedWindow(name, cv::WINDOW_AUTOSIZE);
    imshow(name, canvas);
    cv::waitKey(0);
}